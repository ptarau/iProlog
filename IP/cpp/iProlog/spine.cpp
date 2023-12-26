/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "defs.h"
#include "cell.h"
#include "spine.h"

namespace iProlog {

    using namespace std;

    /**
     * Creates a spine - as a snapshot of some runtime elements.
     */
    Spine::Spine(
        vector<cell> goal_refs_0, // was gs0/goal_stack_0 [Java]
        size_t base_0,               // base
        CellList * goals_0,        // was gs/goal_stack [Java]
        int trail_top_0,
        int k_0,
        vector<size_t> unifiables_0)
    {
        head = goal_refs_0[0];
        base = base_0;
        trail_top = trail_top_0;
        index_vector = t_index_vector{ -1,-1,-1 };
        kount = k_0; // "index of the last clause [that]
                    //  the top goal of [this] Spine
                    //  has tried to match so far" [HHG doc]
#if 0
        cout << endl << "     *** in new Spine() spine.base = " << base << " spine.kount" << kount << endl << endl;
#endif
        goals = CellList::tail(CellList::concat(goal_refs_0, goals_0));
                    // Really need to understand why it was "tail" here.
                    // Ignore the first in the list? Why? We already have
                    // "head = goals_0[0]" above. I see this again in
                    // unfold(). 
        // if (goals == nullptr) abort();
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
        kount = -1; // "index of the last clause [that]
                    //  the top goal of [this] Spine
                    //  has tried to match so far" [HHG doc]
#if 0
        cout << endl << "     *** in Spine(h,tt): spine.base = " << base << " spine.kount" << kount << endl << endl;
#endif
        index_vector = { -1,-1,-1 };
    }

} // end namespace
