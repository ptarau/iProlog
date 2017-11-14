package iProlog;

/**
 * representation of a clause
 */
class Clause {
    final int len;
    final int[] hgs;
    final int base;
    final int neck;
    final int[] xs;
    Clause(int len, int[] hgs, int base, int neck, int[] xs) {
        this.hgs = hgs; // head+goals pointing to cells in cs
        this.base = base; // heap where this starts
        this.len = len; // length of heap slice
        this.neck = neck; // first after the end of the head
        this.xs = xs; // indexables in head
    }
}
