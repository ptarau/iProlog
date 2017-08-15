
/**
Dynamic Stack for int data.
 */
package iProlog;
import java.util.Arrays;

class IntStack {

  private int stack[];

  private int top;

  static int SIZE = 16; // power of 2

  static int MINSIZE = 1 << 15; // power of 2

  IntStack() {
    this(SIZE);
  }

  IntStack(final int size) {
    stack = new int[size];
    clear();
  }

  final int getTop() {
    return top;
  }

  final int setTop(final int top) {
    return this.top = top;
  }

  final void clear() {
    //for (int i = 0; i <= top; i++)
    //stack[i] = 0;
    top = -1;
  }

  final boolean isEmpty() {
    return top < 0;
  }

  /**
   * Pushes an element - top is incremented first than the
   * element is assigned. This means top point to the last assigned
   * element - which can be returned with peek().
   */
  final void push(final int i) {
    // IO.dump("push:"+i);
    if (++top >= stack.length) {
      expand();
    }
    stack[top] = i;
  }

  final int pop() {
    final int r = stack[top--];
    shrink();
    return r;
  }

  final int get(final int i) {
    return stack[i];
  }

  final void set(final int i, final int val) {
    stack[i] = val;
  }

  final int size() {
    return top + 1;
  }

  /**
   * dynamic array operation: doubles when full
   */
  private final void expand() {
    final int l = stack.length;
    final int[] newstack = new int[l << 1];

    System.arraycopy(stack, 0, newstack, 0, l);
    stack = newstack;
  }

  /**
  * dynamic array operation: shrinks to 1/2 if more than than 3/4 empty
  */
  private final void shrink() {
    int l = stack.length;
    if (l <= MINSIZE || top << 2 >= l)
      return;
    l = 1 + (top << 1); // still means shrink to at 1/2 or less of the heap
    if (top < MINSIZE) {
      l = MINSIZE;
    }

    final int[] newstack = new int[l];
    System.arraycopy(stack, 0, newstack, 0, top + 1);
    stack = newstack;
  }

  int[] toArray() {
    final int[] array = new int[size()];
    if (size() > 0) {
      System.arraycopy(stack, 0, array, 0, size());
    }
    return array;
  }

  public final void reverse() {
    int l = size();
    int h = l >> 1;
    // Prolog.dump("l="+l);
    for (int i = 0; i < h; i++) {
      int temp = stack[i];
      stack[i] = stack[l - i - 1];
      stack[l - i - 1] = temp;
    }
  }

  @Override
  public String toString() {
    return Arrays.toString(toArray());
  }

}
