package org.iprolog;

class IntList {

  private final int head;
  private final IntList tail;

  // Singleton list
  private IntList(final int head) {
    this.head = head;
    tail = null;
  }

  // Cons
  private IntList(final int X, final IntList Xs) {
    head = X;
    tail = Xs;
  }

  static boolean isEmpty(final IntList Xs) {
    return null == Xs;
  }

  static int head(final IntList Xs) {
    assert Xs != null;
    return Xs.head;
  }

  static final IntList empty = null;

  static IntList tail(final IntList Xs) {
    return Xs.tail;
  }

  static IntList cons(final int X, final IntList Xs) {
    return new IntList(X, Xs);
  }

  // append Intlist Ys to Intlist made from int array xs, return result
  static IntList concat(final int[] xs, final IntList Ys) {
    IntList Zs = Ys;
    for (int i = xs.length - 1; i >= 0; i--) {
      Zs = cons(xs[i], Zs);
    }
    return Zs;
  }

  // push Zs Intlist onto new stack, return stack (tos = last)
  static IntStack toInts(IntList Xs) {
    final IntStack is = new IntStack();
    while (!isEmpty(Xs)) {
      is.push(head(Xs));
      Xs = tail(Xs);
    }
    return is;
  }

  // IntList len (note O(n) running time)
  static int len(final IntList Xs) {
    return toInts(Xs).size();
  }

  @Override
  public String toString() {
    return toInts(this).toString();
  }
}
