#pragma once
/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "defs.h"

namespace iProlog {
    /**
     * "representation of a clause" [Clause.java].
     */
    struct Clause {
        // Skeletal elements for compiled form:

        int len;            // length of heap slice
        vector<cell> goal_refs; // "head+goals pointing to cells in clauses"???
        int base;           // the point in the heap where this clause starts
        int neck;           // first after the end of the head (=length of the head)
        t_index_vector index_vector; // indexables in head. In the video, this is described as
                          // "the index vector containing dereferenced constants,
                          // numbers or array sizes as extracted from the outermost
                          // term of the head of the clause, with zero values
                          // marking variable positions."
                          // Should it be "outermost termS"?

        Clause() : index_vector{ -1, -1, -1 }, len(size_t(0)), base(size_t(0)), neck(size_t(0)) {}
        Clause(int len_0, vector<cell> goal_refs_0, int base_0, int neck_0, t_index_vector xs);
    };

} // end namespace