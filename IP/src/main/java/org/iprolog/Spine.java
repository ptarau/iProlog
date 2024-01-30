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

  final IntList goals; // with the top one ready to unfold
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
  
  int[] cs; // array of clauses known to be unifiable with top goal in goal stack.
          // (This is not listed in the HHG description of Spine.)
          // Initialized from cls, in Engine. If indexing is not
          // activated, cs[i] == i.

  /**
   * Creates a spine - as a snapshot of some runtime elements.
   */
  Spine(final int[]   goal_refs_0
       ,final int     base
       ,final IntList goal_stack
       ,final int     trail_top
       ,final int     k
       ,final int[]   cs
       ) {
    this.head = goal_refs_0[0];
    this.base = base;
    // prepends the goals of clause with head:
    this.goals = IntList.tail(IntList.concat(goal_refs_0, goal_stack));
    this.trail_top = trail_top;
    this.k = k;
    // Prog.println("\n     *** in new Spine() spine.base = " + base + " spine.kount=" + this.k + "\n");
    this.cs = cs;
  }

  /**
   * Creates a specialized spine returning an answer (with no goals left to solve).
   */
  Spine(final int head, final int trail_top) {
    this.head = head;
    this.base = 0;
    this.goals = IntList.empty;
    this.trail_top = trail_top;
    this.k = -1;
    // Prog.println("\n     *** in Spine(h,tt): spine.base = " + this.base + " spine.kount=" + this.k + "\n");
    this.cs = null;
  }
}
