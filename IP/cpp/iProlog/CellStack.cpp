#include "defs.h"
#include "cell.h"
#include "CellStack.h"
/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

namespace iProlog {

    using namespace std;

        /**
         * dynamic array operation: doubles when full
         */
        void CellStack::expand() {
            // size_t l = stack.size();
            size_t l;
            if (top < 0) l = 4;
            else l = top + 1;
            vector<cell> newstack = vector<cell>(l << 1);

            copy(stack.begin(), stack.end(), newstack.begin());
            stack = newstack;
        }

        /**
        * dynamic array operation: shrinks to 1/2 if more than than 3/4 empty
        */
        void CellStack::shrink() {
      // not clear how to solve error message from "stack.size()"
            size_t l = stack.size();
            if (l <= MINSIZE || top << 2 >= l)
                return;
            l = 1 + (top << 1); // still means shrink to at 1/2 or less of the heap
            if (top < MINSIZE) {
                l = MINSIZE;
            }

            vector<cell> newstack = vector<cell>(l);
            // System.arraycopy(stack, 0, newstack, 0, top + 1);
            copy(stack.begin(), stack.end(), newstack.begin());
            stack = newstack;

        }

        vector<cell> CellStack::toArray() {
            vector<cell> array = vector<cell>(size());
            if (size() > 0) {
                copy(stack.begin(), stack.end(), array.begin());
                // System.arraycopy(stack, 0, array, 0, size());
            }
            return array;
        }
#if 0
        void Stack<class T>::reverse() {
            size_t l = size();
            size_t h = l >> 1;
            // Prolog.dump("l="+l);
            for (size_t i = 0; i < h; i++) {
                T temp;
                temp = stack[i];
                stack[i] = stack[l - i - 1];
                stack[l - i - 1] = temp;
            }
        }

        // from IntStack.java
        @Override
            public String toString() {
            return Arrays.toString(toArray());
        }
#endif

} // end namespace

