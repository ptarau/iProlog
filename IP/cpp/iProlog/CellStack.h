#pragma once
/*
* iProlog / C++  [derived from Java version]
* License: Apache 2.0
* Copyright(c) 2017 Paul Tarau
*/

#include "defs.h"
#include <assert.h>
#include "cell.h"

namespace iProlog {

    using namespace std;

    class CellStack {
    protected:
        int top = -1;;
        void shrink();
    public:
        vector<cell> stack;
        void expand();
        const int MINSIZE = 1 << 15; // power of 2

        const int SIZE = 16; // power of 2

    public:
        inline CellStack() : CellStack(SIZE) { }

        inline CellStack(int size) {
            stack = vector<cell>(size);
            clear();
        }

        inline int getTop() {
            return top;
        }

        inline int setTop(int t) {
            return top = t;
        }

        inline void clear() {
            top = -1;
        }

        inline bool isEmpty() {
            return top < 0;
        }

        /**
         * Pushes an element - top is incremented first then the
         * element is assigned. This means top points to the last assigned
         * element.
         */
        inline void push(cell i) {
            // IO.dump("push:"+i);
            if (++top >= stack.size()) {
                expand();
            }
            stack[top] = i;
        }

        inline cell pop() {
            cell r = stack[top--];
            shrink();
            return r;
        }

        inline cell get(int i) const {
            return stack[i];
        }

        inline void set(int i, cell val) {
            stack[i] = val;
        }

        inline CellStack &operator=(CellStack c) {
            for (int i = 0; i < c.size(); ++i)
                stack[i] = c.stack[i];
            top = c.top;
        }

        inline int size() {
            return top + 1;
        }

        inline size_t capacity() {
            return stack.capacity();
        }

        inline void resize(size_t l) {
            stack.resize(l);
        }
        vector<cell> toArray();
        // void reverse();

        cell *data() { return stack.data();  }
    };
} // end namespace
