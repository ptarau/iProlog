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
        void shrink();
    public:

#ifdef RAW
        cell* stack;
#else
        vector<cell> stack;
#endif
        int cap; // actually unused if RAW not defined

        void expand();
        const int MINSIZE = 1 << 10; // power of 2
        const int SIZE = 16; // power of 2

    public:
        inline CellStack()  {
            int size = MINSIZE;
            cap = size;
#ifdef RAW
            stack = (cell*)std::malloc(sizeof(cell) * size);
            if (stack == nullptr) abort();
#else
            stack.resize(cap);
#endif
            top = -1;
        }

        inline CellStack(int size) {
            if (size == 0) size = MINSIZE;
#ifdef RAW
	    stack = (cell*)std::malloc(sizeof(cell) * size);
	    if (stack == nullptr) abort();
	    cap = size;
	    top = -1;
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
	    int limit = cap;
#else
	    int limit = (int) stack.capacity();
#endif
            if (++top >= limit) {
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
	    return *this;
        }

        inline int size() {
            return top + 1;
        }

        inline size_t capacity() {
#ifdef RAW
		return cap;
#else
		return stack.size();
#endif
        }

        inline void realloc_(int l) {
#ifdef RAW
            cell* tcp = (cell*)std::realloc((void*)stack, l*sizeof(cell));
            if (tcp == nullptr) abort();
            cap = l;
            stack = tcp;
#endif
        }

        inline void resize(int l) {
#ifdef RAW
		realloc_(l);
#else
		stack.resize(l);
#endif
        }

        vector<cell> toArray();

        inline cell *data() {
#ifdef RAW
	    return stack;
#else
	    return stack.data();
#endif
        }

// maybe redefine the corresponding memb fns in Engine.h to use these?
// check for performance penalty
    static inline cell   cell_at(CellStack &h, int i)
        { return h.get(i);              			}
    static inline void   set_cell(CellStack &h, int i, cell v)
	{ h.set(i,v);                   			}
    static inline cell   getRef(CellStack &h, cell x)
        { return cell_at(h, cell::detag(x));  			}
    static inline void   setRef(CellStack &h, cell w, cell r)
        { set_cell(h, cell::detag(w), r);     			}

    static inline cell deref(CellStack &h, cell x) {
        while (cell::isVAR(x)) {
            cell r = getRef(h,x);
            if (cell::isVarLoc(r,x))
                break;
            x = r;
        }
        return x;
    }

        static inline cell cell2index(CellStack &heap, cell c) {
            cell x = cell::tag(cell::V_,0);
            int t = cell::tagOf(c);
            switch (t) {
                case cell::R_:
                    x = getRef(heap, c);
                    break;
                case cell::C_:
                case cell::N_:
                    x = c;
                    break;
            }
            return x;
        }

        static inline void ensureSize(CellStack &heap, int more) {
	    if (more < 0) abort();
            // assert(more > 0);
            if (size_t(1 + heap.getTop() + more) >= heap.capacity()) {
                heap.expand();
            }
        }

	/**
	 * "Pushes slice[from,to] at given base onto the heap."
	 * b has cell structure, i.e, index, shifted left 3 bits, with tag V_
	 */
	static inline void pushCells(CellStack &heap, cell b, int from, int upto, int base) {

	    int count = upto - from;
	    ensureSize(heap, count);

	    if (is_raw) {
		cell* srcp = heap.data() + base + from;
		cell* dstp = (cell*)(heap.data() + heap.getTop()) + 1;
		heap.setTop(heap.getTop() + count);
		cell::cp_cells(b,srcp,dstp,count);
	    }
	    else
		for (int i = from; i < upto; i++) {
		    heap.push(cell::relocate(b, heap.get(base + i)));;
	    }
	}

	/**
	 * "Pushes slice[from,to] at given base onto the heap."
	 *  TODO: Identical to pushToTopOfHeap()?
	 * 
	 */
	static inline void pushCells(CellStack &heap, cell b, int from, int to, vector<cell> cells) {
	    int count = to - from;
	    ensureSize(heap, count);

	    if (is_raw) {
		cell* heap_dst = (cell*)(heap.data() + heap.getTop()) + 1;
		heap.setTop(heap.getTop() + count);
		cell::cp_cells(b,cells.data(),heap_dst,count);
	    }
	    else
		for (int i = from; i < to; i++)
		    heap.push(cell::relocate(b, cells[i]));
	}
    };
} // end namespace
