package org.iprolog;

/**
 * Runtime representation of an immutable list of goals
 * together with top of heap (base) and trail pointers
 * and current clause tried out by head goal
 * as well as registers associated to it.
 *
 * Note that parts of these immutable lists
 * are shared among alternative branches.
 * ("Note that (most of the) goal elements on this immutable list
 * [of the goal stack] are shared among alternative branches" [HHG doc])
 */
class Spine {

  final int head; // head of the clause to which this Spine corresponds
  final int base; // top of the heap when this Spine was created
                  // "base of the heap where the clause starts" [HHG doc]

  final IntList goal_stack; // with the top one ready to unfold
                            // "immutable list of the locations
                            // of the goal elements accumulated
                            // by unfolding clauses so far" [HHG doc]
  final int trail_top; // top of the trail when this Spine was created
                        // "as it was when this clause got unified" [HHG doc]

  int k; // "index of the last clause [that]
          // the top goal of [this] Spine
          // has tried to match so far " [HHG doc]

  int[] xs; // index elements ("based on regs" [HHG] but no regs)
  // "int[] regs: dereferenced goal registers" [HHG doc]
  // Comments in Engine.java suggest that xs is regs
  
  int[] clauses; // array of clauses known to be unifiable with top goal in goals
          // (This is not listed in HHG description of Spine.)
          // Initialized from cls, in Engine. Not used in any obvious way.
          // Seems like clauses[i] == i, always, from how cls is
          // initialized in Engine.

  /**
   * Creates a spine - as a snapshot of some runtime elements.
   */
  Spine(final int[]   goal_stack_0
       ,final int     base
       ,final IntList goal_stack
       ,final int     trail_top
       ,final int     k
       ,final int[]   clauses
       ) {
    this.head = goal_stack_0[0];
    this.base = base;
    // prepends the goals of clause with head:
    this.goal_stack = IntList.tail(IntList.concat(goal_stack_0, goal_stack));
    this.trail_top = trail_top;
    this.k = k;
    this.clauses = clauses;
  }

  /**
   * Creates a specialized spine returning an answer (with no goals left to solve).
   */
  Spine(final int head, final int trail_top) {
    this.head = head;
    this.base = 0;
    this.goal_stack = IntList.empty;
    this.trail_top = trail_top;
    this.k = -1;
    this.clauses = null;
  }
}
