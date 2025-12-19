#pragma once
/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "defs.h"
#include "cell.h"
#include "CellList.h"
#include "index.h"
#include "clause.h"

namespace iProlog {
    using namespace std;

    static Clause xp;

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
    private:
        static Spine* free_list;
        Spine* next_free;
    public:
        cell head;      // "head of the clause to which this corresponds" [Spine.java]
        int base;      // "base of the heap where the clause starts" [HHG doc]

        CL_p the_goals;         // goals - "with the top one ready to unfold" [Spine.java]
                            // "immutable list of the locations
                            //  of the goal elements accumulated
                            //  by unfolding clauses so far" [HHG doc]
        int skel_len;

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
        typedef vector<ClauseIndex> Unifiables;
        Unifiables unifiables; // "array of clauses known to be unifiable
                //  with top goal in goal stack" ("cs" in Spine.java)
                // [This is not listed in the HHG description of Spine.]
                // Initialized from unifiables, in Engine. If indexing
                // is not activated, unifiables[i] == i.]

        Spine() {
            next_free = nullptr;
            head = 0;   // head of the clause to which this Spine corresponds
            base = 0L;  // top of the heap when this Spine was created
                        // "base of the heap where the clause starts" [HHG doc]
            the_goals = nullptr;
            skel_len = 0;
            trail_top = 0;  // "top of the trail when this Spine was created"
                            // "as it was when this clause got unified" [HHG doc]
            last_clause_tried = -1; // "index of the last clause [that]
                                    //  the top goal of [this] Spine
                                    //  has tried to match so far" [HHG doc]
            for (int i = 0; i < MAXIND; ++i)
                index_vector[i] = cell::tag(cell::BAD, 0);
					 // index elements ("based on regs" [HHG] but no regs)
                     // "int[] regs: dereferenced goal registers" [HHG doc]
                     // Comments in Engine.java suggest that xs is regs
            unifiables = vector<ClauseIndex>(0);
        }

        /**
         * "Creates a spine - as a snapshot of some runtime elements." [Spine.java]
         */
        static Spine* new_Spine(
            Clause& C0,
            cell b,
            int base_0,               // base
            CL_p goals_0,        // was gs/goal_stack [Java]
            int trail_top_0,
            Unifiables &unifiables_0); // was cs [Java]

        /**
         * "Creates a specialized spine returning an answer 
         * (with no goals left to solve)." [Spine.java]
         */
        static Spine *new_Spine(cell head, int trail_top);

	    inline bool hasGoals() { return the_goals != nullptr; }

        ~Spine();

        inline static Spine *alloc() {
            if (free_list == nullptr)
                return new Spine();
            Spine* r = free_list;
            free_list = r->next_free;
            return r;
        }

        inline static void free(Spine *sp) {
            CellList::collect_n(sp->the_goals, sp->skel_len-1);
            sp->next_free = free_list;
            free_list = sp;
        }

        string show() const;
    };
} // end namespace


