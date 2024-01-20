/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include <iostream>
#include "index.h"

namespace iProlog {

// "Indexing extensions - ony active if START_INDEX clauses or more."

    index::index(vector<Clause> &clauses) {

    cout<<"Entering index() constr...."<<endl;

	// was vcreate in Java version:
	var_maps = vector<IntMap<int>*>(MAXIND);

	for (int i = 0; i < MAXIND; i++) {
	    cout << "  index constr, before new IntMap()" << endl;
	    var_maps[i] = new IntMap<int>();
	    cout << "  In index::index() constr, var_maps["<<i<<"]->size()="
	         << var_maps[i]->size() << endl;
	}
	// end vcreate inlined

	// was index() in Java version
	if (clauses.size() < START_INDEX) {
	    imaps = vector<IMap*>();
	    return;
	}

	// var_maps.size() should be MAXIND as ell
	imaps = IMap::create(var_maps.size());

	for (int i = 0; i < clauses.size(); i++) {
	    put(clauses[i].index_vector, i + 1); // "$$$ UGLY INC"
		// ...because possible_match() is using 0 as "ignore"
	    ;
        }
	cout<<"Exiting index() constr."<<endl;
    }

    inline cell cell2index(CellStack &heap, cell c) {
	cell x = 0; // wildcard
	int t = cell::tagOf(c);
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

    void index::put(t_index_vector &keys, int val) {

cout << "    index::put entered........" << endl;
if(imaps.size()!=MAXIND) abort();

	for (int i = 0; i < imaps.size(); i++) {
	    int key = keys[i];
	    if (key != 0) {
//
// INDEX PARTLY FAILS WITH SIGN BIT ON
// Probably because 0 is tag(V_,0) with sign bit off
//
		IMap::put_(imaps, i, key, val);
	    }
	    else {
		var_maps[i]->add(val);
	    }
	}
cout << "    index::put exiting........" << endl;
    }

    void index::makeIndexArgs(CellStack &heap, Spine *G, cell goal) {
	if (G->index_vector[0] != -1 || !G->hasGoals())
	    return;

	int p = 1 + cell::detag(goal); // point to # of args of goal
	int n_args = cell::detag(CellStack::getRef(heap, goal));
	int n = min(MAXIND, n_args); // # args to compare

	for (int i = 0; i < n; i++) {
	    cell arg_val = CellStack::deref(heap, CellStack::cell_at(heap, p + i));
	    G->index_vector[i] = cell2index(heap, arg_val).as_int();
	}
/* imaps and var_maps (=vmaps) are not available here,
 * so this code is put just after makeIndexArgs is called
 * (if indexing is turned on.)
    if (null == imaps)
      return;
    final int[] cs = IMap.get(imaps, vmaps, xs);
    G.cs = cs;
*/ 
    }
} // namespace
