/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */


#include <map>
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

    n_matches = 0;

    trail.clear();
    makeHeap();

    clauses = dload(asm_nl_source); // load "natural language" source

    if (clauses.size() == 0) {
        throw logic_error(cstr("clauses: none"));
    }

    clause_list = toNums(clauses); // initially an array  [0..clauses.length-1]

    query = init(); /* initial spine built from query from which execution starts */

    size_t base = heap_size();          // should be just after any code on heap

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
    if (CellList::isEmpty(G->goals))
        return nullptr;

    int saved_trail_top = trail.getTop();
    int saved_heap_top = heap.getTop();
    int base = size_t(saved_heap_top) + 1;

    cell goal = CellList::head(G->goals);

    makeIndexArgs(G, goal);

    size_t last = G->unifiables.size();

    for (int k = G->last_clause_tried; k < last; k++) {
        Clause* C0 = &clauses[G->unifiables[k]];

        if (!possible_match(G->index_vector, *C0))
            continue;

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

/**
  * Places a clause built by the Toks reader on the heap.
  */
Clause Engine::putClause(vector<cell> cells, vector<cell> &hgs, int neck) {

    int base = heap_size();

    cell b = cell::tag(cell::V_, base);
    // ... because b is used later in '+' ops that would otherwise mangle tags.
    int len = int(cells.size());
    CellStack::pushCells(heap, b, 0, len, cells);

    if (RAW)
        cell::cp_cells(b, hgs.data(), hgs.data(), (int) hgs.size());
    else
        for (size_t i = 0; i < hgs.size(); i++)
            hgs[i] = cell::relocate(b, hgs[i]);

    t_index_vector index_vector = getIndexables(hgs[0]);

    Clause rc = Clause(len, hgs, base, neck, index_vector);

    return rc;
}


void Engine::clear() {
    heap.setTop(-1);
}

cstr Engine::heapCell(int w) {
    return cell::tagSym(cell::tagOf(w)) + ":" + cell::detag(w) + "[" + w + "]";
}

/**
 * "Places an identifier in the symbol table."
 */
Integer *Engine::addSym(string sym) {
    try { return syms.at(sym); }
    catch (const std::exception& e) {
        Integer* I = new Integer(syms.size());
        syms.insert(pair<string, Integer*>(sym, I));
        slist.push_back(sym);
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

/*static*/ vector<int> &
Engine::put_ref(string arg,
                unordered_map<string, vector<int>> &refs,
                int clause_pos) {
    vector<int>& Is = refs[arg];
    if (Is.empty()) {
        Is = vector<int>();
        refs[arg] = Is;
    }
    Is.push_back(clause_pos);
    return Is;
}

vector<Clause> Engine::dload(cstr s) {
    vector<vector<vector<string>>> clause_asm_list = Toks::toSentences(s);
    vector<Clause> compiled_clauses;

    for (vector<vector<string>> unexpanded_clause : clause_asm_list) {
        // map<string, IntStack> refs;
        unordered_map<string, vector<int>> refs = unordered_map<string,vector<int>>();
        vector<cell> cells;
        vector<cell> goals;
        int k = 0;
        for (vector<string> clause_asm : Toks::mapExpand(unexpanded_clause)) {

            size_t line_len = clause_asm.size();

            goals.push_back(cell::reference(k++));
            cells.push_back(cell::argOffset(line_len));
            for (string cell_asm_code : clause_asm) {
                if (1 == cell_asm_code.length())
                    cell_asm_code = "c:" + cell_asm_code;
                string arg = cell_asm_code.substr(2);

                switch (cell_asm_code[0]) {
                case 'c':   cells.push_back(encode(cell::C_, arg));     k++; break;
                case 'n':   cells.push_back(encode(cell::N_, arg));     k++; break;
                case 'v':   put_ref(arg, refs, k);
                            cells.push_back(cell::tag(cell::BAD, k));   k++; break;
                case 'h':   refs[arg].push_back(k-1);
                            assert(k > 0);
                            cells[size_t(k-1)] = cell::argOffset(line_len-1);
                            goals.pop_back();                               break;
                default:    throw logic_error(cstr("FORGOTTEN=") + cell_asm_code);
                }
            }
        }
        linker(refs, cells, goals, compiled_clauses);
    }

    size_t clause_count = compiled_clauses.size();
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
        
        for (auto kIs = refs.begin(); kIs != refs.end(); ++kIs) {
            vector<int> Is = kIs->second;
            if (Is.size() == 0)
                continue;
            assert(goals.size() > 0);

            // "finding the A among refs" [Engine.java]
            bool found = false;
            size_t leader = -1;
            for (size_t j = 0; j < Is.size(); ++j)
                if (cell::isArgOffset(cells[Is[j]])) {
                    leader = Is[j];
                    found = true;
                    break;
                }

            if (!found) {
                // "for vars, first V others U" [Engine.java]
                leader = Is[0];
                for (size_t i = 0; i < Is.size(); ++i)
                    if (Is[i] == leader)
                        cells[Is[i]] = cell::tag(cell::V_, Is[i]);
                    else
                        cells[Is[i]] = cell::tag(cell::U_, leader);
            }
            else {
                for (size_t i = 0; i < Is.size(); ++i) {
                    if (Is[i] == leader)
                        continue;
                    cells[Is[i]] = cell::tag(cell::R_, leader);
                }
            }
        }

        int neck;
        if (1 == goals.size())
            neck = int(cells.size());
        else
            neck = cell::detag(goals[1L]);

        Clause C = putClause(cells, goals, neck); // safe to pass all?

        int len = int(cells.size());

        compiled_clauses.push_back(C);
    }

    //    was iota(clause_list.begin(), clause_list.end(), 0);
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
 * before resuming execution. It also unpacks the actual answer term
 * (by calling the method exportTerm) to a tree representation of a term,
 * consisting of recursively embedded arrays hosting as leaves,
 * an external representation of symbols, numbers and variables." [HHG doc]
 */
Object Engine::ask() {
    query = yield_();
    if (nullptr == query)
        return Object();

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
 * yield_ "Main interpreter loop: starts from a spine and works
 * though a stream of answers, returned to the caller one
 * at a time, until the spines stack is empty - when it
 * returns null." [Engine.java]
 */
Spine* Engine::yield_() {
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
        if (hasGoals(C)) {
            spines.push_back(C);
            continue;
        }
        return C; // answer
    }
    return nullptr;
}

Object Engine::exportTerm(cell x) {

    x = deref(x);
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
                        throw logic_error(cstr("*** should be A, found=") + showCell(a));
                    }
                    int n = cell::detag(a);
                    vector<Object> args;
                    int k = w + 1;
                    for (int i = 0; i < n; i++) {
                        int j = k + i;
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
    if (RAW)
	cell::cp_cells (b, C.goal_refs.data()+1, goals.data()+1, l-1);
    else
        for (int k = 1; k < l; k++)
            goals[k] = cell::relocate(b, C.goal_refs[k]);
    return goals;
}

void Engine::makeIndexArgs(Spine *G, cell goal) {
    if (G->index_vector[0] != -1 || !hasGoals(G))
        return;
    int p = 1 + cell::detag(goal);
    int n = min(MAXIND, cell::detag(getRef(goal)));
    for (int i = 0; i < n; i++) {
        G->index_vector[i] = cell2index(deref(cell_at(p + i))).as_int();
    }
}

t_index_vector Engine::getIndexables(cell ref) {
    int p = 1 + cell::detag(ref);
    int n = cell::detag(getRef(ref));
    t_index_vector index_vector = { -1,-1,-1 };
    for (int i = 0; i < MAXIND && i < n; i++) {
        cell c = deref(cell_at(p + i));
        index_vector[i] = cell2index(c).as_int();
    }
    return index_vector;
}
#if 0
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
#endif

/**
 * Copies and relocates the head of clause C from heap to heap.
 */
cell Engine::pushHeadtoHeap(cell b, const Clause& C) {
    CellStack::pushCells(heap, b, 0, C.neck, C.base);
    cell head = C.goal_refs[0];
    cell reloc_head = cell::relocate(b, head);
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
    imaps = vector<IMap>(var_maps.size());
    for (size_t i = 0; i < clauses.size(); i++) {
        Clause c = clauses[i];
        put(c.index_vector, int(i + 1)); // $$$ UGLY INC
        // because possible_match() is using 0 as "ignore"
    }
    return imaps;
}

string Engine::showCells2(int base, int len) {
    string buf;
    for (int k = 0; k < len; k++) {
        cell instr = cell_at(base + k);
        buf += cstr("[") + base + k + "]" + showCell(instr) + " ";
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
