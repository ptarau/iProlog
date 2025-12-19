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
#include "Object.h"

namespace iProlog {

using namespace std;

class Engine {

public:

    static Engine* ep;  // inited in unfold, so that Engine::checkit can see it.
                        // (Can't init this in Engine ctor)

    Spine* query = nullptr;
    int base;

    vector<Clause> clauses;     // "Trimmed-down clauses ready to be quickly relocated
                                //  to the heap" [Engine.java]
                                // [Not clear what "trimmed-down" means.]
    Spine::Unifiables clause_list; // if no indexing, contains [0..clauses.length-1]

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
           sym_tab&         sym_0,
           index *Ip        )
		                    : heap(heap_0), clauses(clauses_0), symTab(sym_0), Ip(Ip) {

	    if (clauses.size() == 0) {
		    throw logic_error(cstr("clauses: none"));
	    }

	    trail.clear();

	    query = init(); /* initial spine built from query from which execution starts */

        base = heap_size();          // should be just after any code on heap
	};

    virtual ~Engine();

// should try heap-as-class (maybe subclassed from CellStack)
// to see whether there's a performance penalty

    static inline cell cell_at(CellStack& h, int i) { return h.get(i);              }
           inline cell cell_at(int i) const         { return heap.get(i);           }

           inline void set_cell(int i, cell v)      { heap.set(i,v);                }

    static inline cell getRef(CellStack& h, cell x) { return cell_at(h, x.arg());   }
           inline cell getRef(cell x)  const        { return cell_at(x.arg());      }

           inline void   setRef(cell w, cell r)     { set_cell(w.arg(), r);         }
           inline void   setUnbound(cell href)      { setRef(href, href); }

protected:
    int unify_stack_depth = 0;
    inline void unify_stack_clear() { unify_stack_depth = 0; };  // "set up unification stack" [Engine.java]
    inline void unify_stack_push(cell head) { ++unify_stack_depth;  heap.push(head); }
    inline cell unify_stack_pop() { --unify_stack_depth;  return heap.pop(); }
    inline bool unify_stack_isEmpty() { return unify_stack_depth == 0; }

    vector<Spine*> spines;

    void makeHeap(int size = MIN_HEAP_SIZE) {
        heap.resize(size);
        clear();
    }
    
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

public:
    inline bool isVarLoc_(cell x) const {
        cell r = getRef(x);
        return x.as_int() == r.as_int();   // if rel addressing, check if var and arg is zero
    }

    static inline bool isVarLoc_(CellStack& h, cell x) {
        cell r = getRef(h,x);
        return x.as_int() == r.as_int();   // if rel addressing, check if var and arg is zero
    }

    inline cell deref(cell x) const {
        while (x.is_var()) {
            cell r = getRef(x);
            if (isVarLoc_(x))
                break;
            x = r;
        }
        return x;
    }

    static inline cell deref(CellStack &h, cell x) {
        while (x.is_var()) {
            cell r = getRef(h,x);
            if (isVarLoc_(h,x))
                break;
            x = r;
        }
        return x;
    }

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

    vector<ClauseIndex> toNums(vector<Clause> clauses);

    Clause getQuery();
    Spine* init();
    Spine* answer(int trail_top);
    void popSpine();

    Spine* yield();
    Object ask();
    Object exportTerm(cell c) const;

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
