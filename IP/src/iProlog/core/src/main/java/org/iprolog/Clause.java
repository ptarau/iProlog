
package org.iprolog;
/**
 * representation of a clause
 */
class Clause {

  final int len; // length of heap slice
  final int[] hgs; // "head+goals pointing to cells in clauses"???
  final int base; // the point in the heap where this clause starts
  final int neck; // first after the end of the head
  final int[] xs; // indexables in head

  Clause(final int len, final int[] hgs, final int base, final int neck, final int[] xs) {
    this.hgs = hgs;
    this.base = base;
    this.len = len;
    this.neck = neck;
    this.xs = xs;
  }
}
