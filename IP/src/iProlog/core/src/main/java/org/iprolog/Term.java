package org.iprolog;

import java.util.List;

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
    final public static int MaxTag = Constant;

    final int tag;

    final String v;
    final String c;
    final List<Term> terms;

    Term (int tag, String thing, List<Term> terms) {
    
        assert (tag > 0 && tag <= Constant);
        assert (thing != null);

        if (tag == Compound) {
            assert (terms != null);
        } else {
            assert (terms == null);
        }

        this.tag = tag;

        switch (tag) {
            case Variable: this.v = thing; this.terms = null;  this.c = null;  return;
            case Compound: this.c = thing; this.terms = terms; this.v = null;  return;
            case Constant: this.v = null;  this.terms = null;  this.c = thing; return;
        }
// should really raise some exception here
        this.v = null;
        this.c = null;
        this.terms = null;
    }

    public Boolean is_a_variable() {  return this.tag == Variable;  }
    public Boolean is_a_compound() {  return this.tag == Compound;  }
    public Boolean is_a_constant() {  return this.tag == Constant;  }

    public static Term variable(String v) {  Term t = new Term (Variable, v, null); return t; }
    public static Term compound(String C,
                         List<Term> terms) {
        assert (terms != null);  
        Term t = new Term (Compound, C, terms);
        return t;
    }
    public static Term constant(String c) {  Term t = new Term (Constant, c, null); return t; }

    private String terms_to_str() {
        String delim = "";
        String s = "";
        for (Term t : terms) {
            s = s + delim;
            s = s + t;
            delim = ",";
        }
        return s;
    }
    public String toString() {
        switch (tag) {
            case Variable: return v;
            case Constant: return c;
            case Compound: return c + "(" + terms_to_str() + ")";
        }
 
        return "<should have thrown exception here>";
    }
}
