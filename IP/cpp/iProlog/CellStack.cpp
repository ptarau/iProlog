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
            if (l <= MINSIZE || (top << 2) >= l)
                return;
            l = 1 + (top << 1); // still means shrink to at 1/2 or less of the heap
            if (top < MINSIZE) {
                l = MINSIZE;
            }

            vector<cell> newstack = vector<cell>(l);
            copy(stack.begin(), stack.end(), newstack.begin());
            stack = newstack;
        }

        vector<cell> CellStack::toArray() {
            vector<cell> array = vector<cell>(size());
            if (size() > 0)
                copy(stack.begin(), stack.end(), array.begin());
            return array;
        }

} // end namespace

