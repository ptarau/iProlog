#pragma once
/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include <iostream>   // until I can get rid of cout

#include <unordered_map>
#include <cassert>
#include "defs.h"
#include "cell.h"
#include "spine.h"
#include "CellStack.h"
#include "index.h"
#include "clause.h"
#include "sym_tab.h"

namespace iProlog {

using namespace std;

class Engine {

public:

    Spine* query = nullptr;
    int base;

    vector<Clause> clauses;     // "Trimmed-down clauses ready to be quickly relocated
                                //  to the heap" [Engine.java]
                                // [Not clear what "trimmed-down" means.]
    vector<int> clause_list; // if no indexing, contains [0..clauses.length-1]

    // "Symbol table - made of map (syms) + reverse map from
    //  ints to syms (slist)" [Engine.java]
    sym_tab symTab;

    /** Runtime areas: **/
    // 
    //  heap - "contains code for 'and' clauses and their
    //  copies created during execution" [Engine.java]

    CellStack heap;

/**
 * "Builds a new engine from a natural-language-style assembler.nl file"
 *  -- for standalone engine, file reading, parsing & code gen is
 *     done in main.cpp for now
 */
    Engine(CellStack&       heap_0,
           vector<Clause>&  clauses_0,
           sym_tab&         sym_0)
		                    : heap(heap_0), clauses(clauses_0), symTab(sym_0) {

	    if (clauses.size() == 0) {
		    throw logic_error(cstr("clauses: none"));
	    }

	    CellList::init();
	    trail.clear();
	    clause_list = toNums(clauses); // initially an array  [0..clauses.length-1]
	    query = init(); /* initial spine built from query from which execution starts */
	    base = heap_size();          // should be just after any code on heap
	};

    virtual ~Engine();

protected:

// should try heap-as-class (maybe subclassed from CellStack)
// to see whether there's a performance penalty

    inline cell   cell_at(int i) const      { return heap.get(i);       }
    inline void   set_cell(int i, cell v)   { heap.set(i,v);            }
    inline cell   getRef(cell x)  const     { return cell_at(x.arg());  }
    inline void   setRef(cell w, cell r)    { set_cell(w.arg(), r);     }

    CellStack unify_stack;

    vector<Spine*> spines;

    void makeHeap(int size = MIN_HEAP_SIZE) {
        heap.resize(size);
        clear();
    }

    vector<cell> pushBody(cell b, cell head, const Clause& C);
    
    void clear();

    inline int heap_size() {
        return heap.getTop() + 1;
    }

    inline void ensureSize(CellStack &heap, int more) {
	if (more < 0) abort();
        // assert(more > 0);
        if (1 + heap.getTop() + more >= (int) heap.capacity()) {
            heap.expand();
        }
    }

    void unwindTrail(int savedTop);

// maybe redefine Engine.h version to use CellStack version?
    inline cell deref(cell x) const {
        while (x.is_var()) {
            cell r = getRef(x);
            if (CellStack::isVarLoc_(heap,x))
                break;
            x = r;
        }
        return x;
    }
public:
    index *Ip;
    CellStack trail;

    string showCell(cell w) const;

protected:
    void ppSpines() {}

    bool unify(int base);
    bool unify_args(int w1, int w2);

    cell pushHeadtoHeap(cell b, const Clause& C);

    bool hasClauses(const Spine* S) const;

    Spine* unfold(Spine *G);

    vector<int> toNums(vector<Clause> clauses);

    Clause getQuery();
    Spine* init();
    Spine* answer(int trail_top);
    void popSpine();

    Spine* yield();
    cell ask();

    void ppc(const Clause&);

    inline string showCS(string name, CellStack cs) {
        string s = name + ":";
        for (int i = 0; i < cs.size(); ++i) {
            s += " ";
            s += showCell(cs.get(i));
        }

        return s;
    }
}; // end Engine

} // end namespace
