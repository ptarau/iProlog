#pragma once
/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

using namespace std;

#include <vector>
#include <array>
#include "cell.h"

namespace iProlog {

    const bool indexing = false;

    const int MAXIND = 3;       // "number of index args" [Engine.java]
    const int START_INDEX = 1;	// "if # of clauses < START_INDEX,
				// turn off indexing" [Engine.java]

    typedef array<cell, MAXIND> t_index_vector; // deref'd cells
}

#include "IMap.h"
#include "clause.h"
#include "spine.h"
#include "CellStack.h"

namespace iProlog {

class Spine;

class index {
public:
    vector<IMap*> imaps;
    vector<IntMap<int>*> var_maps;

    long n_matches = 0;
    index() { n_matches = 0; };
    index(vector<Clause> &cls);

    inline bool possible_match( t_index_vector& iv0,
				t_index_vector &iv1) {
// cout<<"possible_match():" << endl;
        for (size_t i = 0; i < MAXIND; i++) {
            cell x = iv0[i];
            cell y = iv1[i];
// cout<<"   try x=" << x << " y=" << y << endl;
            if (cell::isVAR(x) || cell::isVAR(y))
	    				// but possible to deref down to var? is 0 val special?
                continue;
            if (x.as_int() != y.as_int()) // shd be comp op
                return false;
        }
// cout << "**** MATCH FOUND *****" << endl;
	n_matches++;
        return true;
    }

    void put(t_index_vector& keys, int val);

    void makeIndexArgs(CellStack &heap, Spine *G, cell goal);
};

} // namespace
