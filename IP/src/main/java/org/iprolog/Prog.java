
package org.iprolog;
//import java.util.Arrays;
import java.util.LinkedList;
import java.util.Objects;
import java.util.stream.Stream;
import java.util.Spliterator;
import java.util.function.Consumer;
import java.util.stream.StreamSupport;

/* Prog is mainly an Engine with some extended trace output */

public class Prog extends Engine implements Spliterator<Object> {
  Prog(final String fname) {
    super();
    this.init_engine();

      clauses = dload(fname);
      assert(clauses != null);
      prep_clauses();
  }
  Prog (final String s, Boolean fromFile)  {
    super(s, fromFile);
  }

  static void pp(final Object o) {
    Main.pp(o);
  }


  public static void println(final Object o) {
    Main.println(o);
  }

  // From Engine
  @Override
  public String showTerm(final Object O) {
    if (O instanceof Object[]) {
      Object [] OO = (Object[]) O;
      assert OO.length != 0;
      return make_string_from((Object[]) O);
    }
    return O.toString();
  }

  static boolean isListCons(final Object name) {
    return ".".equals(name) || "[|]".equals(name) || "list".equals(name);
  }

  static boolean isInfixOp(final Object name) {
    return "/".equals(name) || "-".equals(name) || "+".equals(name) || "=".equals(name);
  }

  public static Term term_made_from(Object O) {
    assert (O != null);
    if (!(O instanceof Object[])) {
      if (O instanceof String || O instanceof Integer) {
        return Term.constant (O.toString());
      }
    } else {
      Object[] oa = (Object[]) O;
      // should try to make sure oa[0] is a functor first
      if (oa.length == 0) {
        Main.println ("oa.length == 0! O is " + O.toString());
        Main.println ("O type is " + O.getClass().getTypeName());
      }
      if (oa.length == 0) {
        return Term.termlist();
      }
      Term f = Term.compound(oa[0].toString());
      for (int i = 1; i < oa.length; ++i) {
          f.takes_this (Objects.requireNonNull(term_made_from(oa[i])));
      }
      return f;
    }
    return null;
  }

  static Term functor_and_args(final Object[] f_of_args) {

    Term f = Term.compound(f_of_args[0].toString());
    for (int i = 1; i < f_of_args.length; ++i) {
      // Prog.println("f.takes_this on term_made_from f_of_args["+i+"]:");
      f.takes_this(Objects.requireNonNull(term_made_from(f_of_args[i])));
    }
    return f;
  }

  public static LinkedList<Term>
  make_terms_from(final Object[] f_of_args) {

    final LinkedList<Term> terms = new LinkedList<>();
    final String name = f_of_args[0].toString();

    if (f_of_args.length == 3 && isInfixOp(name)) {
      // Main.println ("~~~~~~~ isInfixOp case: "+name+" ~~~~~~~");
      Term t = Term.compound(name);
      t.takes_this(Objects.requireNonNull(term_made_from(f_of_args[1])));
      t.takes_this(Objects.requireNonNull(term_made_from(f_of_args[2])));
      terms.add(t);
    } else
    if (f_of_args.length == 3 && isListCons(name)) {
        // Main.println ("~~~~~~~ isInfixOp case: "+name+" ~~~~~~~");
        terms.add(term_made_from(f_of_args[1]));
        Object tail = f_of_args[2];
        for (;;) {
          if ("[]".equals(tail) || "nil".equals(tail))
            break;
          if (!(tail instanceof Object[])) {
            terms.add(term_made_from(tail));
            break;
          }
          final Object[] list = (Object[]) tail;
          if (!(list.length == 3 && isListCons(list[0]))) {
            terms.add(term_made_from(tail));
            break;
          } else {
            //if (i > 1)
            terms.add(term_made_from(list[1]));
            tail = list[2];
          }
        }
    } else if (f_of_args.length == 2 && "$VAR".equals(name)) { // when?
      // Main.println("$$$$$$$$$$ $VAR $$$$$$$$$$$$$$");
      Term t = Term.variable("_" + f_of_args[1]);
      terms.add(t);
    } else {
      // Main.println ("make_terms_from: default case.");
      Term f = functor_and_args(f_of_args);
      terms.add(f);
    }
    return terms;
  }

  static String maybeNull(final Object O) {
    if (null == O)
      return null;  
    if (O instanceof Object[])
      return make_string_from((Object[]) O);
    return O.toString();
  }

  public static String make_string_from(final Object[] f_of_args) {
    assert f_of_args.length > 0;

    final StringBuilder buf = new StringBuilder();

    final String name = f_of_args[0].toString();

    if (f_of_args.length == 3 && isListCons(name)) {
        buf.append('[');
        buf.append(maybeNull(f_of_args[1]));
        Object tail = f_of_args[2];
        for (;;) {
          if ("[]".equals(tail) || "nil".equals(tail))
            break;
          if (!(tail instanceof Object[])) {
            buf.append("|").append(maybeNull(tail));
            break;
          }
          final Object[] list = (Object[]) tail;
          buf.append(Term.arg_sep);
          buf.append(maybeNull(list[1]));
          tail = list[2];
        }

      buf.append(']');
    } else {
      buf.append(maybeNull(f_of_args[0])).append(Term.args_start);
      String sep = "";
      for (int i = 1; i < f_of_args.length; i++) {
        buf.append(sep).append(maybeNull(f_of_args[i]));
        sep = Term.arg_sep;
      }
      buf.append(Term.args_end);
    }
    return buf.toString();
  }

  public void ppCode() {
    pp("\nSYMS:");
    pp(symTab);
    pp("\nCLAUSES:\n");

    for (int i = 0; i < clauses.length; i++) {

      final Clause C = clauses[i];
      pp("Clause " + i + ":" + showClause(C));
    }
    pp("");

  }

  String showClause(final Clause s) {
    final StringBuilder buf = new StringBuilder();
    final int l = s.skeleton.length;
    buf.append("\n ");
    buf.append(showTerm(s.skeleton[0]));
    if (l > 1) {
      buf.append(Term.if_sym).append("\n");
      for (int i = 1; i < l; i++) {
        final int e = s.skeleton[i];
        buf.append("   ");
        buf.append(showTerm(e));
        buf.append("\n");
      }
    } else {
      buf.append("\n");
    }
    buf     .append("--- base=")
            .append(s.base)
            .append(" neck=")
            .append(s.neck)
            .append(" -----\n");
    buf.append(showCells(s.base, s.len)); // TODO
    buf.append("\n");
    buf.append(showCell(s.skeleton[0]));

    buf.append(" :- [");
    for (int i = 1; i < l; i++) {

      final int e = s.skeleton[i];
      buf.append(showCell(e));
      if (i < l - 1) {
        buf.append(", ");
      }
    }

    buf.append("]\n");

    return buf.toString();
  }

  /*
  String showHead(final Cls s) {
    final int h = s.gs[0];
    return showCell(h) + "=>" + showTerm(h);
  }
  */

  // from Engine
  @Override
  void ppGoals(IntList bs) {
    while (!IntList.isEmpty(bs)) {
      pp(showTerm(IntList.head(bs)));
      bs = IntList.tail(bs);
    }
  }

  // from Engine
  @Override
  void ppc(final Spine S) {
    //stats();
    final IntList bs = S.goals;
    pp("\nppc: trail_top=" + S.trail_top + ", last_clause_tried=" + S.last_clause_tried + " len=" + IntList.len(bs));
    ppGoals(bs);
  }

  /////////////// end of show

  // possibly finite Stream support

  public Stream<Object> stream() {
    return StreamSupport.stream(this, false);
  }

  //Spliterator:

  @Override
  public Spliterator<Object> trySplit() {
    return null;
  }

  @Override
  public int characteristics() {
    return (Spliterator.ORDERED | Spliterator.NONNULL) & ~Spliterator.SIZED;
  }

  @Override
  public long estimateSize() {
    return Long.MAX_VALUE;
  }

  // @Override
  public boolean tryAdvance(final Consumer<Object> action) {
    final Object R = POJO_ask();
    final boolean ok = null != R;
    if (ok) {
      action.accept(R);
    }
    return ok;
  }
}
