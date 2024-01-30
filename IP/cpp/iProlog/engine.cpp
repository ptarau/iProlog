/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */


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
 * unfold - "transforms a spine [G] containing references to choice point and
 * immutable list of goals into a new spine, by reducing the
 * first goal in the list with a clause that successfully
 * unifies with it - in which case places the goals of the
 * clause at the top of the new list of goals, in reverse order"
 */
Spine* Engine::unfold(Spine *G) {
    if (CellList::isEmpty(G->goals)) {
        return nullptr;
    }

    int saved_trail_top = trail.getTop();
    int saved_heap_top = heap.getTop();
    int base = saved_heap_top + 1;

    cell goal = CellList::head(G->goals);

if(indexing) {
    cout<<"... about to call makeIndexArgs()"<<endl;
    Ip->makeIndexArgs(heap, G, goal);
    cout<<"... about to call getn()"<<endl;
    G->unifiables = IMap::getn(Ip->imaps, Ip->var_maps, G->unifiables);
}

    size_t last = G->unifiables.size();

    for (int k = G->last_clause_tried; k < last; k++) {
        Clause* C0 = &clauses[G->unifiables[k]];
if(indexing) {
        if (!Ip->possible_match(G->index_vector, C0->index_vector))
            continue;
}
        int base0 = base - C0->base;
        cell b = cell::tag(cell::V_, base0);  // TODO - I really need a "heap index(offset)" type
        cell head = pushHeadtoHeap(b, *C0);
        unify_stack.clear();  // "set up unification stack" [Engine.java]
        unify_stack.push(head);
        unify_stack.push(goal);
 
        if (!unify(base)) {
            unwindTrail(saved_trail_top);            
            heap.setTop(saved_heap_top);
            continue;
        }

        vector<cell> goals = pushBody(b, head, *C0);
        shared_ptr<CellList> tl = CellList::tail(G->goals);
        G->last_clause_tried = k + 1;
        if (goals.size() != 0 || tl != nullptr) {
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
            return answer(saved_trail_top);
        }
    }
    return nullptr;
}

bool Engine::unify(int base) {
    while (!unify_stack.isEmpty()) {
        cell x1 = deref(unify_stack.pop());
        cell x2 = deref(unify_stack.pop());

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
                    }
                } else {                            // "x2 nonvar or older"
                    set_cell(w1,x2);
                    if (w1 <= base) {
                        trail.push(x1);
                    }
                }
            } else if (cell::isVAR(x2)) {           /* "x1 is NONVAR" */
                set_cell(w2,x1);
                if (w2 <= base) {
                    trail.push(x2);
                }
            } else if (cell::R_ == t1 && cell::R_ == t2) {  // "both should be R"
                if (!unify_args(w1, w2)) {
                    return false;
                }
            }
            else {
                return false;
            }
        }
    }
    return true;
}

bool Engine::unify_args(int w1, int w2) { // w1 & w1 already detagged in unify()
    assert(cell::isArgOffset(cell_at(w1)) && cell::isArgOffset(cell_at(w2)));

    cell v1 = cell_at(w1);
    cell v2 = cell_at(w2);

    // both should be A:
    if(cell::tagOf(v1) != cell::A_) abort();
    if(cell::tagOf(v2) != cell::A_) abort();
    int n1 = cell::detag(v1);
    int n2 = cell::detag(v2);

    if (n1 != n2)
        return false;

    int b1 = 1 + w1;
    int b2 = 1 + w2;

    for (int i = n1 - 1; i >= 0; i--) {
        int i1 = b1 + i;
        int i2 = b2 + i;

        cell u1 = cell_at(size_t(i1));
        cell u2 = cell_at(size_t(i2));
        if (u1 == u2) {
            continue;
        }
        unify_stack.push(u2);
        unify_stack.push(u1);
    }
    return true;
}

void Engine::clear() {
    heap.setTop(-1);
}

cstr Engine::heapCell(int w) {
    return cell::tagSym(cell::tagOf(w)) + ":" + cell::detag(w) + "[" + w + "]";
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

    //    was iota(clause_list.begin(), clause_list.end(), 0);
    vector<int> Engine::toNums(vector<Clause> clauses) {
        size_t l = clauses.size();
        vector<int> cls = vector<int>(l);
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
    int base = heap_size();
    Clause G = getQuery();
    Spine *Q = new Spine(G.goal_refs, base, nullptr, trail.getTop(), 0, clause_list);

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
    return S->last_clause_tried < S->unifiables.size();
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
 * before resuming execution." [HHG doc]
 * 
 * Moving to prog.cpp: "It also unpacks the actual answer term
 * (by calling the method exportTerm) to a tree representation of a term,
 * consisting of recursively embedded arrays hosting as leaves,
 * an external representation of symbols, numbers and variables." [HHG doc]
 */
cell Engine::ask() {
    query = yield();
    if (nullptr == query)
	return cell::null();

    auto ans = answer(query->trail_top);

    cell res = ans->head;
    unwindTrail(query->trail_top);
    delete ans;

    // delete query;   // leaky to delete this?
    query = nullptr;

    return res;
}
/**
 * unwindTrail - "Removes binding for variable cells
 * above savedTop." [Engine.java]
 */
void Engine::unwindTrail(int savedTop) {
    while (savedTop < trail.getTop()) {
        cell href = trail.pop();
        int x = cell::detag(href);
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
    
    unwindTrail(savedTop);
    heap.setTop(new_base);
}

/**
 * yield "Main interpreter loop: starts from a spine and works
 * though a stream of answers, returned to the caller one
 * at a time, until the spines stack is empty - when it
 * returns null." [Engine.java]
 */
Spine* Engine::yield() {
    while (!spines.empty()) {
        Spine* G = spines.back(); // was "peek()" in Java

        if (!hasClauses(G)) {
            popSpine();
            continue;
        }
        Spine *C = unfold(G);
        if (nullptr == C) {
            popSpine(); // no matches
            continue;
        }
        if (C->hasGoals()) {
            spines.push_back(C);
            continue;
        }
        return C; // answer
    }
    return nullptr;
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

/**
 * "Copies and relocates body of clause at offset from heap to heap
 * while also placing head as the first element of array 'goals' that,
 * when returned, contains references to the toplevel spine of the clause."
 */
vector<cell> Engine::pushBody(cell b, cell head, Clause &C) {
    CellStack::pushCells(heap, b, C.neck, C.len, C.base);
    int l = C.goal_refs.size();
    vector<cell> goals(l);
    goals[0] = head;
    if (is_raw)
	cell::cp_cells (b, C.goal_refs.data()+1, goals.data()+1, l-1);
    else
        for (int k = 1; k < l; k++)
            goals[k] = cell::relocate(b, C.goal_refs[k]);
    return goals;
}


/**
 * Copies and relocates the head of clause C from heap to heap.
 */
cell Engine::pushHeadtoHeap(cell b, const Clause& C) {
    CellStack::pushCells(heap, b, 0, C.neck, C.base);
    cell head = C.goal_refs[0];
    cell reloc_head = cell::relocate(b, head);
    return reloc_head;
}

} // namespace
