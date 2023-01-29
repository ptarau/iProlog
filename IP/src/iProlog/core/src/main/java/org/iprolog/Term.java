package org.iprolog;

import java.util.List;

// Consistent with Paul Tarau's strategy of
// pretending Java classes are like C structs,
// I won't try to make this an interface
// for constants, variables and compound terms.

// In C it could probably contain a tagged union.
// Because this is just for building up a semi-compiled form, however,
// its size doesn't matter much.
// It'll be used once then garbage-collected or paged out of caches/RAM
// during any long logic-program execution.
//
// Note the correpondence to Engine's V, R, and C. There may be a
// reason to merge the tags in Engine with the tags here.
// 
public class Term {
    final public static int Variable = 1;   // correponds to Engine.U (unbound variable)
    final public static int Compound = 2;   // correponds to Engine.R (reference)
    final public static int Constant = 3;   // correponds to Engine.C (constant)

    final int tag;

    final String v;
    final String c;
    final List<String> terms;

    Term (int tag, String thing, List<String> terms) {
        this.tag = tag;

        switch (tag) {
            case Variable: this.v = thing; this.terms = null;  this.c = null;  return;
            case Compound: this.c = thing; this.terms = terms; this.v = null;  return;
            case Constant: this.v = null;  this.terms = null;  this.c = thing; return;
        }
        this.v = null;
        this.c = null;
        this.terms = null;
    }
}
