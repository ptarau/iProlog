/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

/*
* From the ICLP 2017 conference paper (P. Tarau)
* 
"The indexing algorithm is designed as an independent add-on to be
plugged into the the main Prolog engine."

This C++ code is an attempt to make iProlog indexing more "plug-in".

"For each argument position in the head of a clause
(up to a maximum that can be specified by the programmer)
it associates to each indexable element (symbol, number or arity)
the set of clauses where the indexable element occurs
in that argument position."

MAXIND is that maximum.
*/

#include <iostream>
#include "index.h"
#include "IntMap.h"
#include "IMap.h"

namespace iProlog {

// "Indexing extensions - ony active if START_INDEX clauses or more."

    index::index(const vector<Clause> &clauses) {

	  // was vcreate in Java version:
		var_maps = vector<IntMap<int,int>*>(MAXIND);

		for (int arg_pos = 0; arg_pos < MAXIND; arg_pos++)
			var_maps[arg_pos] = new IntMap<int,int>();
	  // end vcreate inlined

		if (clauses.size() < START_INDEX) {
			imaps = vector<IMap*>();
			return;
		}

		imaps = IMap::create(MAXIND);

		for (int i = 0; i < clauses.size(); i++)
			put(clauses[i].index_vector, to_clause_no(i));
    }

    inline cell cell2index(const CellStack &heap, cell c) {
		cell x = cell::tag(cell::V_,0);
		int t = c.s_tag();
		switch (t) {
			case cell::R_:
				x = CellStack::getRef(heap,c);
			break;
			case cell::C_:
			case cell::N_:
				x = c;
			break;
		}
		return x;
    }

/**
 * "Tests if the head of a clause, not yet copied to the heap
 * for execution, could possibly match the current goal, an
 * abstraction of which has been placed in [the index vectors]."
 * ("abstraction of which"???)
 * Supposedly, none of these "abstractions" can be -1
 */
	bool index::possible_match( const CellStack &h,
								const t_index_vector& iv0,
							    const t_index_vector& iv1)
#ifndef COUNTING_MATCHES
														 const
#endif
																{
		cout << "possible_match(): ... ";

	// reasonable candidate for loop unrolling:
		for (size_t i = 0; i < MAXIND; i++) {
			cell x = iv0[i];
			cell y = iv1[i];
			if (!x.is_var() && !y.is_var())
				if (!CellStack::isVarLoc_(h,y)) // strange name, it's just an as_int == test
					return false;
		}
#ifdef COUNTING_MATCHES
		n_matches++;
#endif
		return true;
	}

	string show(const t_index_vector& iv) {
		string s = "";
		char d = '<';
		for (int arg_pos = 0; arg_pos < MAXIND; ++arg_pos) {
			s += d;
			s += to_string(iv[arg_pos].as_int());
			d = ',';
		}
		s += ">";
		return s;
	}

    void index::put(const t_index_vector &iv, ClauseNumber cl_no) {

		cout << "    index::put entered with keys=" << show(iv) << endl;

		for (int arg_pos = 0; arg_pos < MAXIND; arg_pos++) {
			cell vec_elt = iv[arg_pos];
			if (!vec_elt.is_var())
	// INDEX PARTLY FAILED BEFORE WHEN CELL SIGN BIT ON
	// Probably because 0 is tag(V_,0) with sign bit off
				bool r = imaps[arg_pos]->put(new Integer(vec_elt), cl_no);
			else
			 /* "If [var_maps[arg_pos]] denotes the set of clauses
			  * having variables in position [arg_pos], then any of them
			  * can also unify with our goal element"
			  */
				var_maps[arg_pos]->add(cl_no);
		}
		cout << "    index::put exiting........" << endl;
    }

/**
 * "Makes, if needed, registers associated to top goal of a Spine.
 * These registers will be reused when matching with candidate clauses.
 * Note that [index_vector] contains dereferenced cells - this is done once for
 * each goal's toplevel subterms." [Engine.java]
 */
    void index::makeIndexArgs(const CellStack &heap, Spine *G, cell goal) {
		if (G->index_vector[0].s_tag() != cell::BAD
		// || !G->hasGoals()
		)
			return;

		int arg_start = 1 + goal.arg(); // point to # of args of goal
		int n_args = CellStack::getRef(heap, goal).arg();
		int n = min(MAXIND, n_args); // # args to compare

		for (int arg_pos = 0; arg_pos < n; arg_pos++) {
			cell arg = CellStack::cell_at(heap, arg_start + arg_pos);
			cell arg_val = CellStack::deref(heap, arg);
			G->index_vector[arg_pos] = cell2index(heap, arg_val);
		}
	/* imaps and var_maps (=vmaps) are not available here,
	 * so the C++ equivalent of the following code
	 * is put just after makeIndexArgs is called
	 * (if indexing is turned on.)
		if (null == imaps)
		  return;
		final int[] cs = IMap.get(imaps, vmaps, xs);
		G.cs = cs;
	*/ 
    }


	/* "When looking for the clauses matching an element of
     * the list of goals to solve, for an indexing element x occurring in position i,
     * we fetch the set Cx,i of clauses associated to it.
     * If Vi denotes the set of clauses having variables in position i,
     * then any of them can also unify with our goal element.
     * Thus we would need to compute the union of the sets Cx,i and Vi
     * for each position i, and then intersect them
     * to obtain the set of matching clauses.
     * We will not actually compute the unions, however.
     * Instead, for each element of the set of clauses corresponding to
     * the gpredicate nameh (position 0), we retain only those which are
     * either in Cx,i or in Vi for each i > 0.
     * We do the same for each element for the set V0 of clauses
     * having variables in predicate positions (if any)." [HHG/ICLP 2017]
     */

	void intersect0(
		IntMap<int, int>& m,
		vector<IntMap<int, int>>& maps,
		vector<IntMap<int, int>>& vmaps,
		vector<int>& r) {
		for (int k = 0; k < m.capacity(); k += m.stride()) {
			bool found = true;
			int key = m.get_key_at(k);
			if (!m.is_free(key)) {
				for (int i = 1; i < maps.capacity(); i++)
					if (!maps[i].contains(key) && !vmaps[i].contains(key)) {
						found = false;
						break;
					}
				if (found)
					r.push_back(key);
			}
		}
	}
	vector<int> index::matching_clauses(const vector<int>& unifiables) {
		vector<IntMap<int, int>*> ms = vector<IntMap<int, int>*>();
		vector<IntMap<int, int>*> vms = vector<IntMap<int, int>*>();

		for (int i = 0; i < MAXIND; i++)
			if (!index::is_var_arg(unifiables[i])) {
				IntMap<int, int>* m = imaps[i]->get(new Integer(unifiables[i]));
				ms.emplace_back(m);
				vms.emplace_back(var_maps[i]);
			}
		
		vector<IntMap<int, int>> ims = vector<IntMap<int, int>>(ms.size());
		vector<IntMap<int, int>> vims = vector<IntMap<int, int>>(vms.size());

		for (int i = 0; i < ims.size(); i++) {
			IntMap<int, int>* im = ms.at(i);
			ims[i] = *im;
			IntMap<int, int>* vim = vms.at(i);
			vims[i] = *vim;
		}

		vector<int> cs; // "$$$ add vmaps here"

		intersect0(ims[0], ims, vims, cs);
		intersect0(vims[0], ims, vims, cs);

		vector<int> is /*= cs.toArray() */; {
			for (int i = 0; i < cs.size(); ++i)
				is.push_back(cs[i]);
		}

		for (int i = 0; i < is.size(); i++) {
			is[i] = to_clause_idx(is[i]);
		}

		/* "Finally we sort the resulting set of clause numbers and
		 * hand it over to the main Prolog engine for unification
		 * and possible unfolding in case of success."
		 */
		std::sort(is.begin(), is.end());

		return is;
	}
} // namespace
