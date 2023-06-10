
package org.iprolog;

import java.util.LinkedList;

/**
 * "representation of a clause" (P.Tarau)
 * 
 * [MT]: I decided to pair my API-built in-RAM binary form
 * with Tarau's original, which can hold unfolded
 * programs.
 *   The members "functor", "args", and
 * "body" can be imported into, and exported from,
 * his "compiled assembly language" representation.)
 * In fact, these cleanly map one-for-one, as far
 * as I can tell (i.e., the origina not-unfolded code
 * can be decompiled from his compiled form), but
 * I'll cross that bridge (if it looks sturdy enough)
 * when I come to it.
 */
class Clause {

  // import/export version:

  public Term head;  // equational form will make this plural
  public Term body;
  boolean adding_args = true; // still needed?

  public static Clause f_(String fid) { // still needed?

    Clause cl = new Clause(0,null,0,0,null);

    cl.head = Term.compound(fid);

    cl.body = null;

    return cl;
  }

  // equational form may need to add more heads;
  // where to allow for this?
  public static Clause f__(String fid, Term... ts) {

    assert fid != null;

    Clause cl = new Clause(0,null,0,0,null);

    cl.head = Term.compound(fid);

    assert cl.head != null;
    assert cl.head.is_a_compound();

    for (Term t : ts) 
      cl.head.takes_this(t);

    cl.body = null;

    return cl;
  }

  // Maybe I need to keep this, to add equations to the head?
  // Don't turn off adding_args until if_(...) is encountered?
  // Maybe this is also needed for when body is being built
  // incrementally?

  public Clause __(Term x) {
    assert x != null;
    if (adding_args)
      head.takes_this(x); // Change to adding to head??
    else {
      if (body == null) body = x;
      else body = body.add_elt(body, x);
    }
    return this;
  }

  public Clause if_() {
    adding_args = false;
    return this;
  }

  public Clause if__(Term... body_list) {
    adding_args = false;

    for (Term t : body_list)
      if (body == null)
        body = t;
      else
        body = body.add_elt (body, t);

    return this;
  }

  public String toString() {
    String s = "";
    String sep = "";
    for (Term x = head; x != null; x = x.next) {
      s += sep + x.toString();
      sep = Term.and_op;
    }
    if (body != null) {
      s += Term.if_sym;
      sep = "";
      for (Term x = body; x != null; x = x.next) {
        s += sep + x.toString();
        sep = Term.and_op;
      }
    }
    s += Term.clause_end;
    return s;
  }

  public void flatten() {
      assert head != null;
    head = head.flatten();
      assert head != null;
    Term new_body = null;

    if (body != null)
      // for (Term t = body; t != null; t = t.next) {
      for (Term t : body) {
          if (new_body == null)
            new_body = t.flatten();
          else
            new_body = new_body.add_all (new_body, t.flatten());
      }
    body = new_body;
  }

// Skeletal elements for compiled form:

  final int len; // length of heap slice
  final int[] hgs; // "head+goals pointing to cells in clauses"???
                  // In the video presentation, this seems to be "gs",
                  // described as "the top level skeleton of a clause
                  // containing references to its head then body elements."
                  // Does this mean it's two-element [H,B] though?
                  // From tracing hgs.length, I think not.
  final int base; // the point in the heap where this clause starts
  final int neck; // first after the end of the head (=length of the head)
  final int[] xs; // indexables in head. In the video, this is described as
                  // "the index vector containing dereferenced constants,
                  // numbers or array sizes as extracted from the outermost
                  // term of the head of the clause, with zero values
                  // marking variable positions."
                  // Should it be "outermost termS"?

// Placeholders for Prolog abstract parse tree representation.
// The API here should also be useful for construction, manipulation
// and querying of RAM-only logic programs built from
// NSM/Minimal language specs

// String functor;  // String just a start; this really needs to index into some symbol table
// List<String> terms;

  Clause(final int len, final int[] hgs, final int base, final int neck, final int[] xs) {
    this.hgs = hgs;
    this.base = base;
    this.len = len;
    this.neck = neck;
    this.xs = xs;
  }

}
