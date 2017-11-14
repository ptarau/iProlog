package iProlog;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;

/**
 * Implements execution mechanism
 */
class Engine {

    private static final int MAXIND = 3; // number of index args
    private static final int START_INDEX = 20;
    // switches off indexing for less then START_INDEX clauses e.g. <20
    /**
     * tags of our heap cells - that can also be seen as
     * instruction codes in a compiled implementation
     */
    private static final int V = 0;
    private static final int U = 1;
    private static final int R = 2;
    private static final int C = 3;
    private static final int N = 4;
    private static final int A = 5;
    private static final int BAD = 7;
    private static final int MINSIZE = 1 << 15; // power of 2
    /**
     * trimmed down clauses ready to be quickly relocated to the heap
     */
    final Clause[] clauses;
    private final int[] cls;
    /**
     * symbol table made of map + reverse map from ints to syms
     */

    final LinkedHashMap<String, Integer> syms;
    private final IMap<Integer>[] imaps;
    private final IntMap[] vmaps;
    private final ArrayList<String> slist;
    private final IntStack trail;
    private final IntStack ustack;
    private final ObStack<Spine> spines = new ObStack<>();
    private Spine query;
    /**
     * runtime areas:
     * <p>
     * the heap contains code for and clauses their their copies
     * created during execution
     * <p>
     * the trail is an undo list for variable bindings
     * that facilitates retrying failed goals with alternative
     * matching clauses
     * <p>
     * the unification stack ustack helps handling term unification non-recursively
     * <p>
     * the spines stack contains abstractions of clauses and goals and performs the
     * functions of  both a choice-point stack and goal stack
     * <p>
     * imaps: contains indexes for up toMAXIND>0 arg positions (0 for pred symbol itself)
     * <p>
     * vmaps: contains clause numbers for which vars occur in indexed arg positions
     */

    private int heap[];
    private int top;

    // G - ground?

    /**
     * Builds a new engine from a natural-language style assembler.nl file
     */
    Engine(String fname) {
        syms = new LinkedHashMap<>();
        slist = new ArrayList<>();

        makeHeap();

        trail = new IntStack();
        ustack = new IntStack();

        clauses = dload(fname);

        cls = toNums(clauses);

        query = init();

        vmaps = vcreate(MAXIND);
        imaps = index(clauses, vmaps);
    }

    /**
     * tags an integer value while fliping it into a negative
     * number to ensure that untagged cells are always negative and the tagged
     * ones are always positive - a simple way to ensure we do not mix them up
     * at runtime
     */
    private static final int tag(int t, int w) {
        return -((w << 3) + t);
    }

    /**
     * removes tag after flipping sign
     */
    private static final int detag(int w) {
        return -w >> 3;
    }

    /**
     * extracts the tag of a cell
     */
    private static final int tagOf(int w) {
        return -w & 7;
    }

    /**
     * expands a "Xs lists .." statements to "Xs holds" statements
     */

    private static final ArrayList<String[]> maybeExpand(ArrayList<String> Ws) {
        String W = Ws.get(0);
        if (W.length() < 2 || !"l:".equals(W.substring(0, 2)))
            return null;

        int l = Ws.size();
        ArrayList<String[]> Rss = new ArrayList<>();
        String V = W.substring(2);
        for (int i = 1; i < l; i++) {
            String[] Rs = new String[4];
            String Vi = 1 == i ? V : V + "__" + (i - 1);
            String Vii = V + "__" + i;
            Rs[0] = "h:" + Vi;
            Rs[1] = "c:list";
            Rs[2] = Ws.get(i);
            Rs[3] = i == l - 1 ? "c:nil" : "v:" + Vii;
            Rss.add(Rs);
        }
        return Rss;

    }

    /**
     * expands, if needed, "lists" statements in sequence of statements
     */
    private static final ArrayList<String[]> mapExpand(ArrayList<ArrayList<String>> Wss) {
        ArrayList<String[]> Rss = new ArrayList<>();
        for (ArrayList<String> Ws : Wss) {

            ArrayList<String[]> Hss = maybeExpand(Ws);

            if (null == Hss) {
                String[] ws = new String[Ws.size()];
                for (int i = 0; i < ws.length; i++) {
                    ws[i] = Ws.get(i);
                }
                Rss.add(ws);
            } else {
                Rss.addAll(Hss);
            }
        }
        return Rss;
    }

    private static final int[] toNums(Clause[] clauses) {
        int l = clauses.length;
        int[] cls = new int[l];
        for (int i = 0; i < l; i++) {
            cls[i] = i;
        }
        return cls;
    }

    /**
     * true if cell x is a variable
     * assumes that variables are tagged with 0 or 1
     */
    private static final boolean isVAR(int x) {
        //final int t = tagOf(x);
        //return V == t || U == t;
        return tagOf(x) < 2;
    }

    /**
     * extracts an integer array pointing to
     * the skeleton of a clause: a cell
     * pointing to its head followed by cells pointing to its body's
     * goals
     */
    static int[] getSpine(int[] cs) {
        int a = cs[1];
        int w = detag(a);
        int[] rs = new int[w - 1];
        for (int i = 0; i < w - 1; i++) {
            int x = cs[3 + i];
            int t = tagOf(x);
            if (R != t) {
                Main.pp("*** getSpine: unexpected tag=" + t);
                return null;
            }
            rs[i] = detag(x);
        }
        return rs;
    }

    /**
     * relocates a variable or array reference cell by b
     * assumes var/ref codes V,U,R are 0,1,2
     */
    private static final int relocate(int b, int cell) {
        return tagOf(cell) < 3 ? cell + b : cell;
    }

    /**
     * tests if the head of a clause, not yet copied to the heap
     * for execution could possibly match the current goal, an
     * abstraction of which has been place in xs
     */
    static boolean match(int[] xs, Clause C0) {
        for (int i = 0; i < MAXIND; i++) {
            int x = xs[i];
            if (0 == x)
                continue;
            int y = C0.xs[i];
            if (0 == y) {
                continue;
            }
            if (x != y)
                return false;
        }
        return true;
    }

    /**
     * detects availability of alternative clauses for the
     * top goal of this spine
     */
    private static boolean hasClauses(Spine S) {
        return S.k < S.cs.length;
    }

    /**
     * true when there are no more goals left to solve
     */
    private static boolean hasGoals(Spine S) {
        return !IntList.isEmpty(S.gs);
    }

    private static IntMap[] vcreate(int l) {
        IntMap[] vss = new IntMap[l];
        for (int i = 0; i < l; i++) {
            vss[i] = new IntMap();
        }
        return vss;
    }

    private static void put(IMap<Integer>[] imaps, IntMap[] vss, int[] keys, int val) {
        for (int i = 0; i < imaps.length; i++) {
            int key = keys[i];
            if (key != 0) {
                IMap.put(imaps, i, key, val);
            } else {
                vss[i].add(val);
            }
        }
    }

    private static IMap<Integer>[] index(Clause[] clauses, IntMap[] vmaps) {
        if (clauses.length < START_INDEX)
            return null;

        IMap<Integer>[] imaps = IMap.create(vmaps.length);
        for (int i = 0; i < clauses.length; i++) {
            Clause c = clauses[i];

            put(imaps, vmaps, c.xs, i + 1); // $$$ UGLY INC

        }
        Main.pp("INDEX");
        Main.pp(IMap.show(imaps));
        Main.pp(Arrays.toString(vmaps));
        Main.pp("");
        return imaps;
    }

    /**
     * places an identifier in the symbol table
     */
    private final int addSym(String sym) {
        Integer I = syms.get(sym);
        if (null == I) {
            int i = syms.size();
            I = i;
            syms.put(sym, I);
            slist.add(sym);
        }
        return I;
    }

    /**
     * returns the symbol associated to an integer index
     * in the symbol table
     */
    private final String getSym(int w) {
        if (w < 0 || w >= slist.size())
            return "BADSYMREF=" + w;
        return slist.get(w);
    }

    private final void makeHeap() {
        makeHeap(MINSIZE);
    }

    private final void makeHeap(int size) {
        heap = new int[size];
        clear();
    }

    private final int getTop() {
        return top;
    }

    private final int setTop(int top) {
        return this.top = top;
    }

    private final void clear() {
        //for (int i = 0; i <= top; i++)
        //heap[i] = 0;
        top = -1;
    }

    /**
     * Pushes an element - top is incremented frirst than the
     * element is assigned. This means top point to the last assigned
     * element - which can be returned with peek().
     */
    private final void push(int i) {
        heap[++top] = i;
    }

    private int size() {
        return top + 1;
    }

    /**
     * dynamic array operation: doubles when full
     */
    private final void expand() {
        int l = heap.length;
        int[] newstack = new int[l << 1];

        System.arraycopy(heap, 0, newstack, 0, l);
        heap = newstack;
    }

    private void ensureSize(int more) {
        if (1 + top + more >= heap.length) {
            expand();
        }
    }

    /**
     * loads a program from a .nl file of
     * "natural language" equivalents of Prolog/HiLog statements
     */
    private Clause[] dload(String s) {
        boolean fromFile = true;
        ArrayList<ArrayList<ArrayList<String>>> Wsss = Toks.sentences(s, fromFile);

        ArrayList<Clause> Cs = new ArrayList<>();

        for (ArrayList<ArrayList<String>> Wss : Wsss) {
            // clause starts here

            LinkedHashMap<String, IntStack> refs = new LinkedHashMap<>();
            IntStack cs = new IntStack();
            IntStack gs = new IntStack();

            ArrayList<String[]> Rss = mapExpand(Wss);
            int k = 0;
            for (String[] ws : Rss) {

                // head or body element starts here

                int l = ws.length;
                gs.push(tag(R, k++));
                cs.push(tag(A, l));

                for (String w : ws) {

                    // head or body subterm starts here

                    if (1 == w.length()) {
                        w = "c:" + w;
                    }

                    String L = w.substring(2);

                    switch (w.charAt(0)) {
                        case 'c':
                            cs.push(encode(C, L));
                            k++;
                            break;
                        case 'n':
                            cs.push(encode(N, L));
                            k++;
                            break;
                        case 'v': {
                            IntStack Is = refs.computeIfAbsent(L, k1 -> new IntStack());
                            Is.push(k);
                            cs.push(tag(BAD, k)); // just in case we miss this
                            k++;
                        }
                        break;
                        case 'h': {
                            IntStack Is = refs.computeIfAbsent(L, k1 -> new IntStack());
                            Is.push(k - 1);
                            cs.set(k - 1, tag(A, l - 1));
                            gs.pop();
                        }
                        break;
                        default:
                            Main.pp("FORGOTTEN=" + w);
                    } // end subterm
                } // end element
            } // end clause

            // linker

            for (IntStack Is : refs.values()) {
                // finding the A among refs
                int leader = -1;
                for (int j : Is.toArray()) {
                    if (A == tagOf(cs.get(j))) {
                        leader = j;

                        break;
                    }
                }
                if (-1 == leader) {
                    // for vars, first V others U
                    leader = Is.get(0);
                    for (int i : Is.toArray()) {
                        if (i == leader) {
                            cs.set(i, tag(V, i));
                        } else {
                            cs.set(i, tag(U, leader));
                        }

                    }
                } else {
                    for (int i : Is.toArray()) {
                        if (i == leader) {
                            continue;
                        }
                        cs.set(i, tag(R, leader));
                    }
                }
            }

            int neck = 1 == gs.size() ? cs.size() : detag(gs.get(1));
            int[] tgs = gs.toArray();

            Clause C = putClause(cs.toArray(), tgs, neck);

            Cs.add(C);

        } // end clause set

        int ccount = Cs.size();
        Clause[] cls = new Clause[ccount];
        for (int i = 0; i < ccount; i++) {
            cls[i] = Cs.get(i);
        }
        return cls;
    }

    /*
     * encodes string constants into symbols while leaving
     * other data types untouched
     */
    private final int encode(int t, String s) {
        int w;
        try {
            w = Integer.parseInt(s);
        } catch (Exception ignored) {
            if (C == t) {
                w = addSym(s);
            } else
                //pp("bad in encode=" + t + ":" + s);
                return tag(BAD, 666);
        }
        return tag(t, w);
    }

    /**
     * returns the heap cell another cell points to
     */
    private int getRef(int x) {
        return heap[detag(x)];
    }

    /*
     * sets a heap cell to point to another one
     */
    private final void setRef(int w, int r) {
        heap[detag(w)] = r;
    }

    /**
     * removes binding for variable cells
     * above savedTop
     */
    private void unwindTrail(int savedTop) {
        while (savedTop < trail.getTop()) {
            int href = trail.pop();
            // assert href is var

            setRef(href, href);
        }
    }

    /**
     * scans reference chains starting from a variable
     * until it points to an unbound root variable or some
     * non-variable cell
     */
    private final int deref(int x) {
        while (isVAR(x)) {
            int r = getRef(x);
            if (r == x) {
                break;
            }
            x = r;
        }
        return x;
    }

    /**
     * raw display of a term - to be overridden
     */
    String showTerm(int x) {
        return showTerm(exportTerm(x));
    }

    /**
     * raw display of a externalized term
     */
    String showTerm(Object O) {
        if (O instanceof Object[])
            return Arrays.deepToString((Object[]) O);
        return O.toString();
    }

    /**
     * prints out content of the trail
     */
    void ppTrail() {
        for (int i = 0; i <= trail.getTop(); i++) {
            int t = trail.get(i);
            Main.pp("trail[" + i + "]=" + showCell(t) + ':' + showTerm(t));
        }
    }

    /**
     * builds an array of embedded arrays from a heap cell
     * representing a term for interaction with an external function
     * including a displayer
     */
    private Object exportTerm(int x) {
        x = deref(x);

        int t = tagOf(x);
        int w = detag(x);

        Object res = null;
        switch (t) {
            case C:
                res = getSym(w);
                break;
            case N:
                res = w;
                break;
            case V:
                //case U:
                res = "V" + w;
                break;
            case R: {

                int a = heap[w];
                if (A != tagOf(a))
                    return "*** should be A, found=" + showCell(a);
                int n = detag(a);
                Object[] arr = new Object[n];
                int k = w + 1;
                for (int i = 0; i < n; i++) {
                    int j = k + i;
                    arr[i] = exportTerm(heap[j]);
                }
                res = arr;
            }
            break;
            default:
                res = "*BAD TERM*" + showCell(x);
        }
        return res;
    }

    /**
     * raw display of a cell as tag : value
     */
    final String showCell(int w) {
        int t = tagOf(w);
        int val = detag(w);
        String s = null;
        switch (t) {
            case V:
                s = "v:" + val;
                break;
            case U:
                s = "u:" + val;
                break;
            case N:
                s = "n:" + val;
                break;
            case C:
                s = "c:" + getSym(val);
                break;
            case R:
                s = "r:" + val;
                break;
            case A:
                s = "a:" + val;
                break;
            default:
                s = "*BAD*=" + w;
        }
        return s;
    }

    /**
     * a displayer for cells
     */

    String showCells(int base, int len) {
        StringBuilder buf = new StringBuilder();
        for (int k = 0; k < len; k++) {
            int instr = heap[base + k];

            buf.append("[").append(base + k).append(']');
            buf.append(showCell(instr));
            buf.append(' ');
        }
        return buf.toString();
    }

    String showCells(int[] cs) {
        StringBuilder buf = new StringBuilder();
        for (int k = 0; k < cs.length; k++) {
            buf.append("[").append(k).append(']');
            buf.append(showCell(cs[k]));
            buf.append(' ');
        }
        return buf.toString();
    }

    /**
     * to be overridden as a printer of a spine
     */
    void ppc(Spine C) {
        // override
    }

    /**
     * to be overridden as a printer for current goals
     * in a spine
     */
    void ppGoals(IntList gs) {
        // override
    }

    /**
     * to be overriden as a printer for spines
     */
    void ppSpines() {
        // override
    }

    /**
     * unification algorithm for cells X1 and X2 on ustack that also takes care
     * to trail bindigs below a given heap address "base"
     */
    private final boolean unify(int base) {
        while (!ustack.isEmpty()) {
            int x1 = deref(ustack.pop());
            int x2 = deref(ustack.pop());
            if (x1 != x2) {
                int t1 = tagOf(x1);
                int t2 = tagOf(x2);
                int w1 = detag(x1);
                int w2 = detag(x2);

                if (isVAR(x1)) { /* unb. var. v1 */
                    if (isVAR(x2) && w2 > w1) { /* unb. var. v2 */
                        heap[w2] = x1;
                        if (w2 <= base) {
                            trail.push(x2);
                        }
                    } else { // x2 nonvar or older
                        heap[w1] = x2;
                        if (w1 <= base) {
                            trail.push(x1);
                        }
                    }
                } else if (isVAR(x2)) { /* x1 is NONVAR */
                    heap[w2] = x1;
                    if (w2 <= base) {
                        trail.push(x2);
                    }
                } else if (R == t1 && R == t2) { // both should be R
                    if (!unify_args(w1, w2))
                        return false;
                } else
                    return false;
            }
        }
        return true;
    }

    private final boolean unify_args(int w1, int w2) {
        int v1 = heap[w1];
        int v2 = heap[w2];
        // both should be A
        int n1 = detag(v1);
        int n2 = detag(v2);
        if (n1 != n2)
            return false;
        int b1 = 1 + w1;
        int b2 = 1 + w2;
        for (int i = n1 - 1; i >= 0; i--) {
            int i1 = b1 + i;
            int i2 = b2 + i;
            int u1 = heap[i1];
            int u2 = heap[i2];
            if (u1 == u2) {
                continue;
            }
            ustack.push(u2);
            ustack.push(u1);
        }
        return true;
    }

    /**
     * places a clause built by the Toks reader on the heap
     */
    private Clause putClause(int[] cs, int[] gs, int neck) {
        int base = size();
        int b = tag(V, base);
        int len = cs.length;
        pushCells(b, 0, len, cs);
        for (int i = 0; i < gs.length; i++) {
            gs[i] = relocate(b, gs[i]);
        }
        int[] xs = getIndexables(gs[0]);
        return new Clause(len, gs, base, neck, xs);
    }

    /**
     * pushes slice[from,to] of array cs of cells to heap
     */
    private final void pushCells(int b, int from, int to, int base) {
        ensureSize(to - from);
        for (int i = from; i < to; i++) {
            push(relocate(b, heap[base + i]));
        }
    }

    /**
     * pushes slice[from,to] of array cs of cells to heap
     */
    private final void pushCells(int b, int from, int to, int[] cs) {
        ensureSize(to - from);
        for (int i = from; i < to; i++) {
            push(relocate(b, cs[i]));
        }
    }

    /**
     * copies and relocates head of clause at offset from heap to heap
     */
    private final int pushHead(int b, Clause C) {
        pushCells(b, 0, C.neck, C.base);
        int head = C.hgs[0];
        return relocate(b, head);
    }

    /**
     * copies and relocates body of clause at offset from heap to heap
     * while also placing head as the first element of array gs that
     * when returned contains references to the toplevel spine of the clause
     */
    private final int[] pushBody(int b, int head, Clause C) {
        pushCells(b, C.neck, C.len, C.base);
        int l = C.hgs.length;
        int[] gs = new int[l];
        gs[0] = head;
        for (int k = 1; k < l; k++) {
            int cell = C.hgs[k];
            gs[k] = relocate(b, cell);
        }
        return gs;
    }

    /**
     * makes, if needed, registers associated to top goal of a Spine
     * these registers will be reused when matching with candidate clauses
     * note that xs contains dereferenced cells - this is done once for
     * each goal's toplevel subterms
     */
    private final void makeIndexArgs(Spine G, int goal) {
        if (null != G.xs)
            return;

        int p = 1 + detag(goal);
        int n = Math.min(MAXIND, detag(getRef(goal)));

        int[] xs = new int[MAXIND];

        for (int i = 0; i < n; i++) {
            int cell = deref(heap[p + i]);
            xs[i] = cell2index(cell);
        }

        G.xs = xs;

        if (null == imaps)
            return;
        int[] cs = IMap.get(imaps, vmaps, xs);
        G.cs = cs;
    }

    private final int[] getIndexables(int ref) {
        int p = 1 + detag(ref);
        int n = detag(getRef(ref));
        int[] xs = new int[MAXIND];
        for (int i = 0; i < MAXIND && i < n; i++) {
            int cell = deref(heap[p + i]);
            xs[i] = cell2index(cell);
        }
        return xs;
    }

    private final int cell2index(int cell) {
        int x = 0;
        int t = tagOf(cell);
        switch (t) {
            case R:
                x = getRef(cell);
                break;
            case C:
            case N:
                x = cell;
                break;
            // 0 otherwise - assert: tagging with R,C,N <>0
        }
        return x;
    }

    /**
     * transforms a spine containing references to choice point and
     * immutable list of goals into a new spine, by reducing the
     * first goal in the list with a clause that successfully
     * unifies with it - in which case places the goals of the
     * clause at the top of the new list of goals, in reverse order
     */
    private final Spine unfold(Spine G) {

        int ttop = trail.getTop();
        int htop = getTop();
        int base = htop + 1;

        int goal = IntList.head(G.gs);

        makeIndexArgs(G, goal);

        int last = G.cs.length;
        for (int k = G.k; k < last; k++) {
            Clause C0 = clauses[G.cs[k]];

            if (!match(G.xs, C0))
                continue;

            int base0 = base - C0.base;
            int b = tag(V, base0);
            int head = pushHead(b, C0);

            ustack.clear(); // set up unification stack

            ustack.push(head);
            ustack.push(goal);

            if (!unify(base)) {
                unwindTrail(ttop);
                setTop(htop);
                continue;
            }
            int[] gs = pushBody(b, head, C0);
            IntList newgs = IntList.tail(IntList.app(gs, IntList.tail(G.gs)));
            G.k = k + 1;
            return !IntList.isEmpty(newgs) ? new Spine(gs, base, IntList.tail(G.gs), ttop, 0, cls) : answer(ttop);
        } // end for
        return null;
    }

    /**
     * extracts a query - by convention of the form
     * goal(Vars):-body to be executed by the engine
     */
    private Clause getQuery() {
        return clauses[clauses.length - 1];
    }

    /**
     * returns the initial spine built from the
     * query from which execution starts
     */
    private Spine init() {
        int base = size();

        Clause G = getQuery();
        Spine Q = new Spine(G.hgs, base, IntList.empty, trail.getTop(), 0, cls);
        spines.push(Q);
        return Q;
    }

    /**
     * returns an answer as a Spine while recording in it
     * the top of the trail to allow the caller to retrieve
     * more answers by forcing backtracking
     */
    private final Spine answer(int ttop) {
        return new Spine(spines.get(0).hd, ttop);
    }

    /**
     * removes this spines for the spine stack and
     * resets trail and heap to where they where at its
     * creating time - while undoing variable binding
     * up to that point
     */
    private final void popSpine() {
        Spine G = spines.pop();
        unwindTrail(G.ttop);
        setTop(G.base - 1);
    }

    // indexing extensions - ony active if START_INDEX clauses or more

    /**
     * main interpreter loop: starts from a spine and works
     * though a stream of answers, returned to the caller one
     * at a time, until the spines stack is empty - when it
     * returns null
     */
    private Spine yield() {
        while (!spines.isEmpty()) {
            Spine G = spines.peek();
            if (!hasClauses(G)) {
                popSpine(); // no clauses left
                continue;
            }
            Spine C = unfold(G);
            if (null == C) {
                popSpine(); // no matches
                continue;
            }
            if (hasGoals(C)) {
                spines.push(C);
                continue;
            }
            return C; // answer
        }
        return null;
    }

    /**
     * retrieves an answers and ensure the engine can be resumed
     * by unwinding the trail of the query Spine
     * returns an external "human readable" representation of the answer
     */
    Object ask() {
        query = yield();
        if (null == query)
            return null;
        int res = answer(query.ttop).hd;
        Object R = exportTerm(res);
        unwindTrail(query.ttop);
        return R;
    }

    /**
     * initiator and consumer of the stream of answers
     * generated by this engine
     */
    void run() {
        long ctr = 0L;
        for (; ; ctr++) {
            Object A = ask();
            if (null == A) {
                break;
            }
            if (ctr < 5) Prog.println("[" + ctr + "] " + "*** ANSWER=" + showTerm(A));
        }
        if (ctr > 5) Prog.println("...");
        Prog.println("TOTAL ANSWERS=" + ctr);
    }
}
