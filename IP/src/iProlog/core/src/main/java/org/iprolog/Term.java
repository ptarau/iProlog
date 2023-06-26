package org.iprolog;

import java.util.LinkedList;

import javax.lang.model.element.Element;
import javax.lang.model.util.ElementScanner6;

import java.util.Iterator;

// Prolog "term", with lexical conventions abstracted away.
// E.g., variables don't have to start with a capital letter
// or underscore, and they can have embedded blanks. Maybe better
// to call it a Structure, because it may align better with some
// established Prolog nomenclature.

// To be consistent with Paul Tarau's strategy of
// pretending Java classes are like C structs,
// I won't try to make this an interface or superclass
// for free-standing Java classes for constants,
// variables and compound terms.

// In C it could probably contain a tagged union. However,
// because the members built up through the API get
// turned into Tarau's compiled form, their size doesn't
// matter much. They'll be used once, GCed
// during any long logic-program execution.
// As long as the symbol table info is retained,
// something similar enough to the original
// can be reconstructed from Tarau's
// compiled form.

// For tags, note the correpondence to Engine's V, R, and C.
// There may be a reason to merge the tags in Engine with the tags here.
// For now, tolerate the smelly redundancy. I need to look at whether
// the equations (e.g., "_1=x") can be Compounds (like ""=(_1,x)"")
// internally; if so, this redundancy in coding types could be
// eliminated.


public class Term {

    final private static int Variable = 1;   // correponds to Engine.U (unbound variable)
    final private static int Compound = 2;   // correponds to Engine.R (reference)
    final private static int Constant = 3;   // correponds to Engine.C (constant)
    final private static int TermList = 4;   // Not in Engine tags because lists expand
    final private static int TermPair = 5;   // cons/dotted-pair for list construction

    // TermPair can be used in the "natural assembly language"; "list" is its
    // corresponding reserved word, "lists" being a plurality of termpairs
    // arranged as, well, a list. Yeah, I found it cofusing at first myself.
    // If I ever change the "natural assembly language", I might go with "pair"
    // and maybe "just" when the second argument is nil.

    // Hacky: if a variable presented through the API doesn't
    // start with upper case or underscore, prefix it with
    // something that does; likewise, if a constant starts
    // with upper case, prefix it with something lower case;
    // filter these prefixes back out at a later stage of
    // token processing. Eventually there will be an API that
    // gets around the need for this hack.

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
    private Term Terms = null;
    final private Term terms() { return Terms; }   // What's in "(...)" in a compound,
                                            //  or (hacky) the list [lhs,rhs] if equation
    public Term args() { assert tag == Compound;  return terms(); }
  
    final private Term set_terms(Term Ts) { Terms = Ts;  return Ts; }

    Term next = null;  // cdr in LISP

    // deep equality
    Boolean is_same_as (Term t) {
        if (t == null) return false;
        if (tag != t.tag) return false;
        if (t.S_ != null) {
            if (S_ == null) return false;
            if (t.S_.compareTo(S_) != 0) return false; 
        }

        Term t1 = t.Terms;
        if (Terms != null)
            for (Term tt = Terms; tt != null; tt = tt.next) {
            // for (Term tt : Terms) {              // why is this not equivalent?
                if (t1 == null) return false; 
                if (!tt.is_same_as(t1)) return false; 
                t1 = t1.next;
            }

        return t1 == null;
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
            // iffy, when termpair is actually compound with "|" functor
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
    public Boolean is_an_equation(){  return tag == Compound && c() == "="; }
    public Boolean is_a_termpair() {  return (tag == Compound && c() == "|") || tag == TermPair; }

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
    public static Term termpair(Term car, Term cdr) {
        // Main.println ("termpair (car=<<<"+car+">>>,cdr=<<<"+cdr+">>>)");
        Term Ts = car.clone();
        assert Ts != car;
        Ts.next = cdr.clone();
        assert car.next != Ts.next;
        Term r = new Term (TermPair, "|", Ts);
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


        for (Term t = Terms; t != null; t = t.next) {
            s = s + delim;
            s = s + t;
            delim = sep;
        }
        
        return s;
    }

    private boolean is_lists() {
        return (tag == Compound && this.c() == "lists") || tag == TermList;
    }

    public String toString() {
        String r = "<unassigned>";

        switch (tag) {
            case Variable: r = v(); break;
            case Constant: r = c(); break;
            case Compound: if (this.is_a_termpair()) {
                                if (in_Prolog_mode)
                                    r = terms_to_str("|");
                                else
                                    r = cons + args_start + terms_to_str(arg_sep) + args_end;
                            } else
                            if (!this.is_an_equation()) {                                 
                                 r = c() + args_start + terms_to_str(arg_sep) + args_end;
                            } else {
                                if (rhs().is_lists()) // ?????? WHY ???????? 
                                {    
                                    // Main.println ("(((((( do we ever get here (rhs is lists in eqn???? ))))))");                                                              
                                    r = lhs() + " " + this.rhs();
                                    // Main.println ("        now r = " + r);
                                } else {
                                    // Main.println ("(((((( how about here???? (rhs NOT is lists in eqn???? )))))))");     
                                    r = lhs() + holds_op + rhs();
                                    // Main.println ("        now r = " + r);
                                }
                            }
                            break;
            case TermList:  r =  list_start + terms_to_str(list_elt_sep) + list_end;
                            break;
            case TermPair:  if (in_Prolog_mode)
                                r = terms_to_str ("|");
                            else
                                r = cons + terms_to_str (list_elt_sep);

                            break;
        }
        return r;
    }

    public Term takes_this(Term t) {
        assert tag == Compound;
        assert t != null;

        this.Terms = add_elt(terms(), t.clone());
        assert Terms != null;
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

 /* flatten this */

    static Term add_elt (Term x, Term elt) {
        if (x == null) return elt;
        assert elt != null;
        assert elt.next != elt;
        assert x != elt;
        assert (x.next != x);

        for (Term i = x; i != null; i = i.next) {
            assert i.next != i;
            if (i.next == null) {
                i.next = elt;
                break;
            }
        }
        return x;
    }
    Term add_all (Term x, Term Ts) {
        if (Ts == null) return x;
        return add_elt (x, Ts);
    }

    // iffy naming here -- doesn't copy next link, so maybe it should be called "shallow_copy"
    public Term clone() {
      return new Term(this.tag, this.S_, this.Terms);
    }

    private class nvpair {
        String n;
        Term v;
        public nvpair(String n, Term v) { this.n = n; this.v = v; }
    }

    // Need to figure out side effects:
    //
    private static int tab = 0;
    private static String tabs() {
        String s = "";
        for (int i = tab; i > 0; --i) s += "| ";
        return s;
    }
    private static final void indent() { ++tab; }
    private static final void dedent() { assert tab > 0; --tab; }

    private static int limit = 20; 

    private  void flappin (LinkedList<nvpair> buf, LinkedList<nvpair> result) {
        assert --limit > 0;
        // indent();
        String s_ = "";
        if (S_ != null) s_ = S_;
        // Main.println (tabs()+s_+annote());
        // LinkedList<Term> todo = new LinkedList<Term>();
        Term new_terms = null;
        for (Term t = terms(); t != null; t = t.next)
            if (t.is_simple()) {
                // Main.println (tabs() + "Adding <<<"+t+">>> to new_terms");
                new_terms = Term.add_elt(new_terms, t.clone());
                // buf.add (new nvpair(null, t));
            } else {
                // Main.println (tabs() + "Adding <<<"+t+">>> to new_terms");
                Term v = variable(Term.gensym());
                new_terms = add_elt (new_terms, v);
                nvpair nvp = new nvpair(v.v(),t);
                buf.add (nvp);
            }
        
        while (!buf.isEmpty()) {
            nvpair x = buf.pop();
            // Main.println (tabs() + " .... buf.pop(): "+x.n+" = "+x.v);
            // Main.println (tabs() + " .... recursion on that:");
            x.v.flappin (buf, result);
            // Main.println (tabs() + "now, x is " + x.n + "="+x.v);
            result.add (x);
        }
                
        Terms = new_terms;
        // Main.println (tabs() + "Finishing with " + this);
        // dedent();
    }

    private String annote() {
        String type = "?";
        if (this.is_an_equation()) type = "==";
        if (this.is_a_termpair())  type = ".";
        if (this.is_a_compound())  type = "(...)";
        if (this.is_a_termlist())  type = "[...]";
        if (this.is_a_variable())  type = "$";
        if (this.is_a_constant())  type = "#";
        return type;
    }

// nth attempt to rewrite flatten():

    public Term flatten() {
        // save successor of this and isolate it
        Term save_next = this.next;
        this.next = null;
        tab = 0;
        limit = 20;
        // reset_gensym();
        LinkedList<nvpair> buf = new LinkedList<nvpair>();
        LinkedList<nvpair> result = new LinkedList<nvpair>();

        // Main.println ("flatten -- starting....");

        this.flappin(buf, result);

        // Main.println ("flatten: buf=");
        // for (nvpair x : buf)
        //    Main.println ("      ... " + x.n + " = " + x.v);
        // Main.println ("flatten: result=");
        
        // for (nvpair x : result)
        //    Main.println ("      ... " + x.n + " = " + x.v);
        // Main.println ("flatten: this is now " + this);

        Term tt = this;
        for (nvpair x : result) {
            // Main.println (">>>---   " + x.v + ", ");
            if (x.v.is_a_termlist()) { // hacky & dubious post-processing
                Main.println ("          x.v=" + x.v);
                Term xvterms = x.v.terms();
                if (xvterms != null) {
                    if (xvterms.next != null) {
                        if (xvterms.next.is_a_variable()) {
                            if (xvterms.next.v().startsWith("_")) {
                                // Main.println ("**** BINGO ****");
                                x.v.tag = TermPair;
                                assert x.v.S_ == null;
                            }
                        }
                    }
                }
            }
            tt.next = equation(variable(x.n),x.v);
            tt = tt.next;
        }

        assert tt != null;
        assert tt.next == null;

        // Main.println ("flatten return value....");
        for (Term tx = this; tx != null; tx = tx.next) {

        }

        if (save_next != null)
            save_next.flatten();
        tt.next = save_next;

        return this;
    }
}

