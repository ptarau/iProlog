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
#include <assert.h>
// #include "index.h"
#include "Engine.h"

namespace iProlog {

// "Indexing extensions - ony active if [there are] START_INDEX clauses or more."

	t_index_vector index::getIndexables(cell goal) {
		int arg_start = 1 + goal.arg();
		int n_args = eng->getRef(goal).arg();
		int n = min(n_args, MAXIND);

		cout << "getIndexables: n_args=" << n_args << endl;
		t_index_vector index_vector;

		for (int i = 0; i < MAXIND; ++i)
			index_vector[i] = cell::tag(cell::BAD, 0);

		for (int arg_pos = 0; arg_pos < n; arg_pos++) {
			cell arg = eng->cell_at(arg_start + arg_pos);
			cell c = eng->deref(arg);

			index_vector[arg_pos] = cell2index(c).as_int();

			cout << "getIndexables: index_vector[" << arg_pos << "] <- " << index_vector[arg_pos].as_int() << endl;
		}
		return index_vector;
	}

    index::index(Engine *e) {

		eng = e;

		for (int i = 0; i < e->clauses.size(); ++i)
			e->clauses[i].index_vector = getIndexables(e->clauses[i].skeleton[0]);

	  // was vcreate in Java version:
		var_maps = vector<clause_no_to_int>(MAXIND);

		for (int arg_pos = 0; arg_pos < MAXIND; arg_pos++)
			var_maps[arg_pos] = clause_no_to_int();
	  // end vcreate inlined

		if (e->clauses.size() < START_INDEX) {
			imaps = vector<IMap*>();
			return;
		}

		imaps = IMap::create(MAXIND);

		for (int i = 0; i < e->clauses.size(); i++)
			put(e->clauses[i].index_vector, to_clause_no(i));
    }

    cell index::cell2index(cell c) const {
		
		cell x = cell::tag(cell::V_,0);
		int t = c.s_tag();
		switch (t) {
			case cell::R_:
				x = eng->getRef(c);
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
	bool index::possible_match(	const t_index_vector& iv0,
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

			if (x == cell::tag(cell::V_, 0) || y == cell::tag(cell::V_, 0))
				continue;
			if (x != y)
				break;
		}
#ifdef COUNTING_MATCHES
		n_matches++;
#endif
		return true;
	}

    void index::put(const t_index_vector &iv, ClauseNumber cl_no) {

		cout << "    index::put entered with keys=" << show(iv) << endl;

		for (int arg_pos = 0; arg_pos < MAXIND; arg_pos++) {
			cell vec_elt = iv[arg_pos];
			if (!vec_elt.is_var()) {

				// INDEX PARTLY FAILED BEFORE WHEN CELL SIGN BIT ON
				// Probably because 0 is tag(V_,0) with sign bit off

				// Not clear why new Integer every time
				imaps[arg_pos]->put(new Integer(cl_no), vec_elt.as_int());
			}
			else
			 /* "If [var_maps[arg_pos]] denotes the set of clauses
			  * having variables in position [arg_pos], then any of them
			  * can also unify with our goal element"
			  */
				var_maps[arg_pos].add(cl_no);
		}
		cout << "    index::put exiting........" << endl;
    }

/**
 * "Makes, if needed, registers associated to top goal of a Spine.
 * These registers will be reused when matching with candidate clauses.
 * Note that [index_vector] contains dereferenced cells - this is done once for
 * each goal's toplevel subterms." [Engine.java]
 */
    void index::makeIndexArgs(Spine *G, cell goal) {
		if (G->index_vector[0].s_tag() != cell::BAD
		// || !G->hasGoals()
		)
			return;

		int arg_start = 1 + goal.arg(); // point to # of args of goal
		int n_args = eng->getRef(goal).arg();
		int n = min(MAXIND, n_args); // # args to compare

		for (int arg_pos = 0; arg_pos < n; arg_pos++) {
			cell arg = eng->cell_at(arg_start + arg_pos);
			cell arg_val = eng->deref(arg);
			G->index_vector[arg_pos] = cell2index(arg_val);
		}

		G->unifiables = matching_clauses(G->unifiables);
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
		clause_no_to_int& m,
		vector<clause_no_to_int>& maps,
		vector<clause_no_to_int>& vmaps,
		vector<ClauseNumber>& cl_nos) {

	// r: clause index array
#if 0
		assert(maps.capacity() == MAXIND);
		assert(vmaps.capacity() == MAXIND);
#endif
		for (int i = 0; i < m.capacity(); i += m.stride()) {
			bool found = true;
			ClauseNumber cl_no = m.get_key_at(i);
			
			if (m.is_free(cl_no))
				continue;
			for (int arg_pos = 1; arg_pos < MAXIND; arg_pos++) {
				int cval = maps[arg_pos].get(cl_no);
				if (cval == clause_no_to_int::NO_VALUE) {
					int vcval = vmaps[arg_pos].get(cl_no);
					if (vcval == clause_no_to_int::NO_VALUE) {
						found = false;
						break;
					}
				}
			}
			if (found)
				cl_nos.push_back(cl_no);
		}
	}

	/*
	 * This translation is from IMap.get, with ArrayList for ms & vms 
	 */
	vector<int> index::matching_clauses(const vector<ClauseNumber>& unifiables) {
		vector<clause_no_to_int> ms;
		vector<clause_no_to_int> vms;

		for (int i = 0; i < unifiables.size(); i++)
			if (is_not_cl_no(unifiables[i])) {  // should be legal ClauseNumber
				
				clause_no_to_int* m = imaps[i]->get(new Integer(unifiables[i]));

				ms.emplace_back(*m);
				vms.emplace_back(var_maps[i]);
			}
		
		vector<clause_no_to_int> ims  = vector<clause_no_to_int>(ms.size());
		vector<clause_no_to_int> vims = vector<clause_no_to_int>(vms.size());

		for (int i = 0; i < ims.size(); i++) {
			ims[i] = ms[i];
			vims[i] = vms[i];
		}

		// cs: receives the clause numbers:
		vector<ClauseNumber> cs; // "$$$ add vmaps here"

		// was IntMap.java intersect, expanded here:
		intersect0(ims[0],  ims, vims, cs);
		intersect0(vims[0], ims, vims, cs);

		// is: clause numbers converted to indices
		vector<int> is;
		/*= cs.toArray() in Java, emulated here but
		 * with conversion to indices. Could
		 * probably be done on-the-fly in intersect0.
		 */
			for (int i = 0; i < cs.size(); ++i)
				is.push_back(to_clause_idx(cs[i]));

		/* "Finally we sort the resulting set of clause numbers and
		 * hand it over to the main Prolog engine for unification
		 * and possible unfolding in case of success."
		 * 
		 * I.e., respect standard Prolog clause ordering.
		 */
		std::sort(is.begin(), is.end());

		return is;
	}

	string index::show(const t_index_vector& iv) const {
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
} // namespace
