#pragma once
/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "defs.h"
#include "cell.h"
#include "CellList.h"

namespace iProlog {
    using namespace std;
    /**
     * "Runtime representation of an immutable list of goals
     * together with top of heap (base) and trail pointers
     * and current clause tried out by head goal
     * as well as registers associated to it". [iProlog Spine.java]
     * "Registers" meaning deref'd cells for matching on the index.
     *
     * ("Note that (most of the) goal elements on this immutable list
     * [of the goal stack] are shared among alternative branches" [HHG doc])
     */

    class Spine {
    public:
        cell head;      // "head of the clause to which this corresponds" [Spine.java]
        int base;      // "base of the heap where the clause starts" [HHG doc]

        shared_ptr<CellList> goals; // goals - "with the top one ready to unfold" [Spine.java]
                            // "immutable list of the locations
                            //  of the goal elements accumulated
                            //  by unfolding clauses so far" [HHG doc]

        int trail_top;  // "top of the trail when this was created" Spine.java]
                        // "as it was when this clause got unified" [HHG doc]

        int last_clause_tried;  // "index of the last clause [that]
                                //  the top goal of [this] Spine
                                //  has tried to match so far" [HHG doc]

        t_index_vector index_vector;  // "index elements" ("based on regs" [HHG] but no regs)
        // "int[] regs: dereferenced goal registers" [HHG doc]
        // [Comments in Engine.java suggest that this is regs]
        // A note in Engine.java on makeIndexArgs(), which is called
        // only in unfold(), says "xs contains dereferenced cells"

        vector<int> unifiables; // "array of clauses known to be unifiable
        //  with top goal in goal stack" (for "cs" in Spine.java)
              // [This is not listed in the HHG description of Spine.]
              // Initialized from unifiables, in Engine. If indexing
              // is not activated, unifiables[i] == i.]

        Spine() {
            head = 0;   // head of the clause to which this Spine corresponds
            base = 0L;  // top of the heap when this Spine was created
                        // "base of the heap where the clause starts" [HHG doc]
            trail_top = 0;  // "top of the trail when this Spine was created"
                            // "as it was when this clause got unified" [HHG doc]
            last_clause_tried = -1; // "index of the last clause [that]
                                    //  the top goal of [this] Spine
                                    //  has tried to match so far" [HHG doc]
            index_vector = { -1,-1,-1 }; // index elements ("based on regs" [HHG] but no regs)
                                        // "int[] regs: dereferenced goal registers" [HHG doc]
                                        // Comments in Engine.java suggest that xs is regs
            unifiables = vector<int>(0);
        }

        /**
         * "Creates a spine - as a snapshot of some runtime elements." [Spine.java]
         */
        Spine(
            vector<cell> goal_refs_0,       // was gs0/goal_stack_0 [Java]
            int base_0,               // base
            shared_ptr<CellList> goals_0,        // was gs/goal_stack [Java]
            int trail_top_0,
            int k_0,
            vector<int> unifiables_0); // was cs??

        /**
         * "Creates a specialized spine returning an answer (with no goals left to solve)." [Spine.java]
         */
        Spine(cell head, int trail_top);

	inline bool hasGoals() { return goals != nullptr; }
    };
} // end namespace
