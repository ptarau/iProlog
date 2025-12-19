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
            int l;
            if (top < 0) l = 4;
            else l = top + 1;
#ifdef RAW_CELL_HEAP
            realloc_(l << 1);
#else
            vector<cell> newstack = vector<cell>(l << 1);
            copy(stack.begin(), stack.end(), newstack.begin());
            stack = newstack;
#endif
        }

        vector<cell> CellStack::toArray() {
            vector<cell> array = vector<cell>(size());
            if (size() > 0) {
#ifdef RAW_CELL_HEAP
                    // memcpy if it ever matters
                    for (int i = 0; i < size(); ++i)
                        array[i] = stack[i];
#else
                    copy(stack.begin(), stack.end(), array.begin());
#endif
            }
            return array;
        }
} // end namespace

