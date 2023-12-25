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

class Engine {
	int n_matches = 0;

  Spine query;

  final static int MAXIND = 3; // number of index args
  final static int START_INDEX = 2;  // if # of clauses < START_INDEX, turn off indexing

  /* Trimmed-down clauses ready to be quickly relocated to the heap: */
  /* (Not clear what "trimmed-down" means.) */
  final Clause[] clauses;

  final int[] clause_list; // if no indexing, [0..clauses.length-1]

  /* Symbol table - made of map (syms) + reverse map from ints to syms (slist) */
  final LinkedHashMap<String, Integer> syms; // syms->ints
  final private ArrayList<String> slist;     // resizeable ints->syms

    /** Runtime areas: **/

   /* heap - contains code for 'and' clauses and their copies created during execution
   */
  private int heap[];
  private int heap_top;
  static int MINSIZE = 1 << 15; // power of 2

  /* trail - undo list for variable bindings; facilitates retrying failed goals
   *   with alternative matching clauses
   */
  final private IntStack trail;
  
  /* ustack - unification stack; helps to handle term unification non-recursively
   */
  final private IntStack ustack;
  
  /* spines - stack of abstractions of clauses and goals;
   *    both a choice-point stack and goal stack
   */
  final private ObStack<Spine> spines = new ObStack<Spine>();

   /* imaps - contains indexes for up to MAXIND>0 arg positions (0 for pred symbol itself)
   */
  final IMap<Integer>[] imaps;

    /* vmaps - contains clause numbers for which vars occur in indexed arg positions
   */
  final IntMap[] vmaps;

  /**
   * Builds a new engine from a natural-language-style assembler.nl file
   */
  Engine(final String s, final boolean fromFile) {
    syms = new LinkedHashMap<String, Integer>();
    slist = new ArrayList<String>();

    makeHeap();

    trail = new IntStack();
    ustack = new IntStack();

    // Main.println ("Calling dload_from_x");
    clauses = dload_from_x(s, fromFile); // load "natural language" source

    clause_list = toNums(clauses); // initially an array  [0..clauses.length-1]
      // Used in indexing (somehow)

    query = init();  /* initial spine built from query from which execution starts */

    vmaps = vcreate(MAXIND); // array of MAXIND IntMaps

    imaps = index(clauses, vmaps);
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
  final private static int maybe_invert(final int w) { return -w; }
  final private static int tag(final int t, final int w) {
    assert t <= BAD;
    assert t >= 0;
    return maybe_invert((w << n_tag_bits) + t);
  }

  /**
   * Removes tag after (maybe) flipping sign.
   */
  final private static int detag(final int w) {
    return maybe_invert(w) >> n_tag_bits;
  }

  /**
   * Extracts the tag of a cell.
   */
  final private static int tagOf(final int w) {
    return maybe_invert(w) & TAG_MASK;
  }

  /**
   * Places an identifier in the symbol table.
   */
  final private int addSym(final String sym) {
    Integer I = syms.get(sym);
    if (null == I) {
      final int i = syms.size();
      I = new Integer(i);
      syms.put(sym, I);
      slist.add(sym);
    }
    return I.intValue();
  }

  /**
   * Returns the symbol associated to an integer index
   * in the symbol table.
   */
  final private String getSym(final int w) {
    if (w < 0 || w >= slist.size())
      return "BADSYMREF=" + w;
    return slist.get(w);
  }

  private final void makeHeap() {
    makeHeap(MINSIZE);
  }

  private final void makeHeap(final int size) {
    heap = new int[size];
    clear_heap();
  }

  private final int get_heap_top() {
    return heap_top;
  }

  private final int set_heap_top(final int top) {
    return this.heap_top = top;
  }

  private final void clear_heap() {
    heap_top = -1;
  }

  /**
   * Pushes an element - top is incremented first, then the
   * element is assigned. This means top points to the last assigned
   * element - which can be returned with peek().
   */
  private final void push_to_heap(final int i) {
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
  private final void expand() {
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

  private final static ArrayList<String[]>
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
    final ArrayList<String[]> an_expansion = new ArrayList<String[]>();
    final String V = first_word.substring(2);

// Main.println ("expand_lists_to_holds: l = " + l);
// Main.println ("expand_lists_to_holds: V = " + V);

String s = "[";
String sep = "";
for (int i = 1; i < l; ++i) { s += (sep + words.get(i)); sep = ","; }
s += "]";
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
  private final static ArrayList<String[]>
  expand_lists_stmts(final ArrayList<ArrayList<String>> Wss) {

    // Main.println("\n\nexpand_lists_stmts: entered....");
    // Main.println("  Wss = " + Wss);

    final ArrayList<String[]> Results = new ArrayList<String[]>();
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

  final static IntStack put_ref(String arg,
                                LinkedHashMap<String, IntStack> refs,
                                int clause_pos) {   // guessing it means clause position
    IntStack Is = refs.get(arg);
    if (null == Is) {
      Is = new IntStack();
      refs.put(arg, Is);
    }
    Is.push (clause_pos);
    return Is;
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

    final ArrayList<Clause> compiled_clauses = new ArrayList<Clause>();

    for (final ArrayList<ArrayList<String>> clause_asm : clause_asm_list) {

      final LinkedHashMap<String, IntStack> refs = new LinkedHashMap<String, IntStack>();
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

            Prog.println("%%%% l = ws.length = " + l + " for...");

        goals.push(tag(R, k++));
        assert goals.size() > 0;
        cells.push(tag(A, l));

        for (String w : ws) { // gen code for 'element' (= head/body subterm)

          Prog.println ("   ..." + w);

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
                      // "just in case we miss this:" ??
                      //  P. Tarau comment
                      Prog.println("    &&&& v case, k = " + k);
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

    final Iterator<IntStack> K = refs.values().iterator();

    while (K.hasNext()) {
      final IntStack Is = K.next();

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

    final int[] hgs = goals.toArray();

    assert hgs.length > 0;

          // Prog.println("cells before entering putClause()");
          // for (int i = 0; i < cells.size(); ++i) Prog.println(showCell(cells.get(i)));

    final Clause C = putClause(cells.toArray(), hgs, neck);

    Clauses.add(C);

  }

  private static final int[]
  toNums(final Clause[] clauses) {
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
  private final int encode(final int t, final String s) {
    int w;
    try {
      w = Integer.parseInt(s);
    } catch (final Exception e) {
      if (C == t) {
        w = addSym(s);
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
  final private static boolean isVAR(final int x) {
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
  final private void setRef(final int w, final int r) {
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

      int x = detag(href);

      // Prog.println("   href=" + showCell(href) + " detag(href) = " + x);

      setRef(href, href);
    }
  }

  /**
   * Scans reference chains starting from a variable
   * until it points to an unbound root variable or some
   * non-variable cell.
   */
  final private int deref(int x) {
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
      // Main.pp("trail[" + i + "]=" + showCell(t) + ":" + showTerm(t));
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

    final int t = tagOf(x);
    final int w = detag(x);

    Object res = null;
    switch (t) {
      case C: // symbol
        res = getSym(w);
      break;
      case N: // integer
        res = new Integer(w);
      break;
      case V: // variable
        //case U:
        res = "V" + w;
      break;
      case R: { // reference
        final int a = heap[w];
        assert A == tagOf(a);
        final int n = detag(a);
        final Object[] arr = new Object[n];
        final int k = w + 1;   // offset to embedded array
        for (int i = 0; i < n; i++) {
          final int j = k + i;
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
    String s = null;
    switch (t) {
      case V:        s = "v:" + val;          break;
      case U:        s = "u:" + val;          break;
      case N:        s = "n:" + val;          break;
      case C:        s = "c:" + getSym(val);  break;
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
   * to be overridden as a printer for spines
   */
  void ppSpines() {
    // override
  }

  /**
   * Unification algorithm for cells X1 and X2 on ustack that also takes care
   * to trail bindings below a given heap address "base".
   */
  final private boolean unify(final int base) {
    // Prog.println ("  Entering unify(), unify_stack.getTop()=" + ustack.getTop());
    while (!ustack.isEmpty()) {
      final int x1 = deref(ustack.pop());
      final int x2 = deref(ustack.pop());
      // Prog.println("      unify loop: x1=" + showCell(x1) + " x2=" + showCell(x2));
      // Prog.println("      unify loop: unify_stack.getTop() =" + ustack.getTop());
      if (x1 != x2) {
        final int t1 = tagOf(x1);
        final int t2 = tagOf(x2);
        final int w1 = detag(x1);
        final int w2 = detag(x2);

        if (isVAR(x1)) { /* unb. var. v1 */
          if (isVAR(x2) && w2 > w1) { /* unb. var. v2 */
            heap[w2] = x1;
            if (w2 <= base) {
              trail.push(x2);
              // Prog.println("x1 & x2 vars, w2>w1, x2 pushed: " + showCell(x2));
            }
          } else { // x2 nonvar or older
            heap[w1] = x2;
            if (w1 <= base) {
              trail.push(x1);
              // Prog.println("x1 var, x2 nonvar or older, x1 pushed: " + showCell(x1));
            }
          }
        } else if (isVAR(x2)) { /* x1 is NONVAR */
          heap[w2] = x1;
          if (w2 <= base) {
            trail.push(x2);
            // Prog.println("x1 nonvar, x2 older, x2 pushed: " + showCell(x2));
          }
        } else if (R == t1 && R == t2) { // both should be R
          if (!unify_args(w1, w2))
            return false;
        } else
          return false;
        // Prog.println("      unify loop: unify_stack.getTop() NOW =" + ustack.getTop());
      }
    }
    return true;
  }

  String showCS(String prompt, IntStack cs) {
    String s = prompt + ":";
    for (int i = 0; i < cs.size(); ++i) {
      s += " ";
      s += showCell(cs.get(i));
    }
    return s;
  }

  String showHeap(String prompt) {
    String s = prompt + ":";
    for (int i = 0; i < heap_size(); ++i) {
      s += " ";
      s += showCell(heap[i]);
    }
    return s;
  }

  final private boolean unify_args(final int w1, final int w2) {
    // Prog.println("                Entered unify_args(" + w1 + "," + w2 + ")");
    final int v1 = heap[w1];
    final int v2 = heap[w2];
    // both should be A
    assert tagOf(v1) == A;
    assert tagOf(v2) == A;
    final int n1 = detag(v1);
    final int n2 = detag(v2);
    // Prog.println("               n1=" + n1 + " n2=" + n2);
    if (n1 != n2)
      return false;
    // Prog.println("               continuing");
    final int b1 = 1 + w1;
    final int b2 = 1 + w2;
    // Prog.println("                n1-1=" + (n1-1));
    for (int i = n1 - 1; i >= 0; i--) {
      final int i1 = b1 + i;
      final int i2 = b2 + i;
      // Prog.println("                       i1=" + i1 + " i2=" + i2);
      final int u1 = heap[i1];
      final int u2 = heap[i2];
      if (u1 == u2) {
        continue;
      }
      ustack.push(u2);
      ustack.push(u1);
      // Prog.println("                       "+ showCS("unify_stack", ustack));
      // Prog.println("                       "+ showHeap("heap"));
    }
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

    final int[] xs = getIndexables(hgs[0]);

//  System.out.println ("---- putClause: returning -----");
    // Prog.println ("---- base being set to " + base);
    return new Clause(len, hgs, base, neck, xs);
  }

  /**
   * Relocates a variable or reference by b.
   * Assumes var/ref codes V,U,R are 0,1,2.
   * Also assumes that b has cell structure --
   *  left-shifted by 3, tagged 0 (==V) [???]
   */
  final private static int relocate(final int b, final int cell) {
    assert tagOf(b) == V;
    assert V == 0;
    return tagOf(cell) < 3 ? cell + b : cell;
  }

  /**
   * Pushes slice[from,to] at given base onto the heap.
   * b has cell structure, i.e, index, shifted left 3 bits, with tag 0 (==V)
   */
  final private void pushCells(final int b, final int from, final int to, final int base) {
  if (false) {
    // Prog.println("");
    // Prog.println("??? pushCells(" + showCell(b) + " from=" + from + " to=" + to
    //        + " with base=" + base);
    }
    assert tagOf(b) == V;
    assert V == 0;
    ensureSize(to - from);
    for (int i = from; i < to; i++) {
      int c = heap[base+i];
      int cr = relocate(b,c);
    // if (false) {
    //   Prog.println("???    heap.get(" + (base + i) + ")=" + showCell(c)
    //          + " relocated by " + showCell(b) + " =" + showCell(cr));
    // }
      push_to_heap(cr);
    }
  }

  /**
   * Pushes slice[from,to] of cells array to heap.
   */
  final private void pushCells(final int b, final int from, final int to, final int[] cells) {
    ensureSize(to - from);
    for (int i = from; i < to; i++) {
      push_to_heap(relocate(b, cells[i]));
    }
  }

  /**
   * Copies and relocates the head of clause C from heap to heap.
   */
  final private int pushHead(final int b, final Clause C) {

    // Prog.println("+++ pushHead:" + " b = " + showCell(b)
    //        + " C.neck = " + C.neck
    //        + " C.base = " + C.base);
    // Prog.println(showHeap("+++ pushHead entered with heap"));

    assert tagOf(b) == V;
    assert V == 0;
    pushCells(b, 0, C.neck, C.base);
    final int head = C.hgs[0];
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
    final int l = C.hgs.length;
    final int[] goals = new int[l];
    goals[0] = head;
    for (int k = 1; k < l; k++) {
      final int cell = C.hgs[k];
      goals[k] = relocate(b, cell);
    }
    return goals;
  }

  /**
   * Makes, if needed, registers associated to top goal of a Spine.
   * These registers will be reused when matching with candidate clauses.
   * Note that xs contains dereferenced cells - this is done once for
   * each goal's toplevel subterms.
   */
  final private void makeIndexArgs(final Spine G, final int goal) {

    // Prog.println("makeIndexArgs() entered...");
    if (null != G.xs)  // made only once
      return;
    // Prog.println("  makeIndexArgs() Found work to do ...");

    final int p = 1 + detag(goal);
    final int n = Math.min(MAXIND, detag(getRef(goal)));

    final int[] xs = new int[MAXIND];

    for (int i = 0; i < n; i++) {
      final int cell = deref(heap[p + i]);
      xs[i] = cell2index(cell);
      // Prog.println("    G->index_vector[" + i + "]=" + xs[i]);
    }

    G.xs = xs;

    if (null == imaps)
      return;
    final int[] cs = IMap.get(imaps, vmaps, xs);
    G.cs = cs;
  }

  final private int[] getIndexables(final int ref) {
    final int p = 1 + detag(ref);
    final int n = detag(getRef(ref));
    final int[] xs = new int[MAXIND];
    for (int i = 0; i < MAXIND && i < n; i++) {
      final int cell = deref(heap[p + i]);
      xs[i] = cell2index(cell);
    }
    return xs;
  }

  final private int cell2index(final int cell) {
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
   * abstraction of which has been placed in xs.
   * ("abstraction of which"???)
   * Supposedly, none of these "abstractions" can == -1
   */
  private final boolean possible_match(final int[] xs, final Clause C0) {
    for (int i = 0; i < MAXIND; i++) {
      final int x = xs[i];
      final int y = C0.xs[i];
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

    final int trail_top = trail.getTop();
    // Prog.println("unfold: trail_top=" + trail_top);
    // Prog.println("unfold: get_heap_top()=" + get_heap_top());
    final int saved_heap_top = get_heap_top();
    final int base = heap_top + 1;
    ///////////////////////////////////////////////
    if (G.goals == null) {
      return null;
    }
    ///////////////////////////////////////////////
    final int goal = IntList.head(G.goals);

    makeIndexArgs(G, goal);

    final int last = G.cs.length;
    // G.k: "index of the last clause [that]
          // the top goal of [this] Spine [G]
          // has tried to match so far " [HHG doc]

    // Prog.println("before unfold loop: G->kount=" + G.k);
    // for (int i = 0; i<G.cs.length; ++i)
    //   Prog.println ("G.cs[" + i + "]=" + G.cs[i]);
    // for (int k = G.k; k < last; k++) {
    //   Prog.println("clauses[" +G.cs[k] + "].base=" + clauses[G.cs[k]].base);
    // }

    for (int k = G.k; k < last; k++) {
      // Prog.println("unfold loop: k = " + k);
      final Clause C0 = clauses[G.cs[k]];

      // Prog.println ("C0.base=" + C0.base);
      // Prog.println ("     " + showHeap("heab before pushHead"));

      if (!possible_match(G.xs, C0))
          continue;

      // Prog.println("??????? possible match? ???????");
      final int base0 = base - C0.base;
      final int b = tag(V, base0);
      assert V == 0;
      final int head = pushHead(b, C0);

      ustack.clear(); // set up unification stack

      ustack.push(head);
      ustack.push(goal);
      // Prog.println("pushed to unify_stack: head=" + showCell(head) + " goal=" + showCell(goal));
      // Prog.println("                       "+ showCS("unify_stack", ustack));
      // Prog.println("                       "+ showHeap("heap"));
      // Prog.println("                       base=" + base);

      if (!unify(base)) {
        // Prog.println ("!!!!!! unify failed !!!!! trail_top=" + trail_top + " saved_heap_top=" + saved_heap_top);
        unwindTrail(trail_top);
        set_heap_top(saved_heap_top);
        // Prog.println ("!!!!!! unify continues with trail.getTop()=" + trail.getTop() + " heap_top=" + get_heap_top());
        continue;
      }
      final int[] goals = pushBody(b, head, C0);

      // Prog.println("$$$$$$$$$$$ goals after pushBody:");
      // for (int i = 0; i < goals.length; ++i) { Prog.println(" " + showCell(goals[i]));}

      final IntList new_goals = IntList.tail(IntList.concat(goals, IntList.tail(G.goals)));
      G.k = k + 1;

      // Prog.println("\n     *** spine.base = " + G.base + " UPDATED spine.kount=" + G.k + "\n");

      if (!IntList.isEmpty(new_goals)) {
        // Prog.println("\n     *** new_goals NOT empty --new Spine with initial kount=0\n");
        return new Spine(goals, base, IntList.tail(G.goals), trail_top, 0, clause_list);
      } else {
        // Prog.println("\n     *** new_goals empty--new Spine being generated by answer()\n");
        return answer(trail_top);
      }
    } // end for
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
   */
  Spine init() {
    final int base = heap_size();
    Prog.println("init(): base=" + base);

    final Clause G = getQuery();
    // Prog.println("trail.getTop()=" + trail.getTop());
    final Spine Q = new Spine(G.hgs, base, IntList.empty, trail.getTop(), 0, clause_list);
    // Prog.println("Q.k=" + Q.k);
    spines.push(Q);
    return Q;
  }

  /**
   * Returns an answer as a Spine while recording in it
   * the top of the trail to allow the caller to retrieve
   * more answers by forcing backtracking.
   */
  final private Spine answer(final int trail_top) {
    return new Spine(spines.get(0).head, trail_top);
  }

  /**
   * Detects availability of alternative clauses for the
   * top goal of this spine.
   */
  final private boolean hasClauses(final Spine S) {
    // Prog.println("hasClauses: S.base= "+S.base+" S.k=" + S.k + " S.cs.length=" + S.cs.length);
    return S.k < S.cs.length;
  }

  /**
   * True when there are goals left to solve.
   */
  final private boolean any_goals_left(final Spine S) {
    return !IntList.isEmpty(S.goals);
  }

  /**
   * Removes this spine from the spine stack and
   * resets trail and heap to where they were at its
   * creation time - while undoing variable binding
   * up to that point.
   */
  final private void popSpine() {
    final Spine G = spines.pop();
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
	  // Prog.println("Entering yield()");
    while (!spines.isEmpty()) {
      final Spine G = spines.peek(); // "The active component of a Spine is the topmost goal
                                     // in [its]] immutable [goal_stack]" [HHG doc]
      // Prog.println ("  yield: G.k=" + G.k);
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
      // Prog.println ("  yield: C.k=" + C.k);
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
  Object POJO_ask() {
    // Prog.println(" POJO_ask(), spines.peek().k=" + spines.peek().k);
    query = yield();
    if (null == query)
      return null;
    final int result = answer(query.trail_top).head;
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

    // Prog.println(" &&&& run(): spines.peek().k=" + spines.peek().k);

    for (;; ctr++) {
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

  // Indexing extensions - ony active if START_INDEX clauses or more.

  public static IntMap[] vcreate(final int l) {
    final IntMap[] vss = new IntMap[l];
    for (int i = 0; i < l; i++) {
      vss[i] = new IntMap();
    }
    return vss;
  }

  final static void put(final IMap<Integer>[] imaps, final IntMap[] vss, final int[] keys, final int val) {
    for (int i = 0; i < imaps.length; i++) {
      final int key = keys[i];
      if (key != 0) {
        IMap.put(imaps, i, key, val);
      } else {
        vss[i].add(val);
      }
    }
  }

  final IMap<Integer>[] index(final Clause[] clauses, final IntMap[] vmaps) {

	  // Prog.println ("Entered index() with START_INDEX=" + START_INDEX);
	  // Prog.println ("  clauses.length=" + clauses.length);
    if (clauses.length < START_INDEX)
      return null;

    final IMap<Integer>[] imaps = IMap.create(vmaps.length);
    for (int i = 0; i < clauses.length; i++) {
      final Clause c = clauses[i];

      // Prog.println ("C["+i+"]="+c.toString());
      put(imaps, vmaps, c.xs, i + 1); // $$$ UGLY INC

    }
    Main.pp("INDEX");
    Main.pp(IMap.show(imaps));
    Main.pp(Arrays.toString(vmaps));
    Main.pp("");
    return imaps;
  }
}
