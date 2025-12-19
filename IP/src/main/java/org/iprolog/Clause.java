
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
public class Clause {

  // import/export version:

  public Term head;  // equational form will make this plural
  public Term body;
  boolean adding_args = true; // still needed?

  static LinkedList<Clause> say_(Clause... args) {
    LinkedList<Clause> llc = new LinkedList<>();
    for (Clause c : args) llc.add (c);
    return llc;
  }

  public static Clause f__(Term hd) {
    Clause cl = new Clause(0,null,0,0,null);
    cl.head = hd;
    return cl;
  }

  // equational form may need to add more heads;
  // where to allow for this?
  public static Clause f__(String fid, Term... ts) {

    assert fid != null;

    Clause cl = new Clause(0,null,0,0,null);

    cl.head = Term.compound(fid);
 
    // Main.println ("   IN Clause.f__: cl.head is <<<" + cl.head + ">>>");

    assert cl.head.is_a_compound();

    for (Term t : ts) {
      // Main.println ("   IN Clause.f__: adding " + t);
      cl.head.takes_this(t);
    }

    cl.body = null;

    // Main.println ("    IN Clause.f__: this is now <<<" + cl + ">>>");

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
      else body = Term.append_elt_to_ll(x, body);
    }
    return this;
  }

  public Clause if_(LP_... body_list) {
    adding_args = false;
    assert body == null;

    for (LP_ f : body_list) {
      // Main.println ("   Clause.if__: adding <<<" + t + ">>> to body...");
      if (body == null) {
        body = f.run.fn();
      }  else {
        body = Term.append_elt_to_ll (f.run.fn(), body);
      }
    }
    return this;
  }

  public Clause if_(Term... body_list) {
    adding_args = false;
    assert body == null;

    for (Term t : body_list) {
      // Main.println ("   Clause.if__: adding <<<" + t + ">>> to body...");
      if (body == null) {
        body = t;
      }  else {
        body = Term.append_elt_to_ll (t, body);
      }
    }

    return this;
  }

  public String toString() {
    String s = "Clause show stub";
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
    Term.reset_gensym();
    head = head.flatten();
      assert head != null;
    // Term.reset_gensym();
    if (body != null)
        body.flatten();
  }

// Skeletal elements for compiled form:

  final int len; // length of heap slice
  final int[] skeleton; // "head+goals pointing to cells in clauses"???
                  // In the video presentation, this seems to be "gs",
                  // described as "the top level skeleton of a clause
                  // containing references to its head then body elements."
                  // Does this mean it's two-element [H,B] though?
                  // From tracing hgs.length, I think not.
  final int base; // the point in the heap where this clause starts
  final int neck; // first after the end of the head (=length of the head)
  final int[] index_vector; // indexables in head. In the video, this is
		  // described as
                  // "the index vector containing dereferenced constants,
                  // numbers or array sizes as extracted from the outermost
                  // term of the head of the clause, with zero values
                  // marking variable positions."
                  // Should it be "outermost termS"?

  public String othershow() {
    String s = "\n";

    s += ("      len="+len);
    s += ("  base="+base);
    s += ("  neck="+neck);
    s += ("  index_vector=(TBD)\n");
    s += ("      skeleton=[");
    String sep ="";
    for (int i : skeleton) { s += (sep+i); sep=","; }
    s += "]\n";

    return s;
  }

// Placeholders for Prolog abstract parse tree representation.
// The API here should also be useful for construction, manipulation
// and querying of RAM-only logic programs built from
// NSM/Minimal language specs

// String functor;  // String just a start; this really needs to index into some symbol table
// List<String> terms;

  Clause(final int len, final int[] skeleton, final int base, final int neck, final int[] index_vector) {
    this.skeleton = skeleton;
    this.base = base;

    // Prog.println("     $$$$$$$$$$$$$$$$$$ Clause constructor: base<-" + base);

    this.len = len;
    this.neck = neck;
    this.index_vector = index_vector;
  }

}
