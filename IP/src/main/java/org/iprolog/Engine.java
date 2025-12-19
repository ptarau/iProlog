package org.iprolog;
import java.util.*;

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

public class Engine {
  // this approach to tracing adds a lot to runtimes
  // can be disabled completely with global search and replace "l_.l" -> "//l_.l"
  interface trace {
    void l(String str);
  }
  static trace l_on  = (s) -> { Prog.println(s); };
  static trace l_off = (s) -> { };

	int n_matches = 0;

  Spine query;

  final static int MAXIND = 3; // number of index args
  final static int START_INDEX = 1;  // if # of clauses < START_INDEX, turn off indexing

  /* (definalized a lot of these because I needed default constructor--MT)

  /* Trimmed-down clauses ready to be quickly relocated to the heap: */
  /* (Not clear what "trimmed-down" means.) */
  /*final*/ Clause[] clauses;

  /*final*/ int[] clause_list; // if no indexing, [0..clauses.length-1]

  /* Symbol table - made of map (syms) + reverse map from ints to syms (slist) */
  /*final*/ protected sym_tab symTab;

    /** Runtime areas: **/

   /* heap - contains code for 'and' clauses and their copies created during execution
   */
  private int[] heap;
  private int heap_top;
  static int MINSIZE = 1 << 15; // power of 2

  /* trail - undo list for variable bindings; facilitates retrying failed goals
   *   with alternative matching clauses
   */
  /*final*/ private IntStack trail;
  
  /* unify_stack
   * - unification stack; helps to handle term unification non-recursively
   *
   * (This could theoretically be made dynamically allocated by unfold()
   * and passed to unify/unify_args. Static saves time, even if it makes
   * cloning slightly slower.)
   */
  /*final*/ private IntStack unify_stack;
  
  /* spines - stack of abstractions of clauses and goals;
   *    both a choice-point stack and goal stack
   *
   * (Used pretty widely, needed as global.)
   */
  /*final*/ private ObStack<Spine> spines = new ObStack<>();

  /* (index - Initialized from clauses after clauses loaded. Clones
   * should be able to use this too.)
   */
  index Ip;

  /* (Engine pool of pre-warmed/used engines. UNIMPLEMENTED.) */
  static ArrayList<Engine> engines;

  public Engine() {
    symTab = null;
    heap = null;
    trail = null;
    unify_stack = null;
    clauses = null;
    clause_list = null;
    query = null;
    Ip = null;
  }

  public void init_engine() {
    symTab = new sym_tab();
    makeHeap();
    trail = new IntStack();
    unify_stack = new IntStack();
  }

  public void prep_clauses() {
    clause_list = toNums(clauses); // initially an array  [0..clauses.length-1]
    query = init();  /* initial spine built from query from which execution starts */
    Ip = new index(clauses);
  }

  /*
   * Builds a new engine from a natural-language-style assembler.nl file
   */
  public Engine(final String s, final boolean fromFile) {
    this.init_engine();

    // Main.println ("Calling dload_from_x");
    clauses = dload_from_x(s, fromFile); // load "natural language" source

    prep_clauses();
  }

  /**
   * Tags of our heap cells. These can also be seen as
   * instruction codes in a compiled implementation.
   * Tag marks a ....
   */
  final private static int V = 0; // ... first occurrence of a Variable in a clause
  final private static int U = 1; // ... subsequent occurrence (U for "Unbound"?)
  final private static int R = 2; // ... Ref to array slice representing a subterm

  final private static int C = 3; // ... Constant (index into a sym table)
  final private static int N = 4; // ... small iNteger

  final private static int A = 5; // ... Arity of array slice holding flattened term;
                                  // "(of size 1 + number of arguments, to also
                                  // make room for the function symbol --
                                  // that could be an atom or a variable.)" -- HHG doc
    // G - ground?

  final public static int n_tag_bits = 3;
  final private static int TAG_MASK = (1<<n_tag_bits)-1;
  final private static int BAD = TAG_MASK;
  final public static int MAX_N = 1 << (Integer.SIZE-(n_tag_bits+1));
  // to allow for tag & 1 sign bit -----------------------^

  /**
   * Tags an integer value while (optionally) flipping it into a negative
   * number when we want to ensure that untagged cells are always negative
   * and the tagged ones are always positive - a simple way to ensure we
   * do not mix them up at runtime. Not negating the argument doesn't
   * seem to affect results, and improves performance only slightly (< 5%).
   */
  private static int maybe_invert(final int w) {
	  return w;
	  // return -w;
  }

  private static int tag(final int t, final int w) {
    assert t <= BAD;
    assert t >= 0;
    return maybe_invert((w << n_tag_bits) + t);
  }

  /**
   * Removes tag after (maybe) flipping sign.
   */
  static int detag(final int w) {
    return maybe_invert(w) >> n_tag_bits;
  }

  /**
   * Extracts the tag of a cell.
   */
  static int tagOf(final int w) {
    return maybe_invert(w) & TAG_MASK;
  }

  private void makeHeap() {
    makeHeap(MINSIZE);
  }

  private void makeHeap(final int size) {
    heap = new int[size];
    clear_heap();
  }

  private int get_heap_top() {
    return heap_top;
  }

  private void set_heap_top(final int top) {
    this.heap_top = top;
  }

  private void clear_heap() {
    heap_top = -1;
  }

  /**
   * Pushes an element - top is incremented first, then the
   * element is assigned. This means top points to the last assigned
   * element - which can be returned with peek().
   */
  private void push_to_heap(final int i) {
    heap[++heap_top] = i;
  }

  final int heap_size() {
    return heap_top + 1;
  }

  /**
   * Dynamic array operation: heap size doubles when full
   * or when the heap would otherwise overflow from what will be
   * pushed onto it.
   */
  private void expand() {
    final int l = heap.length;
    final int[] newstack = new int[l << 1];

    System.arraycopy(heap, 0, newstack, 0, l);
    heap = newstack;
  }

  private void ensureSize(final int more) {
    if (1 + heap_top + more >= heap.length) {
      expand();
    }
  }

  private static Clause[] condensed_clauses(ArrayList<Clause> compiled_clauses) {
    final int clause_count = compiled_clauses.size();
    final Clause[] clauses = new Clause[clause_count];
    for (int i = 0; i < clause_count; i++) {
      clauses[i] = compiled_clauses.get(i);
    }
    return clauses;
  }

  /**
  * Expands "Xs lists .." statements to "Xs holds" statements.
  */

  private static ArrayList<String[]>
  expand_lists_to_holds(final ArrayList<String> words) {
    assert words != null;
    assert words.size() > 0;
    final String first_word = words.get(0);

    if (first_word.length() < 2)
      return null;

    String asm_prefix_tag = first_word.substring(0, 2);

    if(!"l:".equals(asm_prefix_tag))
      return null;

// Main.println ("expand_lists_to_holds: entering....");

    final int l = words.size();
    final int n_elts = l-1;
    final ArrayList<String[]> an_expansion = new ArrayList<>();
    final String V = first_word.substring(2);

// Main.println ("expand_lists_to_holds: l = " + l);
// Main.println ("expand_lists_to_holds: V = " + V);

// String s = "[";
// String sep = "";
// for (int i = 1; i < l; ++i) { s += (sep + words.get(i)); sep = ","; }
// s += "]";
// Main.println ("expand_lists_to_holds: list = " + s);

    String subscript = "";
    String sub = "_sub_";

    for (int i = 1; i < l; i++) {
      final String[] a_subexpansion = new String[4];
      a_subexpansion[0] = "h:" + V + subscript;      // 'h' -> "holds" -> "="
      a_subexpansion[1] = "c:list";
      a_subexpansion[2] = words.get(i);
      if (i == n_elts)  a_subexpansion[3] = "c:nil";
      else              a_subexpansion[3] = "v:" +  V + sub + i;

      subscript = sub + i;

// Main.println("\n");
// for (int j = 0; j < 4; ++j) Main.println ("expand_lists_to_holds: a_subexpansion["+j+"] = " + a_subexpansion[j]);
// Main.println ("for next, subscript = " + subscript);

      an_expansion.add(a_subexpansion);
    }

//    Main.println ("expand_lists_to_holds: exiting\n\n");

    return an_expansion;
  }
 
  /**
   * Expands, if needed, "lists" statements in sequence of statements.
   */
  private static ArrayList<String[]>
  expand_lists_stmts(final ArrayList<ArrayList<String>> Wss) {

    // Main.println("\n\nexpand_lists_stmts: entered....");
    // Main.println("  Wss = " + Wss);

    final ArrayList<String[]> Results = new ArrayList<>();
    for (final ArrayList<String> Ws : Wss) {

      assert Ws.size() > 0;
      final ArrayList<String[]> any_expansion = expand_lists_to_holds(Ws);

      if (null == any_expansion) {
        // Main.println("expand_lists_stmts: no list expansion....");
        final String[] ws = new String[Ws.size()];
        for (int i = 0; i < ws.length; i++) {
          ws[i] = Ws.get(i);
          // Main.println("expand_lists_stmts: ws[i] = " + ws[i]);
        }
        Results.add(ws);
      } else {
        for (final String[] X : any_expansion) {
          // for (String elt : X) Main.println("expand_lists_stmts:   adding elt " + elt);
          Results.add(X);
        }
      }
    }
    return Results;
  }

  static void put_ref(String arg,
                                LinkedHashMap<String, IntStack> refs,
                                int clause_pos) {   // guessing it means clause position
    IntStack Is = refs.get(arg);
    if (null == Is) {
      Is = new IntStack();
      refs.put(arg, Is);
    }
    Is.push (clause_pos);
  }

  /**
   * Loads a program from a .nl file (or a String) of
   * "natural language" equivalents of Prolog/HiLog statements.
   */
      // "W" seems to be an abbreviation of "Word". Each "s" indicates a level
      // of pluralization.
  Clause[] dload(final String s) {
    return dload_from_x(s, true);
  }
  Clause[] dload_from_x(final String s, Boolean fromFile) {

    // Main.println ("\ndload_from_x:s (len="+s.length()+") = \n" + s);

    final ArrayList<ArrayList<ArrayList<String>>> clause_asm_list = Toks.toSentences(s, fromFile);

    // Main.println ("clause_asm_list = " + clause_asm_list);

    final ArrayList<Clause> compiled_clauses = new ArrayList<>();

    for (final ArrayList<ArrayList<String>> clause_asm : clause_asm_list) {

      final LinkedHashMap<String, IntStack> refs = new LinkedHashMap<>();
      final IntStack cells = new IntStack();
      final IntStack goals = new IntStack();

      // Main.println ("Just before expand_lists_stmts(clause_asm)");
      // Main.println ("clause_asm = " + clause_asm);
      final ArrayList<String[]> raw_asm = expand_lists_stmts(clause_asm);
      assert raw_asm.size() > 0;
      int k = 0;
      for (final String[] ws : raw_asm) { // for each head or body element
// Main.println ("Stepping through raw_asm arraylist...");

        final int l = ws.length;

            // Prog.println("%%%% l = ws.length = " + l + " for...");

        goals.push(tag(R, k++));
        assert goals.size() > 0;
        cells.push(tag(A, l));

        for (String w : ws) { // gen code for 'element' (= head/body subterm)

          // Prog.println ("   ..." + w);

        // Main.println ("at w = " + w);

          if (1 == w.length())  // when would this be?
            w = "c:" + w;

          final String arg = w.substring(2);

          switch (w.charAt(0)) {  // gen code for subterm:
            // Constant
            case 'c': cells.push(encode(C, arg));              k++; break;
            // small iNt
            case 'n': cells.push(encode(N, arg));              k++; break;
            // Variable
            case 'v': put_ref (arg, refs, k);
                      //   "just in case we miss this:" ??
                      //    P. Tarau comment
                      // Prog.println("    &&&& v case, k = " + k);
                      cells.push(tag(BAD, k));                 k++; break;
            // 'Holds' ('=')
            case 'h': put_ref (arg, refs, k - 1);
                      cells.set(k - 1, tag(A, l - 1));
                      goals.pop();
                                                               /**/ break;
            default: Main.pp("FORGOTTEN=" + w);
          } // end subterm
        } // end element
      } // end clause

      assert cells.size() > 0;
      assert goals.size() > 0;

      // Prog.println("****** ref dump ******");
      // for (String sx : refs.keySet())
      //   Prog.println ("  " + sx + "->" + refs.get(sx));

      linker(refs, cells, goals, compiled_clauses);
    }  // end clause set

    final int clause_count = compiled_clauses.size();
    final Clause[] all_clauses = new Clause[clause_count];
    for (int i = 0; i < clause_count; i++) {
      all_clauses[i] = compiled_clauses.get(i);
    }
    return all_clauses;
  }

  void linker(  LinkedHashMap<String,IntStack> refs,
                IntStack cells,
                IntStack goals,
                ArrayList<Clause> Clauses) {

    assert cells.size() > 0;

            // Prog.println("cells upon entering linker()");
            // for (int i = 0; i < cells.size(); ++i) Prog.println(showCell(cells.get(i)));
            // Prog.println ("refs.size()=" + refs.size());

    for (IntStack Is : refs.values()) {
      // finding the A among refs
      int leader = -1;
      for (final int j : Is.toArray()) {
        if (A == tagOf(cells.get(j))) {
          leader = j;

          break;
        }
      }
      // Prog.println ("leader = " + leader);
      if (-1 == leader) {
        // for vars, first V others U
        leader = Is.get(0);
        // Prog.println ("Leader not found, so leader <- " + leader);
        for (final int i : Is.toArray()) {
          if (i == leader) {
            cells.set(i, tag(V, i));
          } else {
            cells.set(i, tag(U, leader));
          }

        }
      } else {

        // Prog.println("=============== leader found" + leader);

        for (final int i : Is.toArray()) {
          if (i == leader) {
            continue;
          }
          cells.set(i, tag(R, leader));
        }
      }
    }

    final int neck;
    if (1 == goals.size())
      neck = cells.size();
    else
      neck = detag(goals.get(1));

    final int[] skeleton = goals.toArray();

    assert skeleton.length > 0;

          // Prog.println("cells before entering putClause()");
          // for (int i = 0; i < cells.size(); ++i) Prog.println(showCell(cells.get(i)));

    final Clause C = putClause(cells.toArray(), skeleton, neck);

    Clauses.add(C);

  }

  // made public to make it callable from Prog
  public static int[]
  toNums(final Clause[] clauses) {
    assert(clauses != null);
    final int l = clauses.length;
    final int[] cls = new int[l];
    for (int i = 0; i < l; i++) {
      cls[i] = i;
    }
    return cls;
  }

  /*
   * Encodes string constants into symbols while leaving
   * other data types untouched.
   */
  private int encode(final int t, final String s) {
    int w;
    try {
      w = Integer.parseInt(s);
    } catch (final Exception e) {
      if (C == t) {
        w = symTab.addSym(s);
      } else
        //pp("bad in encode=" + t + ":" + s);
        return tag(BAD, 666);
    }
    return tag(t, w);
  }

  /**
   * True if cell x is a variable.
   * Assumes that variables are tagged with 0 or 1.
   */
  private static boolean isVAR(final int x) {
    //final int t = tagOf(x);
    //return V == t || U == t;
    assert V < 2;
    assert U < 2;
    return tagOf(x) < 2;
  }

  /**
   * Returns the heap cell another cell points to.
   */
  final int getRef(final int x) {
    return heap[detag(x)];
  }

  /*
   * Sets a heap cell to point to another one.
   */
  private void setRef(final int w, final int r) {
    heap[detag(w)] = r;
  }

  /**
   * Removes binding for variable cells
   * above savedTop.
   */
  private void unwindTrail(final int savedTop) {
	  // Prog.println("savedTop=" + savedTop);
	  // Prog.println("unwindTrail heap.getTop()=" + heap_top);
    while (savedTop < trail.getTop()) {
      final int href = trail.pop();

      assert tagOf(href) == V || tagOf(href) == U;

      // int x = detag(href);
      // Prog.println("   href=" + showCell(href) + " detag(href) = " + x);

      setRef(href, href);
    }
  }

  /**
   * Scans reference chains starting from a variable
   * until it points to an unbound root variable or some
   * non-variable cell.
   */
  private int deref(int x) {
    while (isVAR(x)) {
      final int r = getRef(x);
      if (r == x) { // unbound root variable
        break;
      }
      x = r;
    }
    return x;
  }

  /**
   * Raw display of a term - to be overridden.
   */
  String showTerm(final int x) {
    return showTerm(exportTerm(x));
  }

  /**
   * Raw display of an externalized term.
   */
  String showTerm(final Object O) {
    if (O instanceof Object[]) {
      return Arrays.deepToString((Object[]) O);
    }
    return O.toString();
  }

  /**
   * Prints out content of the trail.
   */
  void ppTrail() {
    for (int i = 0; i <= trail.getTop(); i++) {
      final int t = trail.get(i);
        Main.pp("trail[" + i + "]=" + showCell(t) + ":" + showTerm(t));
    }
  }

  /**
   * Object returned could be java
   *    Integer,
   *    String (if constant or variable),
   *    Object[] recursively rendered by exportTerm().
   * "Builds an array of embedded arrays from a heap cell
   * representing a term for interaction with an external function
   * including a displayer." - P. Tarau
   */
  Object exportTerm(int x) {
    x = deref(x);

    // Main.println("exportTerm: x=" + showCell(x));

    final int t = tagOf(x);
    final int w = detag(x);

    Object res;
    switch (t) {
      case C: // symbol
        res = symTab.getSym(w);
      break;
      case N: // integer
        res = new Integer(w);
      break;
      case V: // variable
        //case U:
        res = "V" + w;
      break;
      case R: { // reference
        // Main.println("R_:");
        final int a = heap[w];
        assert A == tagOf(a);
        final int n = detag(a);
        final Object[] arr = new Object[n];
        final int k = w + 1;   // offset to embedded array
        for (int i = 0; i < n; i++) {
          final int j = k + i;
          // Main.println("cell_at(" + j + ")=" + showCell(heap[j]));
          // Main.println("exportTerm(cell " + showCell(heap[j]) + ")=" + exportTerm(heap[j]).toString());
          arr[i] = exportTerm(heap[j]);
        }
        res = arr;
      }
      break;
      // Case U is commented out above, strangely.
      // Maybe exportTerm is called only when all variables are bound?
      default:
        res = "*BAD TERM*" + showCell(x);
    }
    return res;
  }

  /**
     * Extracts an integer array pointing to
     * the skeleton of a clause: a cell
     * pointing to its head followed by cells pointing to its body's
     * goals.
     * * *
     * It doesn't look like this is called.
   * "Skeleton" here seems to refer to the [H,B] pair, a "toplevel
   * skeleton abstracting away the main components of a Horn clause:
   * the variable referencing the head, followed by zero or more
   * references to the elements of the conjunctions forming
   * the body of the clause." [HHG doc]
   */
  static int[] getSpine(final int[] cells) {
    System.out.println ("********* getSpine entered *********");
    // cells[0] is apparently ignored. Why?
    // Not called from anywhere, so the mystery remains.
    // Is it "cells" or "conjunctions" or ...?
    final int a = cells[1];  // offset 1 not 0 because ... ?
    assert tagOf(a) == A;   // arity? Looks like
    final int w = detag(a); // == # of args + 1 for functor
    final int[] rs = new int[w - 1];  // subtract 1 because of functor being counted
    for (int i = 0; i < w - 1; i++) {
      final int x = cells[3 + i];
         // + 3 because ... 1 offset + 1 for arity, then + 1 for C: functor atom
      assert R == tagOf(x);
      rs[i] = detag(x);
    }
    return rs;
  }

  /**
   * Raw display of a cell as tag : value
   */
  final String showCell(final int w) {
    final int t = tagOf(w);
    final int val = detag(w);
    String s = "";
    switch (t) {
      case V:        s = "v:" + val;          break;
      case U:        s = "u:" + val;          break;
      case N:        s = "n:" + val;          break;
      case C:        s = "c:" + symTab.getSym(val);  break;
      case R:        s = "r:" + val;          break;
      case A:        s = "a:" + val;          break;

      default:       s = "*BAD*=" + w;
    }
    return s;
  }

  /**
   * Displayers for cells
   */

  String showCells(final int base, final int len) {
    final StringBuffer buf = new StringBuffer();
    for (int k = 0; k < len; k++) {
      final int instr = heap[base + k];

      buf.append("[" + (base + k) + "]");
      buf.append(showCell(instr));
      buf.append(" ");
    }
    return buf.toString();
  }

  String showCells(final int[] cells) {
    final StringBuffer buf = new StringBuffer();
    for (int k = 0; k < cells.length; k++) {
      buf.append("[" + k + "]");
      buf.append(showCell(cells[k]));
      buf.append(" ");
    }
    return buf.toString();
  }

  /**
  * to be overridden as a printer of a spine
  */
  void ppc(final Spine C) {
    // override
  }

  /**
   * to be overridden as a printer for current goals
   * in a spine
   */
  void ppGoals(final IntList goals) {
    // override
  }

  /**
   * "to be overridden as a printer for spines"
   */
  void ppSpines() {
    // override
  }

  String showSpine(String hdr, Spine sp) {
    String s = hdr + ":\n";

    s += ("  head="+showCell(sp.head) + " ");
    s += (" base="+sp.base);
    s += (" trail_top=" + sp.trail_top);
    s += (" last_clause_tried=" + sp.last_clause_tried + "\n");

    s += "  goals = [";
    String sep="";
    for (IntList ip = sp.goals; ip != null; ip = IntList.tail(ip)) {
      s += (sep + showCell(IntList.head(ip)));
      sep=",";
    }
    s += "]\n";

    assert(sp.unifiables!=null);
    s += ("  unifiables = [");
    sep="";
    for (int i = 0; i < sp.unifiables.length; ++i) {
      s += (sep+sp.unifiables[i]);
      sep =",";
    }
    s += "]";

    return s;
  }

  /**
   * Unification algorithm for cells X1 and X2 on unify_stack
   * that also takes care to trail bindings below a given heap address "base".
   *
   * w <= base means ...?
   * w2 > w1 means ...?
   */
  private boolean unify(final int base) {
    boolean tr=false;
    if(tr)Prog.println("    unify: base="+base);

    while (!unify_stack.isEmpty()) {                if(tr)Prog.println("      unify loop: while unify stack not empty...");
      final int x1 = deref(unify_stack.pop());      if(tr && tagOf(x1)==R) Prog.println("      unify loop:  "+showGoal("x1->", x1));
      final int x2 = deref(unify_stack.pop());      if(tr && tagOf(x2)==R) Prog.println("      unify loop:  "+showGoal("x2->", x2));

      if (x1 != x2) {                               if(tr)Prog.println("      unify loop:  x1(="+showCell(x1)+") != x2(="+showCell(x2)+")");
        final int w1 = detag(x1);
        final int w2 = detag(x2);

        if (isVAR(x1)) { /* unb. var. v1 */         if(tr)Prog.println("      unify loop:   unb. var. v1="+showCell(x1));
          if (isVAR(x2) && w2 > w1) {               if(tr)Prog.println("      unify loop:     unb. var. v2="+showCell(x2)+" && w2 > w1");
            heap[w2] = x1;                          if(tr)Prog.println("      unify loop:       heap[w2="+w2+"] = x1(="+showCell(x1));
            if (w2 <= base) {                       if(tr)Prog.println("      unify loop:       x1 & x2 vars, w2>w1, w2<=base:");
              trail.push(x2);                       if(tr)Prog.println("      unify loop:         x2(=" + showCell(x2) + ") pushed");
            }
          } else {                                  if(tr)Prog.println("      unify loop:     x1 var, x2 nonvar or older:");
            heap[w1] = x2;                          if(tr)Prog.println("      unify loop:       heap[w1="+w1+"] = x2(="+showCell(x2)+")");
            if (w1 <= base) {                       if(tr)Prog.println("      unify loop:       w1 <= base:");
              trail.push(x1);                       if(tr)Prog.println("      unify loop:         x1 pushed: " + showCell(x1));
            }
          }
        } else if (isVAR(x2)) {                     if(tr)Prog.println("      unify loop:   x1="+showCell(x2)+" is NONVAR, isVAR(x2="+showCell(x2)+")");
          heap[w2] = x1;                            if(tr)Prog.println("      unify loop:     heap[w2="+w2+"] = x1(="+showCell(x1));
          if (w2 <= base) {                         if(tr)Prog.println("      unify loop:     x1 nonvar, x2 var, w2 <= base, x2 older:");
            trail.push(x2);                         if(tr)Prog.println("      unify loop:       x2 pushed: " + showCell(x2));
          }
        } else if (R==tagOf(x1) && R==tagOf(x2)) {  if(tr)Prog.println("      unify loop:   both t1,t2 shd be R, unify_args("+w1+","+w2+")");
                                                    if(tr)Prog.println("      unify loop:    " + showGoal("x1->",x1) + " " + showGoal("x2->",x2));
          if (!unify_args(base, w1, w2)) {          if(tr)Prog.println("      unify loop:     unify_args failed, returning FALSE");
            return false;
          }                                         if(tr)Prog.println("      unify loop:     unify_args succeeded, continue while loop");
        } else {                                    if(tr)Prog.println("      unify loop:   not (R == t1 && R == t2), return FALSE");
          return false;
        }
      }                                             if(tr)Prog.println("      unify loop:  x1 == x2, loop again");
    }                                               if(tr)Prog.println("      unify loop: all unified, returning true");
    return true;
  }

  String showCS(String prompt, int from, IntStack cs) {
    String s = prompt + ":";
    for (int i = from; i < cs.size(); ++i) {
      s += " ";
      s += ("@"+i+":"+showCell(cs.get(i)));
    }
    return s;
  }

  String showHeap(String prompt, int from) {
    String s = prompt + ":";
    for (int i = from; i < heap_size(); ++i) {
      s += " ";
      s += ("@"+i+":"+showCell(heap[i]));
    }
    return s;
  }

  // (I have "n_args" here, but it seems it checks to also see if the functors are identical.
  // With indexing on, this should be guaranteed before entering unify_args.)
  private boolean unify_args(final int base, final int w1, final int w2) {
    final boolean tr=false;            if(tr)Prog.println("             |       Entered unify_args(" + w1 + "," + w2 + ")");
    final int v1 = heap[w1];
    assert tagOf(v1) == A;
    final int v2 = heap[w2];          if(tr)Prog.println("             |       v1=" + showCell(v1) + " v2=" + showCell(v2));
    assert tagOf(v2) == A;

    int n_args = detag(v1);           if(tr)Prog.println("             |       v1 n_args=" + n_args);
    if (n_args != detag(v2)) {        if(tr)Prog.println("             |       diff arity, returning false");
      return false;
    }                                 if(tr)Prog.println("             |       arity match, maybe unify...");
                                      if(tr)Prog.println("             |       entering " + n_args + "...1 loop");
    while (n_args > 0) {              if(tr)Prog.println("             |         loop: n_args="+n_args+"...");
      final int u1 = heap[w1+n_args]; if(tr)Prog.println("             |           u1=heap[" +(w1 + n_args)+ "]="+showCell(u1));
      if(tagOf(u1)==R)                if(tr)Prog.println("             |             " + showGoal("u1->",u1));
      final int u2 = heap[w2+n_args]; if(tr)Prog.println("             |           u2=heap[" +(w2 + n_args)+ "]="+showCell(u2));
      if(tagOf(u2)==R)                if(tr)Prog.println("             |             " + showGoal("u2->",u2));
      --n_args;
      if (u1 == u2) {                 if(tr)Prog.println("             |           u1 == u2, looping w/o unify stack push");
        continue;
      }                               if(tr)Prog.println("             |           u1 != u2, so push u2,u1 to--");
      unify_stack.push(u2);
      unify_stack.push(u1);           if(tr)Prog.println("             |           "+ showCS("unify_stack", 0, unify_stack));
    }                                 if(tr)Prog.println("             |       exiting " + n_args + "...1 loop, returning true");
    return true;
  }

  /**
   * Places a clause built by the Toks reader on the heap.
   */
  Clause putClause(final int[] cells, final int[] hgs, final int neck) {

    final int base = heap_size();
    // The following seems to depend on V==0 . . .
    assert V==0;
    final int b = tag(V, base);
    // ... because b is used later in '+' ops that would otherwise mangle tags.
    final int len = cells.length;
    pushCells(b, 0, len, cells);

// System.out.println ("---- putClause: hgs.length="+hgs.length+" -----");

    for (int i = 0; i < hgs.length; i++)
      hgs[i] = relocate(b, hgs[i]);

    final int[] index_vector = getIndexables(hgs[0]);

    return new Clause(len, hgs, base, neck, index_vector);
  }

  /**
   * Relocates a variable or reference by b.
   * Assumes var/ref codes V,U,R are 0,1,2.
   * Also assumes that b has cell structure --
   *  left-shifted by 3, tagged 0 (==V) [???]
   */
  private static int relocate(final int b, final int cell) {
    assert tagOf(b) == V;
    assert V == 0;
    return tagOf(cell) < 3 ? cell + b : cell;
  }

  /**
   * Pushes slice[from,to] at given base onto the heap.
   * b has cell structure, i.e, index, shifted left 3 bits, with tag 0 (==V)
   */
  private void pushCells(final int b, final int from, final int to, final int base) {
    assert tagOf(b) == V;
    assert V == 0;
    ensureSize(to - from);
    for (int i = from; i < to; i++)
      push_to_heap(relocate(b,heap[base+i]));
  }

  /**
   * Pushes slice[from,to] of cells array to heap.
   */
  private void pushCells(final int b, final int from, final int to, final int[] cells) {
    ensureSize(to - from);
    for (int i = from; i < to; i++) {
      push_to_heap(relocate(b, cells[i]));
    }
  }

  /**
   * Copies and relocates the head of clause C from heap to heap.
   */
  private int pushHead(final int b, final Clause C) {

    // Prog.println("+++ pushHead:" + " b = " + showCell(b)
    //        + " C.neck = " + C.neck
    //        + " C.base = " + C.base);
    // Prog.println(showHeap("+++ pushHead entered with heap"));

    assert tagOf(b) == V;
    assert V == 0;
    pushCells(b, 0, C.neck, C.base);
    final int head = C.skeleton[0];
    int reloc_head = relocate(b, head);
    // Prog.println("+++ pushHead: head=" + showCell(head) + " b = " + showCell(b) + " C.neck = " + C.neck + " C.base = " + C.base);
    // Prog.println(showHeap("+++ pushHead exiting with heap"));

    return reloc_head;
  }

  /**
   * Copies and relocates body of clause at offset from heap to heap
   * while also placing head as the first element of array 'goals' that,
   * when returned, contains references to the toplevel spine of the clause.
   */
  final private int[] pushBody(final int b, final int head, final Clause C) {
    assert tagOf(b) == V;
    assert V == 0;
    pushCells(b, C.neck, C.len, C.base);
    final int l = C.skeleton.length;
    final int[] goals = new int[l];
    goals[0] = head;
    // Main.println ("pushBody: goals[0]=" + showCell(head));
    for (int k = 1; k < l; k++) {
      final int cell = C.skeleton[k];
      goals[k] = relocate(b, cell);
      // Main.println("pushBody: goals[" + k + "]=" + showCell(goals[k]));
    }
    return goals;
  }

  /**
   * Makes, if needed, registers associated to top goal of a Spine.
   * These registers will be reused when matching with candidate clauses.
   * Note that index_vector contains dereferenced cells - this is done once for
   * each goal's toplevel subterms.
   */
  private void makeIndexArgs(final Spine G, final int goal) {

    // Prog.println("makeIndexArgs() entered...");
    if (null != G.index_vector)  // made only once
      return;
    // Prog.println("  makeIndexArgs() Found work to do ...");

    final int p = 1 + detag(goal);
    final int n = Math.min(MAXIND, detag(getRef(goal)));

    final int[] index_vector = new int[MAXIND];

    for (int i = 0; i < n; i++) {
      final int cell = deref(heap[p + i]);
      index_vector[i] = cell2index(cell);
    }

    G.index_vector = index_vector;

    if (Ip.is_empty())
      return;
    final int[] cs = Ip.matching_clauses(index_vector);
    G.unifiables = cs;
  }

  private int[] getIndexables(final int ref) {
    final int p = 1 + detag(ref);
    final int n = detag(getRef(ref));
    final int[] index_vector = new int[MAXIND];
    // Main.pp ("getIndexables: n=" + n + " ref="+ref);
    for (int i = 0; i < MAXIND && i < n; i++) {
      final int cell = deref(heap[p + i]);
      index_vector[i] = cell2index(cell);
      // Main.pp("  index_vector[" + i + "] for cell c=" + showCell(cell) + " is " + showCell(index_vector[i]));
    }
    // Main.pp("\n");
    return index_vector;
  }

  private int cell2index(final int cell) {
    int x = 0;
    final int t = tagOf(cell);
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
   * Tests if the head of a clause, not yet copied to the heap
   * for execution, could possibly match the current goal, an
   * abstraction of which has been placed in index_vector.
   * ("abstraction of which"???)
   * Supposedly, none of these "abstractions" can == -1
   */
  private boolean possible_match(final int[] index_vector, final Clause C0) {
    for (int i = 0; i < MAXIND; i++) {
      final int x = index_vector[i];
      final int y = C0.index_vector[i];
      if (0 == x || 0 == y) {
        continue;
      }
      if (x != y)
        return false;
    }
    ++n_matches;
    // Prog.println("*** possible match found");
    return true;
  }

  final private String showGoal(String buf, int goal) {
    int goal_loc = detag(goal)+1;
    int n_more = detag(cell2index(goal));
    while (n_more-- > 0) {
      int g = heap[goal_loc];

      if(tagOf(g) == R)
        buf += (showGoal("(", g) + ")");
      else
        buf += (/*"@" + goal_loc + "=" +*/ showCell(heap[goal_loc]) + " ");
      goal_loc++;
    }
    return buf;
  }

  /**
   * Video: "...to procedurally emulate our metainterpreter,
   * ... so what did we do in the metainterpreter?
   * We just took the first goal, and replaced it
   * with the body of a clause whose head was matching that first goal.
   * And that's called unfolding.
   * So it's a repeated process until there's nothing to unfold."
   * 
   * Transforms a spine containing references to choice point and
   * immutable list of goals into a new spine, by reducing the
   * first goal in the list with a clause that successfully
   * unifies with it - in which case it places the goals of the
   * clause at the top of the new list of goals, in reverse order.
   */
  final private Spine unfold(final Spine G) {
    trace l_ = l_off;

    if (G.goals == null) {                            l_.l("unfold: entered, G has no goals, returning null");
      return null;
    }
                                                      l_.l(showSpine("\nunfold: entered with G", G));
    final int trail_top = trail.getTop();
    final int saved_heap_top = get_heap_top();
    final int base_for_unfold = heap_top + 1;
    final int first_goal = IntList.head(G.goals);     l_.l("unfold: " + showGoal("", first_goal));

    if (clauses.length >= START_INDEX) {
      makeIndexArgs(G, first_goal);                   l_.l(showSpine("unfold: after makeIndexArgs, G", G));
    }

    final int last = G.unifiables.length;             l_.l("unfold: before loop: G.last_clause_tried="+G.last_clause_tried+" last="+last);
      // G.last_clause_tried: "index of the last clause [that]
            // the top goal of [this] Spine [G]
            // has tried to match so far " [HHG doc]
    for (int k = G.last_clause_tried; k < last; k++) {       l_.l("  unfold loop: G.unifiables[" + k + "]=" + G.unifiables[k]);
      final Clause clause_to_try = clauses[G.unifiables[k]]; l_.l("     clause_to_try:" + clause_to_try.othershow());

      if (clauses.length >= START_INDEX
        & !Ip.possible_match(G.index_vector,clause_to_try)){ l_.l("  unfold loop: no possible match, loop again");
        continue;
      }
      else ++n_matches;

      assert V == 0;
      final int reloc_offset = tag(V, base_for_unfold - clause_to_try.base);
      final int relocated_head = pushHead(reloc_offset, clause_to_try);
                                                            l_.l("  unfold loop:   " + showGoal("referring to ", relocated_head));
      unify_stack.clear(); // set up unification stack
      unify_stack.push(relocated_head);
      unify_stack.push(first_goal);

      if (unify(base_for_unfold)) {                         l_.l("unfold: unify succeeded...");
        G.last_clause_tried = k + 1;
        final int[] rebased_skel = pushBody(reloc_offset, relocated_head, clause_to_try);
        // if(rebased_skel.length == 0 || IntList.tail(G.goals)==null) return answer(trail_top); // BROKEN, why?
        Spine sp = new Spine(rebased_skel, base_for_unfold, IntList.tail(G.goals), trail_top, 0, clause_list);
                                                            l_.l(showSpine("unfold: new Spine to return",sp));
        return sp;
      }
                                                            l_.l("unfold:   unify failed, unwinding, loop again");
      unwindTrail(trail_top);
      set_heap_top(saved_heap_top);
    } // end for
                                                            l_.l("unfold: returning null at end");
    return null;
  }

  /**
   * Extracts a query - by convention of the form
   * goal(Vars):-body to be executed by the engine
   */
  Clause getQuery() {
    return clauses[clauses.length - 1];
  }

  /**
   * Returns the initial spine built from the query from which execution starts.
   *
   * (This might work for new Engines, if a query is copied starting from clauses.length?)
   */
  Spine init() {
    final int base = heap_size();
    final Clause G = getQuery();
    final Spine Q = new Spine(G.skeleton, base, IntList.empty, trail.getTop(), 0, clause_list);
    spines.push(Q);
    return Q;
  }

  /**
   * Returns an answer as a Spine while recording in it
   * the top of the trail to allow the caller to retrieve
   * more answers by forcing backtracking.
   */
  private Spine answer(final int trail_top) {
    return new Spine(spines.get(0).head, trail_top);
  }

  /**
   * Detects availability of alternative clauses for the
   * top goal of this spine.
   */
  private boolean hasClauses(final Spine S) {
    return S.last_clause_tried < S.unifiables.length;
  }

  /**
   * True when there are goals left to solve.
   */
  private boolean any_goals_left(final Spine S) {
    return !IntList.isEmpty(S.goals);
  }

  /**
   * Removes this spine from the spine stack and
   * resets trail and heap to where they were at its
   * creation time - while undoing variable binding
   * up to that point.
   */
  private void popSpine() {
    assert(spines != null);
    final Spine G = spines.pop();
    assert(G != null);
    unwindTrail(G.trail_top);
    set_heap_top(G.base - 1);
  }

  /**
   * Main interpreter loop: starts from a spine and works
   * though a stream of answers, returned to the caller one
   * at a time, until the spines stack is empty - when it
   * returns null.
   */
  final Spine yield() {
    while (!spines.isEmpty()) {
      final Spine G = spines.peek(); // "The active component of a Spine is the topmost goal
                                     // in [its]] immutable [goal_stack]" [HHG doc]
      if (!hasClauses(G)) {
        popSpine(); // no clauses left
        continue;
      }
      final Spine C = unfold(G);
      if (null == C) {
        popSpine(); // no matches - "When there are no more
                    // matching clauses for a given goal,
                    // the topmost Spine is popped off." [HHG doc]
        continue;
      }
      if (any_goals_left(C)) {
        spines.push(C);
        continue;
      }
      return C; // answer - "When no goals are left to solve,
                // a computed answer is yield[ed],
                // encapsulated in a Spine that can be used by the caller
                // to resume execution." [HHG doc]
    }
    return null; // "An empty Spine stack indicates the end of execution
                // signaled to the caller by returning null." [HHG doc]
  }

  /**
   * Retrieves an answer and ensures the engine can be resumed
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
  public Object POJO_ask() {
    // Prog.println(" POJO_ask(), spines.peek().last_clause_tried=" + spines.peek().last_clause_tried);
    query = yield();
    if (null == query)
      return null;
    // Main.println("ask: query.trail_top=" + query.trail_top);
    Spine tsp = answer(query.trail_top);
    // Main.println ("yield: answer(query.trail_top=" + query.trail_top + ")=" + tsp.show());
    final int result = tsp.head;
    // Main.println ("yield: answer(query.trail_top=" + query.trail_top + ").head=" + showCell(result));
    final Object R = exportTerm(result);
    unwindTrail(query.trail_top);
    return R;
  }

  /**
   * Initiator and consumer of the stream of answers
   * generated by this engine.
   */
  public void run() {
    long ctr = 0L;
    int MAX_OUTPUT_LINES = 5;

    // Prog.println(" &&&& run(): spines.peek().last_clause_tried=" + spines.peek().last_clause_tried);

    for (;; ctr++) {
      // Prog.println("about to call POJO_ask()...");
      final Object A = POJO_ask();
      if (null == A) {
        break;
      }
      if(ctr<MAX_OUTPUT_LINES)
        Prog.println("[" + ctr + "] " + "*** ANSWER=" + showTerm(A));
    }
    if(ctr>MAX_OUTPUT_LINES)
      Prog.println("...");
    Prog.println("TOTAL ANSWERS=" + ctr);
    Prog.println("Total matches=" + n_matches);
  }
}
