/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "index.h"

namespace iProlog {

// "Indexing extensions - ony active if START_INDEX clauses or more."

    index::index(CellStack &heap, vector<Clause> &clauses) {
	if (clauses.size() < START_INDEX) {
	    imaps = vector<IMap>();
	    return;
	}

// was vcreate:
	var_maps = vector<IntMap>(MAXIND);

	for (int i = 0; i < MAXIND; i++) {
	    var_maps[i] = IntMap();
	}
	imaps = vector<IMap>(var_maps.size());
	for (int i = 0; i < clauses.size(); i++) {
	    put(heap, clauses[i].index_vector, i + 1); // "$$$ UGLY INC"
		// ...because possible_match() is using 0 as "ignore"
        }
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

    void index::put(CellStack &heap, t_index_vector &keys, int val) {

	for (int i = 0; i < imaps.size(); i++) {
	    int key = keys[i];
	    if (key != 0) {
//
// INDEX PARTLY FAILS WITH SIGN BIT ON
//
		IMap::put_(imaps, i, key, val);
	    }
	    else {
		var_maps[i][val] = val;
	    }
	}
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
    }
} // namespace
