/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "defs.h"
#include "cell.h"
#include "index.h"
#include "clause.h"
#include "spine.h"

namespace iProlog {

    using namespace std;

    // need head cell plus body array, hg_len becomes . . . body length?
    Clause::Clause(int len_0, clause_skel skel_0, size_t skel_len_0, int base_0, int neck_0) {
        skel = skel_0;
        skel_len = skel_len_0;
        if (skel_len == 0) abort();
        base = base_0;
        len = len_0;
        neck = neck_0;
        for (int i = 0; i < IV_LEN; ++i)
            index_vector[i] = cell::null();
    }
}
