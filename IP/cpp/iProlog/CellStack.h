#pragma once
/*
* iProlog / C++  [derived from Java version]
* License: Apache 2.0
* Copyright(c) 2017 Paul Tarau
*/

#include "defs.h"
#include <assert.h>
#include "cell.h"

/* RAW, when  defined, says to go with a less-safe, faster implementation
 *    than STL vectors, with no bounds check, and less header info to
 *    save a little space. The fast-copy cell heap-to-heap relocation may
 *    end up in this class eventually.
 */
#define RAW

namespace iProlog {

    using namespace std;

    class CellStack {
    protected:
        int top = -1;;
        void shrink();
    public:
#ifdef RAW
        cell* stack;
        size_t cap;
#else
        vector<cell> stack;
#endif
        void expand();
        const int MINSIZE = 1 << 15; // power of 2

        const int SIZE = 16; // power of 2

    public:
        inline CellStack() : CellStack(SIZE) {
            cout << "Got to CellStack()" << endl;
        }

        inline CellStack(int size) {

#ifdef RAW
            stack = (cell*)std::malloc(sizeof(cell) * size);
            cap = size;
            cout << "Got to CellStack(" << size << ")" << endl;
#else
            stack = vector<cell>(size);
#endif
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
         * "Pushes an element - top is incremented first then the
         * element is assigned. This means top points to the last assigned
         * element." [Java code]
         */
        inline void push(cell i) {
#ifdef RAW
            if (++top >= cap) {
#else
            if (++top >= stack.size()) {
#endif
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
            if (c.size() > capacity()) abort();
            // memcpy if it ever matters
            for (int i = 0; i < c.size(); ++i)
                stack[i] = c.stack[i];
            top = c.top;
        }

        inline int size() {
            return top + 1;
        }

        inline size_t capacity() {
#ifdef RAW
            return cap;
#else
            return stack.capacity();
#endif
        }
#ifdef RAW
        inline void realloc_(size_t l) {
            cell* tcp = (cell*)std::realloc((void*)stack, l*sizeof(cell));
            if (tcp == nullptr) abort();
            stack = tcp;
            cap = l;
        }
#endif

        inline void resize(size_t l) {
#ifdef RAW
            realloc_(l);
#else
            stack.resize(l);
#endif
        }
        vector<cell> toArray();
        // void reverse();

        cell *data() {
#ifdef RAW
            return stack;
#else
            return stack.data();
#endif
        }
    };
} // end namespace
