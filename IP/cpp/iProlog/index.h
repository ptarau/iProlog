#pragma once
/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "defs.h"
#include "IMap.h"
#include "clause.h"
#include "spine.h"
#include "CellStack.h"

namespace iProlog {


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
            int x = iv0[i];
            int y = iv1[i];
// cout<<"   try x=" << x << " y=" << y << endl;
            if (0 == x || 0 == y)
                continue;
            if (x != y)
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
