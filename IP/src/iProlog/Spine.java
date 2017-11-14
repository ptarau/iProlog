package iProlog;

/**
 * runtime representation of an immutable list of goals
 * together with top of heap and trail pointers
 * and current clause tried out by head goal
 * as well as registers associated to it
 * <p>
 * note that parts of this immutable lists
 * are shared among alternative branches
 */
class Spine {

    final int hd; // head of the clause to which this corresponds
    final int base; // top of the heap when this was created
    final IntList gs; // goals - with the top one ready to unfold
    final int ttop; // top of the trail when this was created
    int k;
    int[] xs; // index elements
    int[] cs; // array of  clauses known to be unifiable with top goal in gs

    /**
     * creates a spine - as a snapshot of some runtime elements
     */
    Spine(int[] gs0, int base, IntList gs, int ttop, int k, int[] cs) {
        hd = gs0[0];
        this.base = base;
        this.gs = IntList.tail(IntList.app(gs0, gs)); // prepends the goals of clause with head hs
        this.ttop = ttop;
        this.k = k;
        this.cs = cs;
    }
    /**
     * creates a specialized spine returning an answer (with no goals left to solve)
     */
    Spine(int hd, int ttop) {
        this.hd = hd;
        base = 0;
        gs = IntList.empty;
        this.ttop = ttop;

        k = -1;
        cs = null;
    }
}
