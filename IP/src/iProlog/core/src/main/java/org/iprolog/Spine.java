package org.iprolog;

/**
 * Runtime representation of an immutable list of goals
 * together with top of heap (base) and trail pointers
 * and current clause tried out by head goal
 * as well as registers associated to it.
 *
 * Note that parts of these immutable lists
 * are shared among alternative branches.
 */
class Spine {

  final int head; // head of the clause to which this Spine corresponds
  final int base; // top of the heap when this Spine was created

  final IntList goals; // goals - with the top one ready to unfold
  final int trail_top; // top of the trail when this Spine was created

  int k;

  int[] xs; // index elements
  int[] clauses; // array of clauses known to be unifiable with top goal in goals

  /**
   * Creates a spine - as a snapshot of some runtime elements.
   */
  Spine(final int[] goals0, final int base, final IntList goals, final int trail_top, final int k, final int[] cs) {
    head = goals0[0];
    this.base = base;

    // prepends the goals of clause with head hs:
    this.goals = IntList.tail(IntList.concat(goals0, goals));

    this.trail_top = trail_top;
    this.k = k;
    this.clauses = cs;
  }

  /**
   * Creates a specialized spine returning an answer (with no goals left to solve).
   */
  Spine(final int head, final int trail_top) {
    this.head = head;
    base = 0;
    goals = IntList.empty;
    this.trail_top = trail_top;

    k = -1;
    clauses = null;
  }
}
