/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "defs.h"
#include "cell.h"
#include "clause.h"
#include "prog.h"

namespace iProlog {

    using namespace std;

    Clause::Clause(int len_0, vector<cell> goal_refs_0, int base_0, int neck_0, t_index_vector xs) {
#if 0
        std::cout << "\nIn Clause() call:\n" << endl;
#endif
        goal_refs = goal_refs_0;
        base = base_0;
#if 0
        cout << "\n     $$$$$$$$$$$$$$$$$$ Clause constructor: base<-" << base << endl << endl;
#endif
        len = len_0;
        neck = neck_0;
        index_vector = xs;
    }
}
