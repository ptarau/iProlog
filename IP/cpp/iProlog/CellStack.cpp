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
         * "dynamic array operation: doubles when full"
         */
        void CellStack::expand() {
            // size_t l = stack.size();
            size_t l;
            if (top < 0) l = 4;
            else l = top + 1;
#ifdef RAW
            realloc_(l << 1);
#else
            vector<cell> newstack = vector<cell>(l << 1);
            copy(stack.begin(), stack.end(), newstack.begin());
            stack = newstack;
#endif
        }

        /**
        * "dynamic array operation: shrinks to 1/2 if more than than 3/4 empty"
        * Might be good to inline the first comparison, leave the rest in .cpp
        */
        void CellStack::shrink() {
            size_t l = size();
            if (l <= MINSIZE || (top << 2) >= l)
                return;
            l = 1 + (top << 1); // "still means shrink to at 1/2 or less of the heap"
            if (top < MINSIZE) {
                l = MINSIZE;
            }
#ifdef RAW
            realloc_(l);
#else
            vector<cell> newstack = vector<cell>(l);
            copy(stack.begin(), stack.end(), newstack.begin());
            stack = newstack;
#endif
        }

        vector<cell> CellStack::toArray() {
            vector<cell> array = vector<cell>(size());
            if (size() > 0)
#ifdef RAW
                // memcpy if it ever matters
                for (int i = 0; i < size(); ++i)
                    array[i] = stack[i];
#else
                copy(stack.begin(), stack.end(), array.begin());
#endif
            return array;
        }

} // end namespace

