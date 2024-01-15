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
#include "index.h"

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

    CellStack heap;
#if 0
    vector<IMap> imaps;
    vector<IntMap> var_maps;
#else
    index *Ip;
#endif

/**
 * "Builds a new engine from a natural-language-style assembler.nl file"
 *  -- for standalone engine, file reading, parsing & code gen is
 *     done in main.cpp for now
 */
    Engine(CellStack &heap_0,
           vector<Clause> &clauses_0,
	   unordered_map<string, Integer*> &syms_0,
	   vector<string> &slist_0)
		: heap(heap_0), clauses(clauses_0), syms(syms_0), slist(slist_0) {

	    if (clauses.size() == 0) {
		throw logic_error(cstr("clauses: none"));
	    }

	    CellList::init();
	    trail.clear();
	    clause_list = toNums(clauses); // initially an array  [0..clauses.length-1]
	    query = init(); /* initial spine built from query from which execution starts */
	    size_t base = heap_size();          // should be just after any code on heap
#if 0
	    var_maps = vcreate(MAXIND);  // vector of IntMaps
	    imaps = index(clauses,var_maps);
#endif
	    Ip = new index(heap, clauses);
	};

    virtual ~Engine();

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

    void makeHeap(int size = MINSIZE) {
        heap.resize(size);
        clear();
    }

    void pushCells(CellStack &h, cell b, int from, int upto, int base);
    void pushCells(CellStack &h, cell b, int from, int upto, vector<cell> cells);

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
#if 0
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
#endif
public:
    vector<Clause> dload(cstr s);

    CellStack trail;
    string showCell(cell w);
#if 0
    static vector<IntMap> vcreate(size_t l);
#endif

protected:
    void ppc(const Clause&);
    // void ppGoals(IntList *bs);
    void ppSpines() {}

    bool unify(int base);
    bool unify_args(int w1, int w2);

    cell pushHeadtoHeap(cell b, const Clause& C);

    bool hasClauses(Spine* S);

#if 0
    void makeIndexArgs(Spine* G, cell goal);

    inline cell cell2index(cell c) {
	    return CellStack::cell2index(heap, c);
    }
#endif

    Spine* unfold(Spine *G);

    vector<size_t> toNums(vector<Clause> clauses);

    Clause getQuery();
    Spine* init();
    Spine* answer(int trail_top);
    void popSpine();

    Spine* yield();
    cell ask();
#if 0
    void put(vector<IMap> &imaps,
	     vector<IntMap> &var_maps,
	     t_index_vector& keys,
	     int val);

    vector<IMap> index(vector<Clause> &clauses, vector<IntMap> &var_maps);
#endif

    void linker(unordered_map<string,vector<int>> refs,
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
#if 0
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
#endif
}; // end Engine

} // end namespace
