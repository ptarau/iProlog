package org.iprolog;

import java.util.LinkedList;

// Prolog "term", with lexical conventions abstracted away.
// E.g., variables don't have to start with a capital letter
// or underscore, and can have embedded blanks.

// Consistent with Paul Tarau's strategy of
// pretending Java classes are like C structs,
// I won't try to make this an interface
// for free-standing Java classes for constants,
// variables and compound terms.

// In C it could probably contain a tagged union.
// Because this is just for building up a semi-compiled form, however,
// its size doesn't matter much.
// It'll be used once then garbage-collected or paged out of caches/RAM
// during any long logic-program execution.

// For tags, note the correpondence to Engine's V, R, and C.
// There may be a reason to merge the tags in Engine with the tags here.
// For now, tolerate smelly redundancy.

public class Term {
    final public static int Variable = 1;   // correponds to Engine.U (unbound variable)
    final public static int Compound = 2;   // correponds to Engine.R (reference)
    final public static int Constant = 3;   // correponds to Engine.C (constant)
    final public static int Equation = 4;   // special for Term = Term
    final public static int MaxTag = Equation;

    final public static String Var_prefix = "V__";
    final public static String Const_prefix = "c__";

    final private int tag;
    String v;
    String c;
    final private LinkedList<Term> terms;   // What's in "(...)" in a compound,
                                            // or (hacky) the list [lhs,rhs] if equation

    Term (int tag, String thing, LinkedList<Term> terms) {
    
        assert (tag > 0 && tag <= MaxTag);
        assert (thing != null);

        if (tag == Compound || tag == Equation) {
            assert (terms != null);
        } else {
            assert (terms == null);
        }

        this.tag = tag;

        switch (tag) {
            case Variable: this.v = thing; this.terms = null;  this.c = null;  return;
            case Compound: this.c = thing; this.terms = terms; this.v = null;  return;
            case Constant: this.v = null;  this.terms = null;  this.c = thing; return;
            case Equation: this.v = null;  this.terms = terms; this.c = null;  return;
        }
// should really raise some exception here
        this.v = null;
        this.c = null;
        this.terms = null;
    }

    public Boolean is_a_variable() {  return this.tag == Variable;  }
    public Boolean is_a_compound() {  return this.tag == Compound;  }
    public Boolean is_a_constant() {  return this.tag == Constant;  }
    public Boolean is_an_equation(){  return this.tag == Equation;  }

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
        return new Term(Equation, "=", ll);
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
    
    protected static String arg_sep;
    protected static String and_op; 
    protected static String args_start;
    protected static String args_end;
    protected static String clause_end;
    protected static String if_sym;
    protected static String holds_op;
    
    // See Toks; there, I squeeze out whitespace
    // from these. Used for pretty-printing the
    // Tarau "assembly language" and (indirectly)
    // in the toSentences lexeme tagger.
    public static void set_TarauLog() {
        arg_sep = " ";
        and_op = " and ";
        args_start = " ";
        args_end = " ";
        clause_end = ".";
        if_sym = "\nif ";
        holds_op = " holds ";
        in_Prolog_mode = false;
    }

    public static boolean set_Prolog() {
        arg_sep = ",";
        and_op = ",";
        args_start = "(";
        args_end = ")";
        clause_end = ".";
        if_sym = ":-";
        holds_op = "=";
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
    public String toString() {
        switch (tag) {
            case Variable: return v;
            case Constant: return c;
            case Compound: return c
                                + args_start
                                + terms_to_str(arg_sep)
                                + args_end;
            case Equation: return lhs() + holds_op + rhs();
        }
        return "<should've thrown exception here>";
    }

    public void takes_this(Term t) {
        assert tag == Compound;
        terms.add (t);
    }

    private static int    gensym_i = 0;
    public  static void   reset_gensym() { gensym_i = 0; }
    public  static String gensym() { return "_" + gensym_i++; }

    private Boolean is_simple() {
        return tag == Variable || tag == Constant;
    }

    public Boolean is_flat() {
        if (this.is_simple()) return true;

//        System.out.println ("In is_flat: this=" + this);

        if (tag == Equation)
            return lhs().is_flat() && rhs().is_flat();

        //depends on representing lhs+rhs as terms list in eqns:
        for (Term t : terms)
            if (!t.is_simple())
                return false;

        return true;
    }

    public LinkedList<Term> flatten() {

// COULD BE LEAKY:
        LinkedList<Term> eqn_form = new LinkedList<Term>();

        if (tag == Variable || tag == Constant) {
            eqn_form.add (this);
            return eqn_form;
        }
        if (tag == Equation) {
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

        // System.out.println ("Flattening a compound: " + this);

        Term nC = compound(c);                      // make a new compound
        eqn_form.add(nC);

        for (Term t : terms) {                      // for each elt in this compound's arg list
            if (t.tag == Variable
             || t.tag == Constant) {                 // if elt is var or const,
                // System.out.println ("  flatten sees t = " + t);
                nC.takes_this (t);                  //      emit elt to new compound
             } else {                                  // else
                // System.out.println ("  flatten sees t = " + t);
                Term nV = variable(gensym());       //      add new variable
                nC.takes_this (nV);                 //      add it to args for new compound
                Term nEq = Term.equation(nV,t);
                eqn_form.addAll (nEq.flatten());
            }
        }
        // while queue not empty
        //   flatten first elt (note side effect on queue)
        //   move ptr (careful of side effects)
        
        return eqn_form;
    }
}
