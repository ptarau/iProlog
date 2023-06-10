package org.iprolog;

import java.util.LinkedList;

import javax.lang.model.element.Element;
import javax.lang.model.util.ElementScanner6;

import java.util.Iterator;

// Prolog "term", with lexical conventions abstracted away.
// E.g., variables don't have to start with a capital letter
// or underscore, and can have embedded blanks. Maybe better
// to call it a Structure, which may align better with some
// established Prolog nomenclature.

// To be consistent with Paul Tarau's strategy of
// pretending Java classes are like C structs,
// I won't try to make this an interface
// for free-standing Java classes for constants,
// variables and compound terms.

// In C it could probably contain a tagged union. However,
// because the members built up through the API get
// turned into Tarau's compiled form, their size doesn't
// matter much. They'll be used once paged out of caches/RAM
// during any long logic-program execution; if their size
// somehow becomes a memory or speed bottleneck, the
// can be garbage-collected; as long as the symbol
// table info is retained, something similar enough
// to the original can be reconstructed from Tarau's
// compiled form.

// For tags, note the correpondence to Engine's V, R, and C.
// There may be a reason to merge the tags in Engine with the tags here.
// For now, tolerate smelly redundancy. I need to look at whether
// the equations (e.g., "_1=x") can be Compounds (like ""=(_1,x)"")
// internally; if so, this redundancy in coding types could be
// eliminated.

public class Term // implements Iterable<Term>
                {
/* 
    class TermIterator implements Iterator<Term> {
        Term start;
        Term cursor;
        TermIterator(Term obj) {      // constructor -- initialize cursor
            Main.println ("cursor init with obj = " + obj);
            start = cursor = obj;
        }
        public boolean hasNext() {      // Checks if the next element exists
            Main.println ("%%%%%%%%% OK are we ever checking next???? %%%%%%");
            return cursor.next != null;
        }
        public Term next() {            // moves the cursor/iterator to next element
            // should probably throw exception if !hasnext() 

            return cursor = cursor.next;
        }
    }
    public Iterator<Term> iterator() {
        Main.println ("<<<<< do we ever get a Term iterator??? >>>>>");
        return new Term.TermIterator(this);
    }
*/
    final private static int Variable = 1;   // correponds to Engine.U (unbound variable)
    final private static int Compound = 2;   // correponds to Engine.R (reference)
    final private static int Constant = 3;   // correponds to Engine.C (constant)
    final private static int TermList = 4;   // Not in Engine tags because lists expand
    final private static int TermPair = 5;   // cons/dotted-pair for list construction

    // "TermPair" can be used in the "natural assembly language"; "list" is the
    // corresponding reserved word, "lists" being a plurality of termpairs
    // arranged as, well, a list. Yeah, I found it cofusing at first myself.
    // If I ever change the "natural assembly language", I might go with "pair"
    // and maybe "just" when the second argument is nil.

    // hacky: if a variable presented through the API doesn't
    // start with upper case or underscore, prefix it with
    // something that does; likewise, if a constant starts
    // with upper case, prefix it with something lower case;
    // filter these prefixes back out at a later stage of
    // token processing.

    final public static String Var_prefix = "V__";
    final public static String Const_prefix = "c__";

    /*final*/ private int tag;      // mutable for in-place rewriting
                                    // which I admit may turn out to be
                                    // a bad idea
    
    private String S_;
    final public String v() { assert tag == Variable; assert S_ != null; return S_; };
    final String set_v(String v) { assert v != null; S_ = v; return S_; }

    // c/C_  -- constant or the functor of a compound
    final public String c() { assert tag == Compound || tag == Constant; return S_; }
    final String set_c(String c) { S_ = c; return S_; }

    // terms -- args of compound,
    //       or elements of "lists",
    //       or car of "list",
    //       or lhs+rhs of equation
    final private Term terms() { return Terms; }   // What's in "(...)" in a compound,
                                            //  or (hacky) the list [lhs,rhs] if equation
    private Term Terms;
    final private Term set_terms(Term Ts) {
        Terms = Ts;
        return Ts;
    }

    Term next = null;  // cdr

    public Term args() {
        assert tag == Compound;
        return terms();
    }

    public int n_after() {
        int n = 0;
        for (Term x = this.next; x != null; x = x.next) ++n;
        return n;
    }

    private void _T(int i) { // Main.println ("-----> in is_same_as " + i); 
        }
    // compares two terms
    // really need to sort out what null list means
    // is it empty list?
    // null pointer?
    // either?
    Boolean is_same_as (Term t) {
        if (t == null) { _T(1); return false; }
        if (tag != t.tag) { _T(2); return false; }
        if (t.S_ != null) {
            if (S_ == null) { _T(3); return false; }
            if (t.S_.compareTo(S_) != 0) { _T(4); return false; }
        }

        Term t1 = t.Terms;
        for (Term tt = Terms; tt != null; tt = tt.next) {
            if (t1 == null) { _T(5); return false; }
            if (!tt.is_same_as(t1)) { _T(6); return false; }
            // Main.println ("     &&& tt = <<<"+tt+">>>, t1=<<<"+t1+">>>");
            // if (t1.next != null) Main.println ("     &&& t1.next = <<<"+t1.next+">>>");
            t1 = t1.next;
        }

        /*return*/ { _T(7); // Main.println ("this is <<<"+this+">>> t is <<<"+t+">>>");
                                // Main.println (" t1==null is " + (t1 == null));
                                if (t1 != null) {
                                    // Main.println ("t1 is <<<"+t1+">>>")
                                    ;
                                }
                                    return t1 == null;
                    }
    }

    // Because of side effects, this could break all over the place
    // unless we make a copy
    //
    Term a_term (int tag, String thing, Term... ts) {

        if (ts != null) {
            Term cp_Ts[] = ts.clone();
            for (int i = 0; i < ts.length-1; ++i)
                cp_Ts[i].next = cp_Ts[i+1];
            cp_Ts[ts.length-1].next = null;
            return new Term (tag, thing, cp_Ts[0]);
        }
        return new Term (tag, thing, null);

    }

    Term (int tag, String thing, Term Ts) {

        this.tag = tag;

        switch (tag) {
            case Variable: set_terms(null); set_v(thing);   return;
            case Compound: set_terms(Ts); set_c(thing); return;
            case Constant: set_terms(null); set_c(thing); return;
            case TermList: set_terms(Ts); set_c(null);  return;
            case TermPair: set_terms(Ts); set_c(null);  return;
        }

// I should really raise some exception here
        set_v(null);
        set_c(null);
        set_terms(null);
    }

    public Boolean is_a_variable() {  return tag == Variable;  }
    public Boolean is_a_compound() {  return tag == Compound;  }
    public Boolean is_a_constant() {  return tag == Constant;  }
    public Boolean is_a_termlist() {  return tag == TermList;}
    public Boolean is_an_equation(){  return tag == Compound && c() == "=";  }
    public Boolean is_a_termpair() {  return tag == TermPair; }

    public static String remove_any_Var_prefix(String s) {
        if (s.startsWith(Var_prefix))
            s = s.substring(Var_prefix.length());

        return s;
    }
    public static String remove_any_Const_prefix(String s) {
        if (s.startsWith(Const_prefix))
            s = s.substring(Const_prefix.length());

        return s;
    }
    
    // Embarrassingly hacky Prolog fakeout:
    //  Var_prefix is prepended when a variable ID is lower case.
    // Maybe in getword() this could be detected and removed,
    // but the v tag kept.
    // Similar hacky treatment for constants starting upper case.
    public static Term variable(String v) {
        if (Character.isLowerCase(v.charAt(0))) v = Var_prefix + v;
        return new Term (Variable, v, null);
    }

    public static Term constant(String c) {
        if (Character.isUpperCase(c.charAt(0))) c = Const_prefix + c;
        return new Term (Constant, c, null);
    }
    public static Term compound(String f) {
        return new Term (Compound, f, null);
    }
    public static Term compound(String f, Term terms) {
        return new Term (Compound, f, terms);
    }
    public static Term equation(Term lhs, Term rhs) {
        assert lhs.is_a_variable(); // for now
        assert !rhs.is_an_equation();
        Term l = lhs.clone();
        l.next = rhs.clone();
        // Main.println ("in equation (lhs=<<<"+lhs+">>>, rhs=<<<"+rhs+">>>)");
        return new Term(Compound, "=", l);
    }
    public static Term termlist(Term... ts) {
        Term r = new Term (TermList, null, null);
        assert r.is_a_termlist();
        if (ts == null) return r;
        if (ts.length == 0) return r;

        Term cp_Ts[] = ts.clone();

        for (int i = 0; i < ts.length-1; ++i)
            cp_Ts[i].next = cp_Ts[i+1];

        cp_Ts[ts.length-1].next = null;

        r.Terms = cp_Ts[0];

        return r;
    }

    public Term lhs() {
        assert this.is_an_equation();
        return terms();
    }

    public Term rhs() {
        assert this.is_an_equation();
        return terms().next;
    }

    // The following differences in lexicalization
    // may be better managed with a class for
    // lexicals + subclassing.

    public static boolean in_Prolog_mode = set_Prolog();
    
    protected static String and_op; 
    protected static String args_start;
    protected static String arg_sep;
    protected static String args_end;
    protected static String clause_end;
    protected static String if_sym;
    protected static String holds_op;
    protected static String list_start;
    protected static String list_elt_sep;
    protected static String list_end;
    protected static String cons;
    
    // See Toks; there, I squeeze out whitespace
    // from these. Used for pretty-printing the
    // Tarau "assembly language" and (indirectly)
    // in the toSentences lexeme tagger.
    public static void set_TarauLog() {

        and_op = " and ";
        args_start = " ";
        arg_sep = " ";
        args_end = " ";
        clause_end = ".";
        if_sym = "\nif ";
        holds_op = " holds ";
        list_start = "lists ";
        list_elt_sep = " ";
        list_end = " ";
        cons = "list ";

        in_Prolog_mode = false;
    }

    public static boolean set_Prolog() {

        and_op = ",";
        args_start = "(";
        arg_sep = ",";
        args_end = ")";
        clause_end = ".";
        if_sym = ":-";
        holds_op = "=";
        list_start = "[";
        list_elt_sep = ",";
        list_end = "]";
        cons = "(.)";

        return in_Prolog_mode = true;
    }

    public String as_fact() { return this + clause_end; }
    public String as_head() { return this + if_sym; }
    public String as_expr(String ended_with) {
        return "  " + this + ended_with;
    }

    // Problem with conflating equations/expressions and terms:
    // - for compounds it's still comma-separated;
    // - between expressions, it's Tarau's "and";
    // - so allow for either.
    private String terms_to_str(String sep) {

        String delim = "";
        String s = "";

        if (terms() != null) {
            for (Term t = Terms; t != null; t = t.next) {
                s = s + delim;
                s = s + t;
                delim = sep;
            }
        }
        return s;
    }

    private boolean is_lists() {
        return tag == Compound && this.c() == "lists";
    }

    public String toString() {
        String r = "<unassigned>";

        switch (tag) {
            case Variable: r = v(); break;
            case Constant: r = c(); break;
            case Compound: if (c() != "=") {                                 
                                 r =      c()
                                        + args_start
                                        + terms_to_str(arg_sep)
                                        + args_end;
                            } else {
                                if (rhs().is_lists()) // ?????? WHY ????????                                                                  
                                    r = lhs().toString() + " " + rhs().toString();
                                else 
                                    r = lhs() + holds_op + rhs();
                            }
                            break;
            case TermList:  // if (terms == null)
                            //    Main.println ("  Term.toString:      Term.toString: list terms = null !!!!");
                            
                            r =  list_start + terms_to_str(list_elt_sep) + list_end;
                            break;
            case TermPair:  r = cons + terms_to_str(list_elt_sep);
                            break;
        }
        return r;
    }

    public Term takes_this(Term t) {
        assert tag == Compound;
        assert t != null;
        /*
        Main.println (" is t really " + t + "?????????????");
        Main.println ("t.S_ = " + t.S_);
        Main.println ("t.tag = " + t.tag);
        Main.println ("Term "+this+" takes_this("+t+"):");
        */
        this.Terms = add_elt(terms(), t.clone());
        // Main.println ("In takes_this, this term is now "+this);
        assert Terms != null;
        // Main.println ("Terms = " + Terms);
        // for (Term x = Terms; x != null; x = x.next) { Main.println ("    .... " + x); }
        return this;
    }

    private static int    gensym_i = 0;
    public  static void   reset_gensym() { gensym_i = 0; }
    public  static String gensym() { return "_" + gensym_i++; }

    private Boolean is_simple() {
        return tag == Variable || tag == Constant;
    }

    public Boolean is_flat() {
        if (this.is_simple()) return true;

        // With lhs and rhs coded in Term as two-element list
        // this step is not even really necessary; you've
        // got either a compound, or a list:
        // if (c == "=")
        //    return lhs().is_flat() && rhs().is_flat();

        // depends on representing lhs+rhs as terms list in eqns:
        for (Term t = terms(); t != null; t = t.next)
            if (!t.is_simple())
                return false;

        return true;
    }

    /*
 * https://stackoverflow.com/questions/64814365/flattened-form-in-wam
 * ... build the arguments before you build the outer terms.
 * For example, you must build a(K, C) before you can build h(..., a(K, C), ...),
 * and you must build that before you can build p(..., h(..., a(K, C), ...), ...).
 * Here is one legal order for p(Z,h(Y,a(K,C),K),f(C)):

          _4 = a(K, C)
     _2 = h(Y, _4, K)
     _3 = f(C)
_1 = p(Z, _2, _3)
 */

// A flatten that returns residue "squeezed out".
// this transforms clause structures.
// it's applied first to the head, then to the body
// Making it DFS in what it flattens.
// Trying to make it work in-place -- i.e., it
// rewrites the structure given to it.
// The original should be recoverable (P. Tarau)
// Not sure any of this is right.
// Basically, loop until residue gone.

// flatten a term, with (mutable?) todo list passed in.
// Has shared-structure result for already-flattened terms
// It can also munge what's originally passed to it, so
// watch out for linked list side effects on terms list,
// at point of call.

// think of holds/'=' and list/'.' as functors of compounds
// just always two arguments a1,a2, either or both of which
// could be flat themselves.

/*
 * a(b(c)) -> a(_0), todo = [_0 = b(c)]
 * But if treated as compound ['='(_0,b(c))] in further iteration,
 * recursion would be infinite.
 * So equations really need to be a special case
 */

 /* flatten this */

    Term add_elt (Term x, Term elt) {
        // Main.println ("   in add_elt("+x+", "+elt+"):");

        if (x == null) return elt;
        assert elt != null;
        assert elt.next != elt;
        assert x != elt;
        assert (x.next != x);

        int limit = 6;
        for (Term i = x; i != null; i = i.next) {
            // Main.println ("   i = " + i);
            assert i.next != i;
            if (i.next == null) {
                i.next = elt;
                break;
            }
            assert (--limit > 0);
        }
        return x;
    }
    Term add_all (Term x, Term Ts) {
        if (Ts == null) return x;
        assert Ts.next != Ts;
        // Main.println ("add_all: x = <<<" + x + ">>> Ts (start) is <<<" + Ts + ">>>");
        String xS;
        if (x != null)
             xS = x.toString();
        else xS = "<<<null>>>";
        String TsStartS = Ts.toString();
        assert xS.compareTo(TsStartS) != 0;
        return add_elt (x, Ts);
    }

    public Term clone() {
      return new Term(this.tag, this.S_, this.Terms);
    }

    private void check_it(Term r) {
        for (Term x = r; x != null; x = x.next) {
            if (x.is_an_equation()) {
                assert !x.rhs().is_an_equation();
            }
        }
    }

    public Term flatten() {
        // Main.println ("\n\nflatten1: this is " + this);

        if (this.is_simple()) {
            // Main.println ("Returning a clone of it");
            return this.clone();
        }
 
        Term r = null;
        Term todo = null;
        Term new_terms = null;

        if (this.is_an_equation()) {
            assert lhs().is_a_variable();
            assert !rhs().is_an_equation();
            new_terms = lhs().clone();
            if (rhs().is_simple()) {
                new_terms = add_elt (new_terms, rhs().clone());
            } else {
                Term rhs_fl = rhs().flatten();
                Term rhs_next = rhs_fl.next;
                new_terms = add_elt (new_terms, rhs_fl.clone());
                // Main.println ("    adding rhs_next <<<"+rhs_next+">>> to todo");
                todo = add_all (todo, rhs_next);
            }   
        }
        else {
            // Main.println (" compound:");
            if (terms() != null)
                for (Term t = terms(); t != null; t = t.next) {
                    if (t.is_simple()) {
                        // Main.println ("  simple arg t is <<<"+t+">>>:");
                        new_terms = add_elt (new_terms, t.clone());
                    } else {
                        // Main.println ("  complex arg t is <<<"+t+">>>:");
                        Term v = variable(Term.gensym());
                        new_terms = add_elt (new_terms, v);
                        Term e = equation (v.clone(), t.clone());
                        // Main.println ("    adding <<<"+e+">>> to todo");
                        todo = add_elt (todo, e);
                    }
                }
        }
        
        set_terms(new_terms);
        
        if (todo != null) {
            for (Term t = todo; t != null; t = t.next) {
                Term tf = t.flatten();
                r = add_all (r, tf);
            }
        }
        for (Term x = r; x != null; x = x.next) {
            if (x.is_an_equation()) {
                assert !x.rhs().is_an_equation();
            }
        }


        Term new_head = this.clone();
        new_head = add_all (new_head, r);
        // Main.println ("flatten: new_head is <<<"+new_head+">>>");
        // Main.println ("returning = ");
        // for (Term x = r; r != null; r = r.next) Main.println ("   " + x);
        // Main.println ("return");

        return new_head;
    }

}



