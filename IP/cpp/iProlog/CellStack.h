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
        int top = -1;
        // void shrink();
    public:

#ifdef RAW_CELL_HEAP
        cell* stack;
#else
        vector<cell> stack;
#endif
        int cap; // actually unused if RAW not defined

        void expand();
        const int MINSIZE = 1 << 10; // power of 2
        const int SIZE = 16;         // power of 2

    public:
        inline CellStack()  {
            int size = MINSIZE;
            cap = size;
#ifdef RAW_CELL_HEAP
            stack = (cell*)std::malloc(sizeof(cell) * size);
            if (stack == nullptr) abort();
#else
            stack.resize(cap);
#endif
            top = -1;
        }

        inline CellStack(int size) {
            if (size == 0) size = MINSIZE;
#ifdef RAW_CELL_HEAP
	        stack = (cell*)std::malloc(sizeof(cell) * size);
	        if (stack == nullptr) abort();
	        cap = size;
	        top = -1;
#else
	        stack = vector<cell>(size);
#endif
            clear();
        }

        inline int getTop() const {
            return top;
        }

        inline int setTop(int t) {
            return top = t;
        }

        inline void clear() {
            top = -1;
        }

        inline bool isEmpty() const {
            return top < 0;
        }

        /**
         * "Pushes an element - top is incremented first then the
         * element is assigned. This means top points to the last assigned
         * element." [Java code]
         */
        inline void push(cell i) {
            if (++top >= capacity())
                expand();
                stack[top] = i;
        }

#if 1
        /**
        * "dynamic array operation: shrinks to 1/2 if more than than 3/4 empty"
        * Might be good to inline the first comparison, leave the rest in .cpp
        */
        inline void shrink() {
            int l = size();
            if (l <= MINSIZE || (top << 2) >= l)
                return;
            l = 1 + (top << 1); // "still means shrink to at 1/2 or less of the heap"
            if (top < MINSIZE) {
                l = MINSIZE;
            }
#ifdef RAW_CELL_HEAP
            cout << "realloc called" << endl;
            realloc_(l);
#else
            vector<cell> newstack = vector<cell>(l);
            copy(stack.begin(), stack.end(), newstack.begin());
            stack = newstack;
#endif
        }
#endif

        inline cell pop() {
            cell r;
            r = stack[top--];
            shrink();
            return r;
        }

        inline cell get(int i) const {
#ifdef RAW_CELL_HEAP
                return stack[i];
#else
                return stack.at(i); // bounds-checked
#endif
        }

        inline void set(int i, cell val) {
                stack[i] = val;
        }

        inline CellStack &operator=(CellStack c) {
            if (c.size() > capacity()) abort();
            // memcpy if it ever matters
            for (int i = 0; i < c.size(); ++i)
                    stack[i] = c.get(i);
            top = c.top;
	        return *this;
        }

        inline int size() const {
            return top + 1;
        }

        inline size_t capacity() const {
#ifdef RAW_CELL_HEAP
		    return cap;
#else
		    return stack.size();
#endif
        }

        inline void realloc_(int l) {
#ifdef RAW_CELL_HEAP
            cell* tcp = (cell*)std::realloc((void*)stack, l * sizeof(cell));
            if (tcp == nullptr) abort();
            cap = l;
            stack = tcp;
#endif
        }

        inline void resize(int l) {
#ifdef RAW_CELL_HEAP
            realloc_(l);
#else
            stack.resize(l);
#endif
        }

        vector<cell> toArray();

        inline cell *data() const {
#ifdef RAW_CELL_HEAP
	        return stack;
#else
	        return (cell *) stack.data();
#endif
        }

        static inline void ensureSize(CellStack &heap, int more) {
            if (more == 0) return;
            assert(more > 0);
            if (size_t(1 + heap.getTop() + more) >= heap.capacity())
                heap.expand();
        }

	/**
	 * "Pushes slice[from,to] at given base onto the heap."
	 * b has cell structure, i.e, index, shifted, with tag V_
	 */
	static inline void pushCells(CellStack &heap, cell b, int from, int upto, int base) {
#define TR if(0)
        assert(upto >= from);
	    int count = upto - from;
        if (count == 0)
            return;
	    ensureSize(heap, count);

	    if (has_raw_cell_heap) {
		    cell* srcp = heap.data() + base + from;
		    cell* dstp = (cell*)(heap.data() + heap.getTop()) + 1;
		    heap.setTop(heap.getTop() + count);
		    cell::cp_cells(b,srcp,dstp,count);
	    }
	    else
            for (int i = from; i < upto; i++) {
                TR cout << "pushCells: heap.get(" << base + i << ")."
                    << "relocated_by(" << b.show() << ")="
                    << heap.get(base + i).relocated_by(b).show()
                    << endl;
                heap.push(heap.get(base + i).relocated_by(b));

            }
#undef TR
	}

	/**
	 * "Pushes slice[from,to] at given base onto the heap."
	 *  TODO: Identical to pushToTopOfHeap()?
	 * 
	 */
	static inline void pushCells(CellStack &heap, cell b, int from, int to, const vector<cell> &cells) {
	    int count = to - from;
	    ensureSize(heap, count);

        if (has_raw_cell_heap) {
            cell* heap_dst = (cell*)(heap.data() + heap.getTop()) + 1;
            heap.setTop(heap.getTop() + count);
            cell::cp_cells(b, cells.data(), heap_dst, count);
        }
        else
            for (int i = from; i < to; i++)
                heap.push(cells[i].relocated_by(b));
	}
  };
} // end namespace
