#pragma once
/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "skeleton.h"

namespace iProlog {
    using namespace std;

    /**
     * "representation of a clause" [Clause.java].
     */

    class Clause {
        // Skeletal elements for compiled form:
    public:
        int len;            // length of heap slice
        clause_skel skel;   // "head+goals pointing to cells in clauses"
        int skel_len;       // (for raw array implementation)
        int base;           // the point in the heap where this clause starts
        int neck;           // first after the end of the head (=length of the head)
        t_index_vector index_vector; // indexables in head. In the video, this is described as
                          // "the index vector containing dereferenced constants,
                          // numbers or array sizes as extracted from the outermost
                          // term of the head of the clause, with zero values
                          // marking variable positions."
                          // Should it be "outermost termS"?

        Clause() : len(size_t(0)), skel_len(0), base(size_t(0)), neck(size_t(0)) {
            for (int i = 0; i < IV_LEN; ++i)
                index_vector[i] = cell::null();
            init_skel(skel);
        }

        Clause(int len_0, clause_skel skel_0, size_t skel_len_0, int base_0, int neck_0);
    };

} // end namespace
