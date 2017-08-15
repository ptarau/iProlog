
package iProlog;
class IntList {

  private final int head;
  private final IntList tail;

  private IntList(final int head) {
    this.head = head;
    tail = null;
  }

  private IntList(final int X, final IntList Xs) {
    head = X;
    tail = Xs;
  }

  static final boolean isEmpty(final IntList Xs) {
    return null == Xs;
  }

  static final int head(final IntList Xs) {
    return Xs.head;
  }

  static final IntList empty = null;

  static final IntList tail(final IntList Xs) {
    return Xs.tail;
  }

  static final IntList cons(final int X, final IntList Xs) {
    return new IntList(X, Xs);
  }

  static final IntList app(final int[] xs, final IntList Ys) {
    IntList Zs = Ys;
    for (int i = xs.length - 1; i >= 0; i--) {
      Zs = cons(xs[i], Zs);
    }
    return Zs;
  }

  static final IntStack toInts(IntList Xs) {
    final IntStack is = new IntStack();
    while (!isEmpty(Xs)) {
      is.push(head(Xs));
      Xs = tail(Xs);
    }
    return is;
  }

  static final int len(final IntList Xs) {
    return toInts(Xs).size();
  }

  @Override
  public String toString() {
    return toInts(this).toString();
  }
}
