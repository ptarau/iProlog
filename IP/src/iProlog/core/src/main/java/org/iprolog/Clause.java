
package org.iprolog;
/**
 * representation of a clause
 */
class Clause {

  final int len; // length of heap slice
  final int[] hgs; // "head+goals pointing to cells in clauses"???
                  // In the video presentation, this seems to be "gs",
                  // described as "the top level skeleton of a clause
                  // containing references to its head then body elements."
                  // Does this mean it's two-element [H,B] though?
                  // From tracing hgs.length, I think not.
  final int base; // the point in the heap where this clause starts
  final int neck; // first after the end of the head (=length of the head)
  final int[] xs; // indexables in head. In the video, this is described as
                  // "the index vector containing dereferenced constants,
                  // numbers or array sizes as extracted from the outermost
                  // term of the head of the clause, with zero values
                  // marking variable positions."
                  // Should it be "outermost termS"?

  Clause(final int len, final int[] hgs, final int base, final int neck, final int[] xs) {
    this.hgs = hgs;
    this.base = base;
    this.len = len;
    this.neck = neck;
    this.xs = xs;
  }
}
