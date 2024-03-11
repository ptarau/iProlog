/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "defs.h"
#include "cell.h"
#include "index.h"
#include "clause.h"

namespace iProlog {

    using namespace std;

    Clause::Clause(int len_0, vector<cell> skeleton_0, int base_0, int neck_0, t_index_vector iv) {
        skeleton = skeleton_0;
        base = base_0;
        len = len_0;
        neck = neck_0;
        index_vector = iv;
    }
}
