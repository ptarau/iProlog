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

  /* "For each argument position in the head of a clause
   * (up to a maximum that can be specified by the programmer [MAXIND])
   * it associates to each indexable element(symbol, number or arity)
   * the set of clauses [indicated by clause number]
   * where the indexable element occurs in that argument position."
   */
    // should be array<IMap*,MAXIND> imaps?
    vector<IMap*> imaps;

  /* "The clauses having variables in an indexed argument position are also
   * collected in a separate set for each argument position."
   */
   // should be array<IMap<>*,MAXIND> var_maps?
    vector<IntMap<int,int>*> var_maps;

    long n_matches;

    index() { n_matches = 0; };
    index(const vector<Clause> &clauses);

#define COUNTING_MATCHES

    bool possible_match(const CellStack &h,
                        const t_index_vector& iv0,
                        const t_index_vector& iv1)
#ifndef COUNTING_MATCHES
                                                    const
#endif
                                                         ; 

    void put(const t_index_vector& iv, ClauseNumber clause_no);

    void makeIndexArgs(const CellStack &heap, Spine *G, cell goal);

    // "vector<ClauseNumber>" ?
    vector<int> matching_clauses(const vector<int>& unifiables);

    static inline ClauseNumber to_clause_no(int i)       { return i + 1;      }
    static inline int  to_clause_idx(ClauseNumber cl_no) { return cl_no - 1;  }
    static inline bool is_var_arg(ClauseNumber cl_no)    { return cl_no == 0; }
};

} // namespace
