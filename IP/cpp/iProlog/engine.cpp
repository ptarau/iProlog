/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */


#include <map>
#include <sstream>
#include <numeric>
#include <iostream>
#include <algorithm>

#include "Engine.h"


namespace iProlog {

 /**
  * Implements execution mechanism
  *
  * "an array representation
  *  with variables on the left side of our equations
  *  turned into indices pointing to compound terms
  *  at higher addresses in the same array."
  *
  * add _0 Y _1 and _0 holds(=) s X and _1 holds(=) s Z if add X Y Z:
  *
  **[5]  a: 4        -- because |[add,_0,Y,_1]| = 4
  * [6]  c: ->"add"
  * [7]  r: 10       -- link to subterm: _0 holds(=) s X
  * [8]  v: 8        -- Y
  * [9]  r: 13       -- link to next subterm: _1 holds(=) s Z
  *
  *                  -- _0 holds(=) s X:
  **[10] a: 2
  * [11] c: ->"s"
  * [12] v: 12       -- X
  *
  *                  -- _1 holds(=) s Z:
  **[13] a: 2
  * [14] c: ->"s"
  * [15] v: 15       -- Z
  *
  *                  -- add X Y Z:
  **[16] a: 4
  * [17] c: ->"add"
  * [18] u: 12       -- X
  * [19] u: 8        -- Y
  * [20] u: 15       -- Z
  */

Engine::~Engine() { }

/**
 * Builds a new engine from a natural-language-style assembler.nl file
 */
Engine::Engine(string asm_nl_source) {
    n_matches = 0;
    CellList::init();
#if 0
    /// tests
    int offset = 10;
    cell b = cell::tag(cell::V_, offset);
    cout << " b=" << showCell(b) << endl;

    cell v = cell::tag(cell::V_, 0);
    cout << "v=" << showCell(v) << endl;
    cell v1 = cell::relocate(b, v);
    cout << "v1=" << showCell(v1) << endl;

    cell r = cell::tag(cell::R_, 0);
    cout << "r=" << showCell(r) << endl;
    cell r1 = cell::relocate(b, r);
    cout << "r1=" << showCell(r1) << endl;

    cell u = cell::tag(cell::U_, 0);
    cout << "u=" << showCell(u) << endl;
    cell u1 = cell::relocate(b, u);
    cout << "u1=" << showCell(u1) << endl;
#endif

    n_matches = 0;

    // syms -- from default constructor?
    // slist -- from default constructor?

    trail.clear();
    if (trail.getTop() >= 0) abort();
#if 0
    cout << "trail.size()=" << trail.size() << endl;
#endif
    makeHeap();

    clauses = dload(asm_nl_source); // load "natural language" source

    if (clauses.size() == 0) {
        throw logic_error(cstr("clauses: none"));
    }

    clause_list = toNums(clauses); // initially an array  [0..clauses.length-1]
#if 0
    cout << endl; cout << "clause_list:" << endl;
    for (int i = 0; i < clause_list.size(); ++i)
        cout << "clause_list[" << i << "]=" << clause_list[i] << endl;
    cout << endl;
#endif
    query = init(); /* initial spine built from query from which execution starts */

    size_t base = heap_size();          // should be just after any code on heap

    // spines_top = 0;

    var_maps = vcreate(MAXIND);  // vector of IntMaps
    imaps = index(clauses);
}

// Indexing extensions - ony active if START_INDEX clauses or more.

vector<IntMap> Engine::vcreate(size_t l) {
    vector<IntMap> vss = vector<IntMap>(l);
    for (int i = 0; i < l; i++) {
        vss[i] = IntMap();
    }
    return vss;
}

/**
 * unfold - "transforms a spine [G] containing references to choice point and
 * immutable list of goals into a new spine, by reducing the
 * first goal in the list with a clause that successfully
 * unifies with it - in which case places the goals of the
 * clause at the top of the new list of goals, in reverse order"
 */
Spine* Engine::unfold(Spine *G) {
    int saved_trail_top = trail.getTop();
#if 0
    std::cout << "unfold: saved_trail_top=" << saved_trail_top << endl;
    std::cout << "unfold: heap.getTop()=" << heap.getTop() << endl;
#endif
    int saved_heap_top = heap.getTop();
    int base = size_t(saved_heap_top) + 1;

    if (CellList::isEmpty(G->goals))
        return nullptr;

    cell goal = CellList::head(G->goals);
#if 1
    makeIndexArgs(G, goal);
#endif
    size_t last = G->unifiables.size();
#if 0
    cout << "before unfold loop: G->kount=" << G->kount << endl;
    for (int i = 0; i < G->unifiables.size(); ++i)
        cout << "  G->unifiables[" << i << "]=" << G->unifiables[i] << endl;
    for (int k = G->kount; k < last; k++) {
        cout << "  clauses[" << G->unifiables[k] << "].base=" << clauses[G->unifiables[k]].base << endl;
    }
#endif

    for (int k = G->kount; k < last; k++) {
#if 0
        cout << "unfold loop, k = " << k << endl;
#endif
        assert(k == G->unifiables[k]);
        Clause* C0 = &clauses[G->unifiables[k]];
#if 0
        cout << "C0.base=" << C0->base << endl;
        cout << "     " << showCS("heap before pushHead", heap) << endl;
#endif

        if (!possible_match(G->index_vector, *C0))
            continue;
#if 0
        cout << "???????? possible match? ??????" << endl;
#endif
        int base0 = base - C0->base;
        assert(cell::V_ == 0);
        cell b = cell::tag(cell::V_, base0);  // TODO - I really need a "heap index(offset)" type
        cell head = pushHeadtoHeap(b, *C0);

        unify_stack.clear();  // "set up unification stack" [Engine.java]

        unify_stack.push(head);
        unify_stack.push(goal);
#if 0
        cout << "pushed to unify_stack: head=" << showCell(head) << " goal=" << showCell(goal) << endl;
        cout << "                  " << showCS("unify_stack", unify_stack) << endl;
        cout << "                  " << showCS("heap", heap) << endl;
        cout << "                  base=" << base << endl;
#endif
        if (!unify(base)) {
#if 0
            cout << "!!!!!!! unify failed !!!!! saved_trail_top=" << saved_trail_top << " saved_heap_top=" << saved_heap_top << endl;
            cout << "     calling unwindTrail()" << endl;
#endif
            unwindTrail(saved_trail_top);            
            heap.setTop(saved_heap_top);
#if 0
            cout << "!!!!!!! unify continuing with trail_top=" << saved_trail_top << " heap.getTop()=" << heap.getTop() << endl;
#endif
            continue;
        }

        vector<cell> goals = pushBody(b, head, *C0);
        shared_ptr<CellList> tl = CellList::tail(G->goals);
#if 0
        cout << "    *** new_goals size=" << new_goals->size() << endl;
#endif
        G->kount = k + 1;   // "index of the last clause [that]
                            //  the top goal of [this] Spine
                            //  has tried to match so far" [HHG doc]
#if 0
        cout << endl << "     *** spine.base = " << G->base << " UPDATED spine.kount=" << G->kount << endl << endl;
#endif
        if (goals.size() != 0 || tl != nullptr) {
#if 0
            std::cout << endl << "     *** new_goals NOT empty: new Spine with initial kount=0" << endl << endl;
#endif
            Spine* sp = new Spine(goals
                                , base
                                , tl
                                , saved_trail_top
                                , 0
                                , clause_list
                        );
            return sp;
        }
        else {
#if 0
            std::cout << endl << "     *** new_goals NOT empty: new Spine being generated by answer()" << endl << endl;
#endif
            return answer(saved_trail_top);
        }
    }
    return nullptr;
}

bool Engine::unify(int base) {
#if 0
    cout << "  Entering unify(), unify_stack.getTop()=" << unify_stack.getTop() << endl;
#endif
    while (!unify_stack.isEmpty()) {
        cell x1 = deref(unify_stack.pop());
        cell x2 = deref(unify_stack.pop());
#if 0
        cout << "    unify loop: x1=" << showCell(x1) << " x2=" << showCell(x2) << endl;
        cout << "    unify loop: unify_stack.getTop()=" << unify_stack.getTop() << endl;
#endif
        if (x1.as_int() != x2.as_int()) {
            int t1 = cell::tagOf(x1);
            int t2 = cell::tagOf(x2);
            int w1 = cell::detag(x1);
            int w2 = cell::detag(x2);
            if (cell::isVAR(x1)) {                  /* "unb. var. v1" */
                if (cell::isVAR(x2) && w2 > w1) {   /* "unb. var. v2" */
                    set_cell(w2,x1);
                    if (w2 <= base) {
                        trail.push(x2);
#if 0
                        cout<< "x1 & x2 vars, w2>w1, x2 pushed: " << showCell(x2) << endl;
#endif
                    }
                } else {                            // "x2 nonvar or older"
                    set_cell(w1,x2);
                    if (w1 <= base) {
                        trail.push(x1);
#if 0
                        cout<<"x1 var, x2 nonvar or older, x1 pushed: " << showCell(x1) << endl;
#endif
                    }
                }
            } else if (cell::isVAR(x2)) {           /* "x1 is NONVAR" */
                set_cell(w2,x1);
                if (w2 <= base) {
                    trail.push(x2);
#if 0
                    cout << "x1 nonvar, x2 older, x2 pushed: " << showCell(x2) << endl;
#endif
                }
            } else if (cell::R_ == t1 && cell::R_ == t2) {  // "both should be R"
                if (!unify_args(w1, w2))
                    return false;
            } else
                return false;
#if 0
            cout << "    unify loop: unify_stack.getTop() NOW =" << unify_stack.getTop() << endl;
#endif
        }
    }
    return true;
}

bool Engine::unify_args(int w1, int w2) { // w1 & w1 already detagged in unify()
    assert(isArgOffset(cell_at(w1)) && isArgOffset(cell_at(w2)));
#if 0
    cout << "        Entered unify_args(" << w1 << "," << w2 << ")" << endl;
#endif

    cell v1 = cell_at(w1);
    cell v2 = cell_at(w2);
    // both should be A:
    if(cell::tagOf(v1) != cell::A_) abort();
    if(cell::tagOf(v2) != cell::A_) abort();
    int n1 = cell::detag(v1);
    int n2 = cell::detag(v2);
#if 0
    cout << "              n1=" << n1 << " n2=" << n2 << endl;
#endif
    if (n1 != n2)
        return false;
#if 0
    cout << "              continuing..." << endl;
#endif
    int b1 = 1 + w1;
    int b2 = 1 + w2;
#if 0
    cout << "               n1-1=" << n1 - 1 << endl;
#endif
    for (int i = n1 - 1; i >= 0; i--) {
        int i1 = b1 + i;
        int i2 = b2 + i;
#if 0
        cout << "                 i1=" << i1 << " i2=" << i2 << endl;
#endif
        cell u1 = cell_at(size_t(i1));
        cell u2 = cell_at(size_t(i2));
        if (u1 == u2) {
            continue;
        }
        unify_stack.push(u2);
        unify_stack.push(u1);
#if 0
        cout << "                  " << showCS("unify_stack", unify_stack) << endl;
        // cout << "                  " << showCS("heap", heap) << endl;
#endif
    }
    return true;
}

/**
  * Places a clause built by the Toks reader on the heap.
  */
Clause Engine::putClause(vector<cell> cells, vector<cell> &hgs, int neck) {

    int base = heap_size();
#if 0
    std::cout << "putClause() base = " << base << endl;
#endif
    // The following seems to depend on V==0 . . .
    assert (cell::V_ == 0);
    cell b = cell::tag(cell::V_, base);
    // ... because b is used later in '+' ops that would otherwise mangle tags.
    int len = int(cells.size());
    pushCells(b, 0, len, cells);

    // System.out.println ("---- putClause: hgs.length="+hgs.length+" -----");
#if 0
    cout << "### putClause: relocating " << endl;
#endif
    for (size_t i = 0; i < hgs.size(); i++) {
        cell c = hgs[i];
        cell cr = cell::relocate(b, c);
        hgs[i] = cr;
#if 0
        std::cout << "###    hgs[" << i << "] <- " << showCell(cr)
                  << " reloc'd from c=" << showCell(c) << endl;
#endif
    }

    t_index_vector index_vector = getIndexables(hgs[0]);

    Clause rc = Clause(len, hgs, base, neck, index_vector);

            // for (size_t i = 0; i < cells.size(); ++i) if (cell::tagOf(cells[i]) == cell::BAD) abort();
#if 0
    std::cout << "---- putClause: returning -----" << endl;
    cout << "---- base being set to " << base << endl;
#endif
    return rc;
}

void Engine::pp(string s) {
    std::cout << s << endl;
}

void Engine::clear() {
    /*for (Int i = 0; i < top; i++)
        set_cell([)size_t(i),0);*/
    heap.setTop(-1);
}

cstr Engine::heapCell(int w) {
    return cell::tagSym(cell::tagOf(w)) + ":" + cell::detag(w) + "[" + w + "]";
}

/**
 * Places an identifier in the symbol table.
 */
Integer *Engine::addSym(string sym) {
    try { return syms.at(sym); }
    catch (const std::exception& e) {
        Integer* I = new Integer(syms.size());
        syms.insert(pair<string, Integer*>(sym, I));
        slist.push_back(sym);
        assert(slist.size() == I->i);
        return I;
    }
}

/**
 * Returns the symbol associated to an integer index
 * in the symbol table.
 */
string Engine::getSym(int w) {
    if (w < 0 || w >= slist.size()) {
        cout << (cstr("BADSYMREF=") + w) << endl;
        abort();
    }
    return slist[w];
}

/*
 * Encodes string constants into symbols while leaving
 * other data types untouched.
 */
cell Engine::encode(int t, string s) {
    size_t w;
    try {
        w = stoi(s);
    }
    catch (const std::invalid_argument& e) {
        if (t == cell::C_)
            w = int(addSym(s)->i);
        else {
            cstr err = string("bad number form in encode=") + t + ":" + s + ", [" + e.what() + "]";
            throw logic_error(err);
        }
    }
    return cell::tag(t, w);
}

#if 0   // apparently not called
IntStack Engine::getSpine(const IntStack& cs) {
    Int a = cs[1];
    Int w = cell::detag(a);
    IntStack rs(w - 1);
    for (Int i = 0; i < w - 1; i++) {
        Int x = cs[3 + size_t(i)]; //////// "3"????????????
        Int t = cell::tagOf(x);
        if (cell::R_ != t)
            throw logic_error(cstr("*** getSpine: unexpected tag=") + t);
        rs[size_t(i)] = cell::detag(x);
    }
    return rs;
}
#endif

/*static*/ vector<int> &
Engine::put_ref(string arg,
                unordered_map<string, vector<int>> &refs,
                int clause_pos) {
    vector<int>& Is = refs[arg];
    if (Is.empty()) {
        Is = vector<int>();
        refs[arg] = Is;
#if 0
        std::cout << "        in put_ref, adding arg " << arg << " -> " << clause_pos << endl;
#endif
    }
    Is.push_back(clause_pos);
    return Is;
}

vector<Clause> Engine::dload(cstr s) {
#if 0
    std::cout << "Entering clauses, s.length() = " << s.length() << endl;
    std::cout << "s = " << s << endl;
#endif
    vector<vector<vector<string>>> clause_asm_list = Toks::toSentences(s);
    vector<Clause> compiled_clauses;

    for (vector<vector<string>> unexpanded_clause : clause_asm_list) {
        // map<string, IntStack> refs;
        unordered_map<string, vector<int>> refs = unordered_map<string,vector<int>>();
        vector<cell> cells;
        vector<cell> goals;
        int k = 0;
        for (vector<string> clause_asm : Toks::mapExpand(unexpanded_clause)) {
#if 0
            std::cout << endl << "... starting on a clause\n" << endl;
#endif
            size_t line_len = clause_asm.size();
#if 0
                    cout << "%%%%% line_len = " << line_len << " for..." << endl;
#endif
            goals.push_back(cell::reference(k++));
            cells.push_back(cell::argOffset(line_len));
            for (string cell_asm_code : clause_asm) {
#if 0
                std::cout << "  cell_asm_code=" << cell_asm_code << endl;
#endif
                if (1 == cell_asm_code.length())
                    cell_asm_code = "c:" + cell_asm_code;
                string arg = cell_asm_code.substr(2);

                switch (cell_asm_code[0]) {
                case 'c':   cells.push_back(encode(cell::C_, arg));     k++; break;
                case 'n':   cells.push_back(encode(cell::N_, arg));     k++; break;
                case 'v':   put_ref(arg, refs, k);
                                // std::cout << "    case v: refs[" << arg << "].push_back(" << k << ")" << endl;
                                // std::cout << "    refs[" << arg << "].size()=" << refs[arg].size() << endl;
                                // std::cout << "    &&&& v case, k = " << k << endl;
                            cells.push_back(cell::tag(cell::BAD, k));   k++; break;
                case 'h':   
                                // std::cout << "    case v: refs[" << arg << "].push_back(" << k << ")" << endl;
                                // std::cout << "    refs[" << arg << "].size()=" << refs[arg].size() << endl;
                            refs[arg].push_back(k-1);
                            assert(k > 0);
                            cells[size_t(k-1)] = cell::argOffset(line_len-1);
                            goals.pop_back();                               break;
                default:    throw logic_error(cstr("FORGOTTEN=") + cell_asm_code);
                }
            }
        }
#if 0
                std::cout << "***** refs dump ****" << endl;
                for (auto r : refs)
                    for (auto x : r.second)
                        std::cout << "  " << r.first << "->" << x << endl;
#endif
        assert(cells.size() > 0);
        assert(goals.size() > 0);

        linker(refs, cells, goals, compiled_clauses);
#if 0
        std::cout << "compiled_clauses length is now " << compiled_clauses.size() << endl;
#endif
    }

    size_t clause_count = compiled_clauses.size();
#if 0
    std::cout << "compiled clause_count=" << clause_count << endl;
#endif
    vector<Clause> all_clauses = vector<Clause>(clause_count);
    for (int i = 0; i < clause_count; i++) {
        all_clauses[i] = compiled_clauses[i];
    }

    return all_clauses;
 }

    void Engine::linker(unordered_map<string,vector<int>> refs,
                        vector<cell> &cells,
                        vector<cell> &goals,
                        vector<Clause> &compiled_clauses) {

        // final Iterator<IntStack> K = refs.values().iterator();
        // while (K.hasNext())
#if 0
        std::cout << "----- refs.size()=" << refs.size() << "-----" << endl;
        cout << "linker(): heap.getTop()=" << heap.getTop() << endl;
#endif

        
        for (auto kIs = refs.begin(); kIs != refs.end(); ++kIs) {
            //for (pair<string,vector<int>> kIs : refs) {
            vector<int> Is = kIs->second;
#if 0           
            std::cout << "Is.size() = " << Is.size() << endl;
#endif
            if (Is.size() == 0)
                continue;
            assert(goals.size() > 0);

            // "finding the A among refs" [Engine.java]
            bool found = false;
            size_t leader = -1;
            for (size_t j = 0; j < Is.size(); ++j)
                if (/*cell::isArgOffset(cells[j])*/
                    cell::tagOf(cells[Is[j]]) == cell::A_) {
                    leader = Is[j];
                    found = true;
#if 0
                    std::cout << "!!!! found at " << j << endl;
#endif
                    break;
                }
#if 0
            std::cout << "found = " << found << endl;
#endif
            if (!found) {
                // "for vars, first V others U" [Engine.java]
                leader = Is[0];
#if 0
                cout << "*** Leader not found, so leader <- " << leader << "***" <<endl;
#endif
                for (size_t i = 0; i < Is.size(); ++i)
                    if (Is[i] == leader)
                        cells[Is[i]] = cell::tag(cell::V_, Is[i]);
                    else
                        cells[Is[i]] = cell::tag(cell::U_, leader);
            }
            else {
#if 0
                std::cout << "========= leader found: " << leader << endl;
#endif
                for (size_t i = 0; i < Is.size(); ++i) {
                    if (Is[i] == leader)
                        continue;
                    cells[Is[i]] = cell::tag(cell::R_, leader);
                }
            }
        }
#if 0
        cout << "linker(): heap.getTop()=" << heap.getTop() << endl;
#endif

        int neck;
        if (1 == goals.size())
            neck = int(cells.size());
        else
            neck = cell::detag(goals[1L]);
#if 0
        std::cout << "In linker() with cells BEFORE putClause:" << endl;

        for (size_t i = 0; i < cells.size(); ++i) {
            std::cout << "cells[" << i << "]=" << showCell(cells[i]) << endl;
            // if (cell::tagOf(cells[i]) == cell::BAD) abort();
        }
#endif
        Clause C = putClause(cells, goals, neck); // safe to pass all?
#if 0
        std::cout << "In linker() AFTER putClause:" << endl;
#endif
        int len = int(cells.size());
#if 0
        cout << "        C.base=" << C.base << " len=" << len << endl;

        for (int i = 0; i < len; ++i) {
            std::cout << "heap[" << i + C.base << "]=" << showCell(heap.get(i + C.base)) << endl;
            if (cell::tagOf(cells[i]) == cell::BAD) abort();
        }
#endif
        compiled_clauses.push_back(C);
#if 0
        std::cout << endl << "&&&&&&&& linker() exiting: compiled_clauses.size()=" << compiled_clauses.size() << endl << endl;
#endif
    }

    //    was iota(clause_list.begin(), clause_list.end(), 0);   // 0..clauses.size()
    vector<size_t> Engine::toNums(vector<Clause> clauses) {
        size_t l = clauses.size();
        vector<size_t> cls = vector<size_t>(l);
        for (size_t i = 0; i < l; i++) {
            cls[i] = i;
        }
        return cls;
    }

 /**
  * Extracts a query - by convention of the form
  * goal(Vars):-body to be executed by the engine
  */
Clause Engine::getQuery() {
    return clauses[clauses.size() - 1];
}

/**
 * Returns the initial spine built from the query from which execution starts.
 */
Spine *Engine::init() {
    size_t base = heap_size();
#if 0
    std::cout << "init(): base=" << base << endl;
#endif
    Clause G = getQuery();
    vector<cell> empty;
#if 0
    std::cout << "init(): trail.getTop()=" << trail.getTop() << endl;
    std::cout << endl << "     *** new Spine being called with initial kount=0" << endl << endl;
#endif
    Spine *Q = new Spine(G.goal_refs, base, nullptr, trail.getTop(), 0, clause_list);
#if 0
    std::cout << "Q->kount=" << Q->kount << endl; // "index of the last clause [that]
                                            //  the top goal of [this] Spine
                                            //  has tried to match so far" [HHG doc]
#endif
    spines.push_back(Q);
    return Q;
}

/**
 * answer - "Returns an answer as a Spine while recording in it
 * the top of the trail to allow the caller to retrieve
 * more answers by forcing backtracking."
 */
Spine* Engine::answer(int trail_top) {
    return new Spine(spines[0]->head, trail_top);
}

/**
 * hasClauses - "Detects availability of alternative clauses for the
 * top goal of this spine."
 */
bool Engine::hasClauses(Spine* S) {
#if 0
    cout << "hasClauses: S->base=" << S->base << " S->k = " << S->kount << " S->unifiables.size() = " << S->unifiables.size() << endl;
#endif
    return S->kount < S->unifiables.size();  // "unifiables" here?????????????
        // "index of the last clause [that]
        //  the top goal of [this] Spine
        //  has tried to match so far" [HHG doc]
}

/**
 * ask - "Retrieves an answer and ensures the engine can be resumed
 * by unwinding the trail of the query Spine.
 * Returns an external "human readable" representation of the answer.
 *
 * "A key element in the interpreter loop is to ensure that
 * after an Engine yields an answer, it can, if asked to,
 * resume execution and work on computing more answers. [...]
 * A variable 'query' of type Spine, contains the top of the trail
 * as it was before evaluation of the last goal,
 * up to where bindings of the variables will have to be undone,
 * before resuming execution. It also unpacks the actual answer term
 * (by calling the method exportTerm) to a tree representation of a term,
 * consisting of recursively embedded arrays hosting as leaves,
 * an external representation of symbols, numbers and variables." [HHG doc]
 */
Object Engine::ask() {
#if 0
    cout << "entering ask():" << endl;

    cout << "   Before yield:" << endl;

    for (int k = 0; k < clauses.size(); k++) {
        cout << "    ### clauses[" << k << "].base=" << clauses[k].base << endl;
    }
    cout << endl;
#endif
    query = yield_();
    if (nullptr == query)
        return Object();
#if 0
    cout << "ask(): query->trail_top=" << query->trail_top << endl;
#endif
    auto ans = answer(query->trail_top);

    auto res = ans->head;
    auto R = exportTerm(res);
    unwindTrail(query->trail_top);
    delete ans;

    // delete query;   // leaky to delete this?
    query = nullptr;

    return R;
}
/**
 * unwindTrail - "Removes binding for variable cells
 * above savedTop." [Engine.java]
 */
void Engine::unwindTrail(int savedTop) {
#if 0
    cout << "unwindTrail: savedTop=" << savedTop << endl;
    cout << "unwindTrail: heap.getTop()=" << heap.getTop() << endl;
#endif
    while (savedTop < trail.getTop()) {
        cell href = trail.pop();
        assert(tagOf(href) == V_ || tagOf(href) == U_);
        int x = cell::detag(href);
#if 0       
        cout << "   href=" << showCell(href) << " detag(href) = " << x << endl;
#endif

        setRef(href, href);
    }
}

/**
 * popSpine - "Removes this spine from the spine stack and
 * resets trail and heap to where they were at its
 * creation time - while undoing variable binding
 * up to that point." [Engine.java]
 */
void Engine::popSpine() {

    Spine *G = spines.back();
    int new_base = int(G->base) - 1;
    int savedTop = G->trail_top;
    spines.pop_back();
    delete G;
    assert(spines_top >= 0);
#if 0
    cout << "popSpine: savedTop=" << savedTop << endl;
    cout << "trail.size()=" << trail.size() << endl;
#endif
    unwindTrail(savedTop);
    heap.setTop(new_base);
#if 0
    cout << "heap.getTop()=" << heap.getTop() << endl;
#endif
}

/**
 * yield_ "Main interpreter loop: starts from a spine and works
 * though a stream of answers, returned to the caller one
 * at a time, until the spines stack is empty - when it
 * returns null." [Engine.java]
 */
Spine* Engine::yield_() {
#if 0
    cout << "Entering yield_() with spines.size()=" << spines.size() << endl;
#endif
    while (!spines.empty()) {
        Spine* G = spines.back(); // was "peek()" in Java
#if 0
        cout << "----- Calling hasClauses, spines.size()=" << spines.size() << " ---- - " << endl;
#endif
        if (!hasClauses(G)) {
            popSpine();
            continue;
        }
        Spine *C = unfold(G);
        if (nullptr == C) {
            popSpine(); // no matches
            continue;
        }
        if (hasGoals(C)) {
            spines.push_back(C);
            continue;
        }
        return C; // answer
    }
    return nullptr;
}

Object Engine::exportTerm(cell x) {
#if 0
    cout << "exportTerm(" << showCell(x) <<"):" << endl;
#endif
    x = deref(x);
#if 0
    cout << "exportTerm(): x deref'd =" << showCell(x) << endl;
#endif
    int t = cell::tagOf(x);
    int w = cell::detag(x);

    Object res;
    switch (t) {
        case cell::C_: res = getSym(w);     break;
        case cell::N_: res = Integer(w);            break;
        case cell::V_: res = cstr("V") + w;         break;
            /*case U_:*/ 

        case cell::R_: {
                    cell a = cell_at(w);
                    if (!cell::isArgOffset(a)) {
                        // throw logic_error(cstr("*** should be A, found=") + showCell(a));
#if 0
                        cout << cstr("*** should be A, found=") << showCell(a) << endl;
                        cout << "x=" << showCell(x) << endl;
#endif
                        abort();
                    }
                    size_t n = cell::detag(a);
                    vector<Object> args;
                    size_t k = size_t(w) + 1;
                    for (size_t i = 0; i < n; i++) {
                        size_t j = k + i;

                            // cout << "  exportTerm recursive call, j=" << j << endl;

                        cell c = cell_at(j);
                        Object o = exportTerm(c);
                        args.push_back(o);
                    }
                    res = args;
                }
                break;
        default:
                    throw logic_error(cstr("*BAD TERM*") + showCell(x));
    }
    return res;
}

string Engine::showCell(cell w) {
    int t = cell::tagOf(w);
    int val = cell::detag(w);
    string s = "";
    string sym = "";

    switch (t) {
        case cell::V_:    s = cstr("v:") + val;        break;
        case cell::U_:    s = cstr("u:") + val;        break;
        case cell::N_:    s = cstr("n:") + val;        break;
        case cell::C_:    s = cstr("c:") + getSym(val);break;
        case cell::R_:    s = cstr("r:") + val;        break;
        case cell::A_:    s = cstr("a:") + val;        break;
        default:    s = cstr("*BAD*=") + w.as_int();
    }
    return s;
}

void Engine::relocateToTopOfHeap(cell b, vector<cell>& src, size_t from, size_t upto, size_t index) {

    int count = int(upto - from);
    ensureSize(count);
#if 0
    total_relocs += count; ++reloc_calls;
#endif

#if 0
    cout << "|||||||| relocateToTopOfHeap(" << b << ", src[size=" << src.size() << "], from=" << from
        << " upto=" << upto << " index=" << index << endl;
    cout << "|||||||| " << showCS("heap", heap);
#endif
    bool unroll = false; // fails because vector top not updated
    if (unroll) {
        const cell* pcs = src.data() + index;
        cell* phtop = (cell *) (heap.data() + heap.getTop());
#       define STEP *++phtop = cell::relocate(b, *pcs++)
        heap.setTop(heap.getTop() + count);
        while (count >= 4)
            STEP, STEP, STEP, STEP, count -= 4;
        switch (count) {
        case 3: STEP; case 2: STEP; case 1: STEP; case 0: break;
        }
#       undef STEP
    }
    else
        for (size_t i = from; i < upto; i++) {
            // heap.push(cell::relocate(b, src[index++]));
            heap.push(cell::relocate(b, heap.get(index + i)));
        }
#if 0
    cout << "|||||||| relocateToTopOfHeap after push:" << endl;
    cout << "|||||||| " << showCS("heap", heap) << endl;
#endif
}

/**
 * Pushes slice[from,to] at given base onto the heap.
 * b has cell structure, i.e, index, shifted left 3 bits, with tag 0 (==V)
 */
void Engine::pushCells(cell b, int from, int to, int base) {
#if 0
    cout << endl;
    cout << "??? pushCells(" << showCell(b) << " from=" << from << " to=" << to
         << " with base=" << base << endl;
#endif
    assert (tagOf(b) == V_);
    assert (V_ == 0);

    ensureSize(to - from);

    total_relocs += to - from;
    ++reloc_calls;
#if 1
    for (int i = from; i < to; i++) {
        heap.push(cell::relocate(b, heap.get(base + i)));;
    }
#else
    // broken now? No vector returned by heap.data().
    relocateToTopOfHeap(b, heap.data(), from, to, base);
#endif
}

/**
 * "Pushes slice[from,to] at given base onto the heap."
 *  TODO: Identical to pushToTopOfHeap()?
 * 
 */
void Engine::pushCells(cell b, int from, int to, vector<cell> cells) {
    assert (cell::tagOf(b) == cell::V_);
    assert (cell::V_ == 0);
    ensureSize(to - from);

    total_relocs += to - from;
    ++reloc_calls;

    for (int i = from; i < to; i++) {
        heap.push(cell::relocate(b, cells[i]));
    }
}

/**
 * Copies and relocates body of clause at offset from heap to heap
 * while also placing head as the first element of array 'goals' that,
 * when returned, contains references to the toplevel spine of the clause.
 */
vector<cell> Engine::pushBody(cell b, cell head, Clause &C) {
    assert (tagOf(b) == V_);
    assert (V_ == 0);
    pushCells(b, C.neck, C.len, C.base);
    size_t l = C.goal_refs.size();
    vector<cell> goals(l);
    goals[0] = head;
    for (size_t k = 1; k < l; k++) {
        goals[k] = cell::relocate(b, C.goal_refs[k]);
    }
    return goals;
}

void Engine::makeIndexArgs(Spine *G, cell goal) {
#if 0
    cout << "makeIndexArgs() entered..." << endl;
#endif
    if (G->index_vector[0] != -1 || G->goals->size() == 0)
        return;
#if 0
    cout << "  makeIndexArgs() found work to do..." << endl;
#endif
    size_t p = 1L + size_t(cell::detag(goal));
    size_t n = min(MAXIND, cell::detag(getRef(goal)));
    for (int i = 0; i < n; i++) {
        G->index_vector[size_t(i)] = cell2index(deref(cell_at(p + i))).as_int();
#if 0
        cout << "    G->index_vector[" << i << "]=" << G->index_vector[size_t(i)] << endl;
#endif
    }

    //if (imaps) throw "IMap TBD";
}

t_index_vector Engine::getIndexables(cell ref) {
    size_t p = 1L + size_t(cell::detag(ref));
    size_t n = cell::detag(getRef(ref));
    t_index_vector index_vector = { -1,-1,-1 };
    for (size_t i = 0; i < MAXIND && i < n; i++) {
        cell c = deref(cell_at(p + i));
        index_vector[size_t(i)] = cell2index(c).as_int();
    }
    return index_vector;
}

cell Engine::cell2index(cell c) {
    cell x = 0;
    int t = cell::tagOf(c);
    switch (t) {
    case cell::R_:
        x = getRef(c);
        break;
    case cell::C_:
    case cell::N_:
        x = c;
        break;
    }
    return x;
}

/**
 * Copies and relocates the head of clause C from heap to heap.
 */
cell Engine::pushHeadtoHeap(cell b, const Clause& C) {
#if 0
    cout << "+++ pushHead:" << " b = " << showCell(b)
         << " C.neck = " << C.neck
        << " C.base = " << C.base << endl;
    cout << showCS("+++ pushHead entered with heap", heap) << endl;
#endif
    pushCells(b, 0, C.neck, C.base);

    cell head = C.goal_refs[0];
    cell reloc_head = cell::relocate(b, head);
#if 0
    cout << "+++ pushHead: head=" << showCell(head)
         << " reloc_head=" << showCell(reloc_head) << endl;
    cout << showCS("+++ pushHead exiting with heap", heap) << endl;
#endif
    return reloc_head;
}

void Engine::put(t_index_vector& keys, int val) {
    for (int i = 0; i < imaps.size(); i++) {
        int key = keys[i];
        if (key != 0) {
            IMap::put_(imaps, i, key, val);
        }
        else {
            var_maps[size_t(i)][val] = val;
        }
    }
}

vector<IMap> Engine::index(vector<Clause> clauses) {
    if (clauses.size() < START_INDEX)
        return vector<IMap>();  // something minimal, IFFY
#if 0
    cout << "index(): var_maps.size()=" << var_maps.size() << endl;
#endif
    imaps = vector<IMap>(var_maps.size());
#if 0
    cout << "index(): imaps.size()=" << imaps.size() << endl;
#endif
    for (size_t i = 0; i < clauses.size(); i++) {
        Clause c = clauses[i];
        put(c.index_vector, int(i + 1)); // $$$ UGLY INC
        // because possible_match() is using 0 as "ignore"
    }
    /*
    pp("INDEX");
    pp(T(imaps));
    pp(T(vmaps));
    pp("");
    */
    return imaps;
}

string Engine::showCells2(size_t base, size_t len) {
    string buf;
    for (size_t k = 0; k < len; k++) {
        cell instr = cell_at(base + k);
        buf += cstr("[") + (int)(base + k) + "]" + showCell(instr) + " ";
    }
    return buf;
}
string Engine::showCells1(vector<cell> cs) {
    string buf;
    for (size_t k = 0; k < cs.size(); k++)
        buf += cstr("[") + k + "]" + showCell(cs[k]) + " ";
    return buf;
}

}
