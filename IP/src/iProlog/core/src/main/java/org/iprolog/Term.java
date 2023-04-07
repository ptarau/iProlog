package org.iprolog;

import java.util.LinkedList;

import javax.lang.model.element.Element;
import javax.lang.model.util.ElementScanner6;

// Prolog "term", with lexical conventions abstracted away.
// E.g., variables don't have to start with a capital letter
// or underscore, and can have embedded blanks. Myabe better
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

public class Term {
    final private static int Variable = 1;   // correponds to Engine.U (unbound variable)
    final private static int Compound = 2;   // correponds to Engine.R (reference)
    final private static int Constant = 3;   // correponds to Engine.C (constant)
    final private static int TermList = 4;   // Not in Engine tags because lists expand

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
    String v;
    String c;
    private LinkedList<Term> terms;   // What's in "(...)" in a compound,
                                            // or (hacky) the list [lhs,rhs] if equation
    LinkedList<Term> args() {
        assert tag == Compound;
        return terms;
    }

    Term a_term (int tag, String thing, Term... ts) {
        LinkedList<Term> tl = new LinkedList<Term>();
        for (Term t : ts)
            tl.add (t);
        return new Term (tag, thing, tl);
    }

    Term (int tag, String thing, LinkedList<Term> terms) {

        this.tag = tag;

        switch (tag) {
            case Variable: this.v = thing; this.terms = null;  this.c = null;  return;
            case Compound: this.v = null;  this.terms = terms; this.c = thing; return;
            case Constant: this.v = null;  this.terms = null;  this.c = thing; return;
            case TermList: this.v = null;  this.terms = terms; this.c = null;  return;
        }

// I should really raise some exception here
        this.v = null;
        this.c = null;
        this.terms = null;
    }

    public Boolean is_a_variable() {  return tag == Variable;  }
    public Boolean is_a_compound() {  return tag == Compound;  }
    public Boolean is_a_constant() {  return tag == Constant;  }
    public Boolean is_a_termlist() {  return tag == TermList;}
    public Boolean is_an_equation(){  return tag == Compound && terms.size() == 2 && c == "=";  }

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
    public static Term compound(String C) {
        return new Term (Compound, C, new LinkedList<Term>());
    }
    public static Term compound(String C, LinkedList<Term> terms) {
        return new Term (Compound, C, terms);
    }
    public static Term compound(String C, Term t) {
        LinkedList<Term> llt = new LinkedList<>();
        llt.add (t);
        return compound (C, llt);
    }
    public static Term equation(Term lhs, Term rhs) {
        LinkedList<Term> ll = new LinkedList<Term>();
        ll.add (lhs);
        ll.add (rhs);
        return new Term(Compound, "=", ll);
    }

    public static Term termlist(Term... ts) {
        Main.println ("Entering termlist(Term... ts) ...");
        LinkedList<Term> ll = new LinkedList<Term>();
        for (Term t : ts)
            ll.add(t);
        return termlist(ll);
    }

    public static Term termlist(LinkedList<Term> llt) {
        Term t = new Term (TermList, "[...]", llt);

        // Main.println ("New termlist: " + t.toString());

        return t;
    }

    public Term lhs() {
        assert this.is_an_equation();
        return terms.peekFirst();
    }

    public Term rhs() {
        assert this.is_an_equation();
        return terms.peekLast();
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
        for (Term t : terms) {
            s = s + delim;
            s = s + t;
            delim = sep;
        }
        return s;
    }

    private boolean is_lists() {
        return tag == Compound && this.c == "lists";
    }

    public String toString() {
        String r = "<unassigned>";

        switch (tag) {
            case Variable: r = v; break;
            case Constant: r = c; break;
            case Compound: if (c != "=")
                                 r =      c
                                        + args_start
                                        + terms_to_str(arg_sep)
                                        + args_end;
                            else {
                                if (rhs().is_lists()) {
                                    r = lhs().toString() + " " + rhs().toString();
                                    // Main.println ("holds lists reduced to r = " + r);
                                } else
                                    r =   lhs()
                                        + holds_op
                                        + rhs();
                            }
                            break;
            case TermList:  r =  list_start + terms_to_str(list_elt_sep) + list_end;
                            break;
        }
        return r;
    }

    public void takes_this(Term t) {
        assert tag == Compound;
        terms.add (t);
    }

    public void takes_this(LinkedList<Term> llt) {
        assert llt != null;
        terms.addAll(llt);
    }

    private static int    gensym_i = 0;
    public  static void   reset_gensym() { gensym_i = 0; }
    public  static String gensym() { return "_" + gensym_i++; }

    private Boolean is_simple() {
        return tag == Variable || tag == Constant;
    }

    public Boolean is_flat() {
        if (this.is_simple()) return true;

        if (c == "=")
            return lhs().is_flat() && rhs().is_flat();

        //depends on representing lhs+rhs as terms list in eqns:
        for (Term t : terms)
            if (!t.is_simple())
                return false;

        return true;
    }
/* 
    public LinkedList<Term> flatten() {

// COULD BE LEAKY:
        LinkedList<Term> eqn_form = new LinkedList<Term>();

        if (tag == Variable || tag == Constant) {
            eqn_form.add (this);
            return eqn_form;
        }

        if (tag == TermList) {  // this should be something like Compound
                                // except each list/sublist functor is "lists"
            // First, generate a variable Vroot to substitute for the list
            // for each term
            //    if the term was a list
            //       convert it to "lists(<elts>)"
            //       R = flatten that
            // generate "Vroot, Vroot = <R>"
            // PROBLEM WITH THIS:
            //   if you have 2+ lists of args
            // SO:
            //   need to keep substituting in expansion
        }

        if (c == "=") {  // special kind of compound
            // System.out.println ("flattening an equation: " + this);
            LinkedList<Term> Lhs = lhs().flatten();
            Term eqn_lhs = Lhs.pop();
            LinkedList<Term> Rhs = rhs().flatten();
            Term eqn_rhs = Rhs.pop();
            // System.out.println ("Lhs 1st elt = " + eqn_lhs);
            // System.out.println ("Rhs 1st elt = " + eqn_rhs);

            Term nE = equation(eqn_lhs, eqn_rhs);
            // System.out.println ("... equation flattened: " + nE + "...:");
            // for (Term t : Lhs) System.out.println ("  lhs: " + t);
            // for (Term t : Rhs) System.out.println ("  rhs: " + t);
            eqn_form.add(nE);
            eqn_form.addAll (Lhs);
            eqn_form.addAll (Rhs);
            return eqn_form;
        }

        System.out.println ("Flattening a compound or a list: " + this);

        Term nX;
Main.println("tag="+tag);
        assert (tag == Compound);
        nX = compound(c);                      // make a new compound
Main.println("tag="+tag);

        for (Term t : terms) {                      // for each elt in this compound's arg list
            if (t.tag == Variable
             || t.tag == Constant) {                 // if elt is var or const,
                // System.out.println ("  flatten sees t = " + t);
                nX.takes_this (t);                  //      emit elt to new compound
            } else
            if (t.tag == Compound) {                                  // else
                 System.out.println ("  flatten sees t = " + t);
                Term nV = variable(gensym());       //      add new variable
                nX.takes_this (nV);                 //      add it to args for new compound
                Term nEq = Term.equation(nV,t);
                eqn_form.addAll (nEq.flatten());
            } else 
            if (t.tag == TermList) {
                System.out.println ("  flatten sees t = " + t);
                Term nV = variable(gensym());       //      add new variable
                nX.takes_this (nV);                 //      add it to args for new compound
                Term nTL = Term.termlist(t.terms);
                eqn_form.addAll (nTL.flatten());
            }
        }
        // while queue not empty
        //   flatten first elt (note side effect on queue)
        //   move ptr (careful of side effects)
        
        return eqn_form;
    }

*/
    

// an in-place flatten that returns residue "squeezed out"
// this transforms clause structures
// it's applied first to the head, then to the body
// Making it DFS in what it flattens.
// Trying to make it work in-place -- i.e., it
// rewrites the structure given to it.
// The original should be recoverable (P. Tarau)
// Not sure any of this is right.
// Basically, loop until residue gone.

public static void fatten (Clause cl) {

    // Main.println ("\n\nEntering fatten(Clause cl) ...");

    // flatten head -
    //      loop through args
    //              add flatten of each to residue
    //      add residue to head

    LinkedList<Term> headlist = new LinkedList<Term>();
    assert cl.head.size() == 1;
    for (Term hh : cl.head.peekFirst().args()) {
       //  Main.println ("  hh arg before = " + hh.toString());
        LinkedList<Term> llt = hh.fatten();
        // Main.println ("  hh arg after = " + hh.toString());
        // Main.println ("    llt = " + llt);
        headlist.addAll(llt);
    }

    assert cl.head.size() == 1;
    headlist.addFirst(cl.head.getFirst());
    cl.head = headlist;
    // Main.println ("head => " + cl.head);
    // Main.println ("headlist = " + headlist);

    // flatten body
    //      loop through expressions/conditions
    //              do flattening of each
    //              add the residue to each
    LinkedList<Term> bodylist = new LinkedList<Term>();

    for (Term bb : cl.body) {
        // Main.println ("  bb before = " + bb);
        if (bb.is_flat()) {
            // Main.println ("**** bb IS FLAT ALREADY ****");
            bodylist.add(bb);
        }
        else {
            LinkedList<Term> llt = bb.fatten();
            // Main.println ("  bb after = " + bb);
            Term first = llt.removeFirst();
            assert first != null;
            assert (first.is_an_equation());
            Term lhs = first.terms.removeFirst();
            assert lhs.is_a_variable();
            assert lhs.v.charAt(0) == '_';
            Term rhs = first.terms.removeFirst();
            assert rhs != null;
            llt.addFirst(rhs);
            bodylist.addAll(llt);

            // Main.println ("    bodylist now = " + bodylist);
        }
    }

    // Main.println ("bodylist now = " + bodylist);
    // Main.println ("cl.body before that addAll = " + cl.body);
    // bodylist.addAll (cl.body);
    cl.body = bodylist;
    // Main.println ("body = " + cl.body);
    // Main.println ("bodylist = " + bodylist);

    // Main.println ("...exit from fatten (Clause cl)\n");
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

flatten this:
    residue list = empty
    if this is a compound
        args list = empty
        f = this.c
        for t in terms
            if t is a variable or a constant
                add t to args list
            else
                add flatten t to residue list
        Term e = equation "<gensym> = f(<args list>)"
        add e to residue list
    return residue list
 */

private static LinkedList<Term>
fatten (LinkedList<Term> tl) {
        LinkedList<Term> residue = new LinkedList<Term>();

        // Main.println ("entered fatten on linked list of terms ...");

        for (Term t : tl) {
                residue.addAll (t.fatten());
        }

        // Main.println ("... exiting fatten on linked list of terms ...");
        return residue;
}

private static int indent_level = 0;
private static String tab() {
    String s = "";
    for (int i = 0; i < indent_level; ++i) s += "|  ";
    return s;
}

public boolean is_not_flat() { return this.is_a_compound() || this.is_a_termlist(); }

LinkedList<Term>
fatten() {
    ++indent_level;
        LinkedList<Term> residue = new LinkedList<Term>();

        if (this.is_a_termlist()) {
           // Main.println (tab() + "fatten: this is termlist " + this);
           tag = Variable;
           this.v = gensym(); // replace with a var
           this.c = null;
           Term e = equation(this, compound ("lists", terms));

           for (Term t : terms)
            if (t.is_not_flat()) {
                LinkedList<Term> ft = t.fatten();
                // Main.println (tab()+"ft.peekFirst()=" + ft.peekFirst());
                assert ft.peekFirst().is_an_equation();
                residue.addAll (ft);
            }
           // Main.println (tab()+"e = " + e);
           // Main.println (tab()+"this now = " + this);
           residue.addFirst(e);
        } else
        if (this.is_an_equation()) { /* special compound */
            // Main.println (tab() + "fatten: this is equation " + this);
            residue.addAll(this.lhs().fatten());
            residue.addAll(this.rhs().fatten());
        } else
        if (this.is_a_compound()) {
                // Main.println (tab() + "fatten: this is compound " + this);
                LinkedList<Term> args_list = new LinkedList<Term>();
                
                String f = this.c;     // a functor_of(t) or t.s_functor() would be nice here
                this.c = null;
                this.v = gensym();
                this.tag = Variable;
                for (Term t : terms) {
                    residue.addAll (t.fatten());
                    args_list.add (t);
                   // Main.println (tab() + "fatten: residue is now " + residue);
                }

                /*
                // rewrite args of compound:
                for (Term t : terms) {
                    if (t.is_a_variable() || t.is_a_constant()) {
                        args_list.add(t);
                        Main.println (tab() + "added to args_list: t = " + t);
                    } else {
                        // here we need to add a variable derived from flattening t
                        LinkedList<Term> ft = t.fatten();

                        if (ft.size() != 0) {
                            Term vvv = ft.peekFirst();
                            Main.println ("ft.peekFirst()=" + vvv);
                            assert vvv.is_an_equation();
                            args_list.add (vvv.lhs());
                        }
                        residue.addAll (ft);
                    }
                }
                */

                Term c = compound (f, args_list);
                Term e = equation (this, c);
                residue.addFirst(e);
                this.terms = null;
                
                // Main.println (tab() + "fatten compound: this is now " + this);
                // Main.println (tab() + "fatten compound: ... and residue is finally " + residue);
                // this.terms = args_list;
         }
         else
         if (this.is_a_variable()) {
            // Main.println (tab() + "fatten: this is variable " + this)
            ;
         } else 
         if (this.is_a_constant()) {
            // Main.println (tab() + "fatten: this is constant " + this)
            ;
         } else
            assert(false);

        // Main.println (tab()+"fatten: this changed to " + this);
        // Main.println (tab()+"fatten: residue to return is now " + residue);
    --indent_level;
         return residue;
 }
 


}



