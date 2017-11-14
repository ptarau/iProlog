package iProlog;

class IntList {

    static final IntList empty = null;
    private final int head;
    private final IntList tail;

    private IntList(int head) {
        this.head = head;
        tail = null;
    }

    private IntList(int X, IntList Xs) {
        head = X;
        tail = Xs;
    }

    static final boolean isEmpty(IntList Xs) {
        return null == Xs;
    }

    static final int head(IntList Xs) {
        return Xs.head;
    }

    static final IntList tail(IntList Xs) {
        return Xs.tail;
    }

    private static IntList cons(int X, IntList Xs) {
        return new IntList(X, Xs);
    }

    static final IntList app(int[] xs, IntList Ys) {
        IntList Zs = Ys;
        for (int i = xs.length - 1; i >= 0; i--) {
            Zs = cons(xs[i], Zs);
        }
        return Zs;
    }

    private static IntStack toInts(IntList Xs) {
        IntStack is = new IntStack();
        while (!isEmpty(Xs)) {
            is.push(head(Xs));
            Xs = tail(Xs);
        }
        return is;
    }

    static final int len(IntList Xs) {
        return toInts(Xs).size();
    }

    @Override
    public String toString() {
        return toInts(this).toString();
    }
}
