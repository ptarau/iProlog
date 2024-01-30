/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "spine.h"

namespace iProlog {

    using namespace std;

    /**
     * Creates a spine - as a snapshot of some runtime elements.
     */
    Spine::Spine(
        vector<cell> goal_refs_0, // was gs0/goal_stack_0 [Java]
        int base_0,               // base
        shared_ptr<CellList> goals_0,        // was gs/goal_stack [Java]
        int trail_top_0,
        int k_0,
        vector<int> unifiables_0)
    {
        head = goal_refs_0[0];
        base = base_0;
        trail_top = trail_top_0;
        index_vector = t_index_vector{ -1,-1,-1 };
        last_clause_tried = k_0; 
        goals = CellList::tail(CellList::concat(goal_refs_0, goals_0));
        unifiables = unifiables_0;
    }

    /**
     * "Creates a specialized spine returning an answer (with no goals left to solve)." {Spine.java]
     */
    Spine::Spine(cell h, int tt) {
        head = h;
        base = 0;
        goals = nullptr;
        trail_top = tt;
        last_clause_tried = -1;
        index_vector = { -1,-1,-1 };
    }

} // end namespace
