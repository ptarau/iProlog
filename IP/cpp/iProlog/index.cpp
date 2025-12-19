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
/// #include "index.h"
#include "Engine.h"

namespace iProlog {

	cell index::cell2index(CellStack& heap, cell c) const {
		cell x = cell::abstr_var();
		int t = c.s_tag();
		switch (t) {
		case cell::R_:
			x = Engine::getRef(heap, c);
			break;
		case cell::C_:
		case cell::N_:
			x = c;
			break;
		}
		return x;
	}

// "Indexing extensions - ony active if [there are] START_INDEX clauses or more."

	inline void get_arg_start_and_n(CellStack& h, cell g, int &arg_start, int &n) {
		arg_start = 1 + g.arg(); // "1" to be just after pred symbol ref
		n = min(Engine::getRef(h, g).arg(), MAXIND);
	}

	void index::getIndexables(t_index_vector &index_vector, CellStack &heap, cell goal) {
#define TR if(0)
		TR cout << "getIndexables(...): " << endl;

		int arg_start, n;
		get_arg_start_and_n(heap, goal, arg_start, n);

		for (int arg_pos = 0; arg_pos < n; arg_pos++) {
			cell arg = Engine::cell_at(heap,arg_start + arg_pos);
			cell c = Engine::deref(heap, arg);
			index_vector[arg_pos] = cell2index(heap, c);

			TR cout << "getIndexables: index_vector[" << arg_pos << "] <- " << index_vector[arg_pos].as_int() << endl;
		}
		for (int pos = n; pos < IV_LEN; ++pos) // sentinel-searched
			index_vector[pos] = cell::null();

		TR cout << "getIndexables returning" << endl;
	}

    index::index(CellStack &heap, vector<Clause>& clauses) {
#define TR if(0)

		for (int i = 0; i < clauses.size(); ++i) {
			cell hd = clauses[i].skel[0];
			TR cout << " clauses[" << i << "].skel[0]=" << hd.show() << endl;
			getIndexables(clauses[i].index_vector, heap, hd);
		}

	  // was vcreate() in Java version:
		// var_maps = vector<cls_no_set*>(IV_LEN);

		for (int arg_pos = 0; arg_pos < IV_LEN; arg_pos++) {
			var_maps[arg_pos] = new cls_no_set();
		}
	  // end vcreate() inlined

		if (clauses.size() < START_INDEX) {
#ifndef RAW_IMAPS
			imaps = vector<IMap>();
#endif
			return;
		}
#ifndef RAW_IMAPS
		imaps = IMap::create(IV_LEN);
#else
		for (int i = 0; i < IV_LEN; ++i) {
			imaps[i] = IMap();
		}
#endif

		for (int i = 0; i < clauses.size(); i++) {
			ClauseIndex ci(i);
			TR cout << "in index ctor loop, i = " << to_string(i) << endl;
			put(clauses[i].index_vector, to_clause_no(ci));  // "$$$ UGLY INC" in Java code
		}
#undef TR
    }

/**
 * "Tests if the head of a clause, not yet copied to the heap
 * for execution, could possibly match the current goal, an
 * abstraction of which has been placed in [the index vectors]."
 * ("abstraction of which" seems to refer to the variable
 * positions. With the original tag format, these are V_:0 == 0)
 */

// Reasonable candidate for loop unrolling.
// Sentinel search could also work if the sentinel
// position is temporarily give two different
// values in the two vectors. Only called from
// one place in unfold(), so also a reasonable
// candidate for inlining.

	bool index::possible_match(const Spine* sp,
							   const Clause& cl)
#ifndef COUNTING_MATCHES
											     const
#endif
	{
	if (!indexing) return true;

		for (size_t i = 0; i < MAXIND; i++) {
			cell x = sp->index_vector[i];
			cell y = cl.index_vector[i];

			if (x == cell::abstr_var() || y == cell::abstr_var())
				continue;
			if (x != y)
				break;
		}
#ifdef COUNTING_MATCHES
		n_matches++;
#endif
		return true;
	}

	/* Tarau's IMap.java: K = Integer, a Java object reference.
	 *  "key": index vector element.
	 *  val : clause number (not index)
	 * Java automagically "boxes" key to an Integer.
	 * So we need to hash in imaps on the vector elt?
	 * How does this work with matching clauses in indexing?
	/*
	final boolean put(final K key, final int val) {
		IntSet vals = map.get(key);
		if (null == vals) {
			vals = new IntSet();
			map.put(key, vals);
		}
		return vals.add(val);
	}
	*/

	void index::put(const t_index_vector &iv, ClauseNumber cls_no) {

#define TR if(0)
		for (int arg_pos = 0; arg_pos < MAXIND; arg_pos++) {
			cell vec_elt = iv[arg_pos];

			TR cout << "  arg_pos = " << to_string(arg_pos) 
			<< " vec_elt = " << to_string(vec_elt.as_int()) << endl;

			if (!(vec_elt == cell::null()) && !vec_elt.is_var()) {  // in Java code, basically: != tag(V_,0) 

				// INDEX PARTLY FAILED BEFORE WHEN CELL SIGN BIT ON
				// Probably because 0 is tag(V_,0) with sign bit off

				TR cout << "    imaps[" << arg_pos << "].put("
					<< cls_no.as_int() << ","
					<< vec_elt.as_int() << ")" << endl;
				imaps[arg_pos].put(cls_no, vec_elt);
			}
			else {
				// this can include vec_elt == cell::null case, but maybe that's OK
				/* "If [var_maps[arg_pos]] denotes the set of clauses
				 * having variables in position [arg_pos], then any of them
				 * can also unify with our goal element"
				 */
				int c = cls_no.as_int();
	
				TR cout << "    var_maps[" << arg_pos << "].add_key(" << c << ")" << endl;
				var_maps[arg_pos]->add_key(c);
				assert(var_maps[arg_pos]->contains(c));
			}
		}
#undef TR
    }

/**
 * "Makes, if needed, registers associated to top goal of a Spine.
 * These registers will be reused when matching with candidate clauses.
 * Note that [index_vector] contains dereferenced cells - this is done once for
 * each goal's toplevel subterms." [Engine.java]
 * This is in Engine, on-demand construction of index args,
 * but it could be in initialization.
 * The initial tests could be in an inline member function.
 */
    void index::makeIndexArgs(CellStack &heap, Spine *G, cell goal) {
#define TR if(0)
		if (!indexing) return;

		if (!(G->index_vector[0] == cell::null())
		   || !G->hasGoals()
		)
			return;

		getIndexables(G->index_vector, heap, goal);
		G->unifiables = matching_clauses_(G->index_vector);
#undef TR
    }
	
// copypasta from 
//  https://prepinsta.com/data-structures-algorithms/sorting-of-array/
// Ridiculous that I have to do this, but
// (1) with the shorter vectors in C++, I needed to use std::sort()
//     but the CPU+RAM overhead of the vector is high compared to the
//     actual sorting steps and in-place sort memory requirements;
// (2) even C's sort() takes a pointer to a comparison function,
//     when all that's needed here is int comparison;
// (3) sorting seems to be called only once in all of this code
//     so the sort function can even be inlined.
// 
// Steps to add:
//   Algorithm: special-casing of arrays of length 1 and 2,
//              maybe even 3.
//   In use:    separately sort the two results of calling intersect
//              then do mergesort on them, although this can't be
//				done in-place.
// 
// 

	// Function to swap two elements
	inline void swap(ClauseNumber& a, ClauseNumber& b) {
		ClauseNumber temp = a;
		a = b;
		b = temp;
	}

	// Partition the array and return the pivot index
	inline int partition(ClauseNumber *arr, int low, int high) {
		ClauseNumber pivot = arr[high]; // Choose the rightmost element as the pivot
		int i = low - 1; // Index of the smaller element

		for (int j = low; j <= high - 1; j++) {
			if (arr[j].as_int() <= pivot.as_int()) {
				i++;
				swap(arr[i], arr[j]);
			}
		}

		swap(arr[i + 1], arr[high]);
		return i + 1; // Return the pivot index
	}

	// "Quick sort implementation"
	inline void quickSort(ClauseNumber *arr, int low, int high) {
		if (low < high) {
			int pivotIndex = partition(arr, low, high);

			// Recursively sort the left and right subarrays
			quickSort(arr, low, pivotIndex - 1);
			quickSort(arr, pivotIndex + 1, high);
		}
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

	typedef cls_no_set *cls_no_set_vec[IV_LEN];

	ClauseNumber* intersect0_p( const cls_no_set* m,      // maps[0] or vmaps[0]
								const cls_no_set_vec& maps,
								const cls_no_set_vec& vmaps,
								ClauseNumber* cls_nos_p,
								int push_count) {
#define TR if(0)

		TR cout << "     intersect0_p: m->length()=" << to_string(m->length()) << endl;

		ClauseNumber* cnp = cls_nos_p;

		TR cout << "       m->length()=" << to_string(m->length()) << endl;

		for (int k = 0; k < m->length(); k++) {
			if (!m->is_free(k)) {
				ClauseNumber cn = m->get_key_at(k);
				TR cout << "cn = " << to_string(cn.as_int()) << endl;
				bool found = true;
				for (int i = 1; i < push_count; i++) {
					TR cout << "i loop: i = " << to_string(i) << endl;
					ClauseNumber v0;
					if (maps[i] == 0) v0 = cls_no_set::no_value();
					else v0 = maps[i]->get(cn.as_int());
					TR cout << "      v = " << to_string(v0.as_int()) << endl;
					if (v0 == cls_no_set::no_value()) {
						ClauseNumber v1;
						if (vmaps[i] == nullptr) v1 = cls_no_set::no_value();
						else v1 = vmaps[i]->get(cn.as_int());
						if (v1 == cls_no_set::no_value()) {
							found = false;
							break;
						}
					}
				}
				if (found) {
					TR cout << "      at data[" << to_string(k) << "] found key="
						<< to_string(cn.as_int()) << " to push to result" << endl;
					*cnp++ = cn;
				}
			}
		}

		TR cout << "MAXIND*push_count=" << to_string(MAXIND*push_count) << " cnp - cls_nos_p = " << cnp - cls_nos_p << endl;
		return cnp;
#undef TR
	}
	
	/*
	 * This translation is from IMap.get, with ArrayList for ms & vms 
	 */
 // The iv loop continue condition was "== 0" in Java code.
 // In that code, since V_ cells are never zero (even though V_ == 0),
 // zero may be a kind of null- terminator for index vectors
 // that are shorter than MAXIND.
 // This works because V_ cells can't be at index 0 in the heapf.
 // 
 // This complicates the idea of changing to relative addressing
 // in variables, with a zero offset indicating that dereferencing
 // has ended at a variable. I'll keep cell::BAD for now.
 // If this is true, the mystery is why there's no loop break
 // on zero.

	vector<ClauseIndex> index::matching_clauses_(t_index_vector& iv) {
#define TR if(0)
		TR cout << "Entering matching_clauses, IV_LEN=" << to_string(IV_LEN) << endl;

		cls_no_set_vec msp;
		cls_no_set_vec vmsp;

		// init empty placeholders
		for (int i = 0; i < IV_LEN; ++i)
				msp[i] = vmsp[i] = nullptr;

		TR cout << " ==== matching_clauses: start iv loop, imaps.size()=" << imaps.size() << endl;

		int push_count = 0;

		for (int i = 0; /*i < MAXIND*/; i++) { // sentinel stops this
			if (iv[i] == cell::abstr_var())
				continue;
			else if (iv[i] == cell::null()) // "index vectors are null-terminated if < MAXIND"
				break;
			else {
				TR cout << "OK, starting to add to msp & vmsp with i=" << to_string(i) << endl;
				cls_no_set* m = imaps[i].map[iv[i]];
				msp[push_count] = m;
				vmsp[push_count] = var_maps[i];
				++push_count;

				TR cout << "  iv[" << i << "]=" << iv[i].show()
				        << ", ms:" << (m==nullptr? "_" : m->show())
				        << ", vms:" << (var_maps[i]==nullptr? "_" : var_maps[i]->show())
				        << ", push_count =" << push_count << endl;
			}
		}

		TR cout << " ==== matching_clauses: rest of processing" << endl;

		// vector<ClauseNumber> cs;

		int n_to_alloc = MAXIND * push_count; // ????

		auto csp = (ClauseNumber*) _malloca(n_to_alloc * sizeof(ClauseNumber));
		if (csp == nullptr) abort();
		// cout << "csp len = " << n_to_alloc << endl;

		// was IntMap.java intersect, expanded here:
		TR cout << "  msp[0].m_size=" << to_string(msp[0]->size()) << endl;
		ClauseNumber *new_end = intersect0_p(msp[0], msp, vmsp, csp, push_count);

		TR cout << "  vms[0].m_size=" << to_string(vmsp[0]->size()) << endl;
		new_end = intersect0_p(vmsp[0], msp, vmsp, new_end, push_count);
		long cs_size = new_end - csp;

		if (cs_size > n_to_alloc)
			abort();

#ifdef RAW_QUICKSORT
		quickSort(csp, 0, cs_size - 1);
#endif
		// cout << "  after intersect0_p new_end - csp=" << cs_size << endl;

		// is: clause numbers converted to indices
		vector<ClauseIndex> is;	  /*= cs.toArray() in Java, emulated here but
									* with conversion to indices. Could
									* probably be done on-the-fly in intersect0. */
		is.reserve(cs_size);

		TR cout << "  (1) is.size()=" << is.size() << endl;
		TR cout << "  (1) is.capacity()=" << is.capacity() << endl;

		for (int i = 0; i < cs_size; ++i)
			is.emplace_back(to_clause_idx(csp[i]));

		TR cout << "  is.size()=" << to_string(is.size()) << endl;

		/* "Finally we sort the resulting set of clause numbers and
			* hand it over to the main Prolog engine for unification
			* and possible unfolding in case of success."
			*
			* I.e., respect standard Prolog clause ordering.
			*/
		TR cout << "  (2) is.size()=" << is.size() << endl;
		TR cout << "  (2) is.capacity()=" << is.capacity() << endl;
#ifndef RAW_QUICKSORT
		if (is.size() > 1)
			std::sort(is.begin(), is.end());
#endif
		TR for (int i = 0; i < cs_size; ++i) {
			cout << "   is[" << i << "]=" << is[i].as_int() << endl;
		}

		TR cout << " ==== matching_clauses: exiting" << endl << endl;

		_freea((void*)csp);

		return is;
#undef TR
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

	string index::show() const {
		string s = "index::show():";

		for (int i = 0; i < MAXIND; ++i) {
			s += "\n  INDEX: [" + to_string(i);
			s += "]";
			s += "\n    imaps:    " + imaps[i].show();
			s += "\n    var_maps: " + var_maps[i]->show();
		}

		s += "\n";
		return s;
	}
} // namespace
