package iProlog;

import java.util.ArrayList;

class ObStack<T> extends ArrayList<T> {

    private static final long serialVersionUID = 1L;

    final T pop() {
        int last = size() - 1;
        return remove(last);
    }

    final void push(T O) {
        add(O);
    }

    final T peek() {
        return get(size() - 1);
    }
}
