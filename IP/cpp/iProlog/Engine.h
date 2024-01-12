#pragma once
/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <cassert>
#include "defs.h"
#include "cell.h"
#include "Object.h"
#include "toks.h"
#include "spine.h"
#include "Integer.h"
#include "CellStack.h"
#include "IntMap.h"
#include "IMap.h"
#include "clause.h"

namespace iProlog {

using namespace std;

class Engine {

public:

    Spine* query = nullptr;

    vector<Clause> clauses;     // "Trimmed-down clauses ready to be quickly relocated
                                //  to the heap" [Engine.java]
                                // [Not clear what "trimmed-down" means.]
    vector<size_t> clause_list; // if no indexing, contains [0..clauses.length-1]

    // "Symbol table - made of map (syms) + reverse map from
    //  ints to syms (slist)" [Engine.java]
    unordered_map<string, Integer*> syms;
    vector<string> slist;

    /** Runtime areas: **/
    // 
    //  heap - "contains code for 'and' clauses and their
    //  copies created during execution" [Engine.java]

    // vector<cell> heap_;
    CellStack heap;

    vector<IMap> imaps;
    vector<IntMap> var_maps;

    Engine(string asm_nl_source);
    virtual ~Engine();

    int n_matches;

protected:

    Integer *addSym(string sym);
    string getSym(int w);

// should try heap-as-class (maybe subclassed from CellStack)
// to see whether there's a performance penalty

    inline cell   cell_at(int i)            { return heap.get(i);              }

    inline void   set_cell(int i, cell v)   { heap.set(i,v);                   }

    inline cell   getRef(cell x)            { return cell_at(cell::detag(x));  }

    inline void   setRef(cell w, cell r)    { set_cell(cell::detag(w), r);     }

    CellStack unify_stack;

    vector<Spine*> spines;

    static cstr heapCell(int w);

    static inline bool hasGoals(const Spine* S) { return S->goals != nullptr; }

    void makeHeap(int size = MINSIZE) {
        heap.resize(size);
        clear();
    }

#if 1
    void pushCells(CellStack &h, cell b, int from, int upto, int base);
    void pushCells(CellStack &h, cell b, int from, int upto, vector<cell> cells);
#endif
    vector<cell> pushBody(cell b, cell head, Clause& C);
    
    void clear();

    inline int heap_size() {
        return heap.getTop() + 1;
    }
#if 1
    inline void ensureSize(CellStack &heap, int more) {
	if (more < 0) abort();
        // assert(more > 0);
        if (size_t(1 + heap.getTop() + more) >= heap.capacity()) {
            heap.expand();
        }
    }
#endif
    static vector<int>&
        put_ref(string arg,
                unordered_map<string, vector<int>>& refs,
                int clause_pos);

    cell encode(int t, cstr s);

    void unwindTrail(int savedTop);

// maybe redefine Engine.h version to use CellStack version?
    inline cell deref(cell x) {
        while (cell::isVAR(x)) {
            cell r = getRef(x);
            if (cell::isVarLoc(r,x))
                break;
            x = r;
        }
        return x;
    }

    /**
     * raw display of a term - to be overridden
     */
    virtual string showTerm(cell x) {
      return showTerm(exportTerm(x));
    }

    /**
     * raw display of an externalized term
     */
    virtual string showTerm(Object O) {
      return O.toString();
    }

    Object exportTerm(cell x);
public:
    vector<Clause> dload(cstr s);

    CellStack trail;
    string showCell(cell w);
    static vector<IntMap> vcreate(size_t l);
protected:
    string showCells2(int base, int len);
    string showCells1(vector<cell> cs);

    void ppc(const Clause&);
    // void ppGoals(IntList *bs);
    void ppSpines() {}

    bool unify(int base);
    bool unify_args(int w1, int w2);

    cell pushHeadtoHeap(cell b, const Clause& C);

    bool hasClauses(Spine* S);

    void makeIndexArgs(Spine* G, cell goal);
    t_index_vector getIndexables(cell ref);
    // cell cell2index(cell c);
#if 0
inline cell cell2index(CellStack h, cell c) {
    cell x = 0;
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
#endif

inline cell cell2index(cell c) {
	return CellStack::cell2index(heap, c);
}

    Spine* unfold(Spine *G);

    vector<size_t> toNums(vector<Clause> clauses);

    Clause getQuery();
    Spine* init();
    Spine* answer(int trail_top);
    void popSpine();

    Spine* yield_();
    Object ask();

    void put(t_index_vector& keys, int val);
    vector<IMap> index(vector<Clause> clauses);

    void linker(unordered_map<string, vector<int>> refs,
        vector<cell>& cells,
        vector<cell>& goals,
        vector<Clause>& compiled_clauses);

    Clause putClause(vector<cell> cells, vector<cell> &hgs, int neck);


    inline string showCS(string name, CellStack cs) {
        string s = name + ":";
        for (int i = 0; i < cs.size(); ++i) {
            s += " ";
            s += showCell(cs.get(i));
        }

        return s;
    }

    inline bool possible_match(const t_index_vector& an_index_vector, Clause &C) {
        for (size_t i = 0; i < MAXIND; i++) {
            int x = an_index_vector[i];
            int y = C.index_vector[i];
            if (0 == x || 0 == y)
                continue;
            if (x != y)
                return false;
        }
        ++n_matches;
        return true;
    }

}; // end Engine

} // end namespace
