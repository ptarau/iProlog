
package org.iprolog;
//import java.util.Arrays;
import java.util.stream.Stream;
import java.util.Spliterator;
import java.util.function.Consumer;
import java.util.stream.StreamSupport;

/* Prog is mainly an Engine with some extended trace output */

public class Prog extends Engine implements Spliterator<Object> {
  Prog(final String fname) {
    super(fname, true);
  }
  Prog (final String s, Boolean fromFile) {
    super(s, fromFile);
  }

  static void pp(final Object o) {
    Main.pp(o);
  }

  static void println(final Object o) {
    Main.println(o);
  }

  // From Engine
  @Override
  String showTerm(final Object O) {
    if (O instanceof Object[])
      return make_string_from((Object[]) O);
    return O.toString();
  }

  static String maybeNull(final Object O) {
    if (null == O)
      return "$null";   // meaningful? or should raise exception?
    if (O instanceof Object[])
      return make_string_from((Object[]) O);
    return O.toString();
  }

  static boolean isListCons(final Object name) {
    return ".".equals(name) || "[|]".equals(name) || "list".equals(name);
  }

  static boolean isInfixOp(final Object name) {
    return "/".equals(name) || "-".equals(name) || "+".equals(name) || "=".equals(name);
  }

  static String make_string_from(final Object[] f_of_args) {
    final StringBuffer buf = new StringBuffer();
    final String name = f_of_args[0].toString();

    if (f_of_args.length == 3 && isInfixOp(name)) {
         ////////////////////////////////////
        /// Doesn't look right.
       /// Seems like it should be args 1 0 2.
      //////////////////////////////////////
      Main.println ("+++++++++++++ isInfixOp case ++++++++++++++");
      buf.append(Term.args_start + maybeNull(f_of_args[0]) + name + Term.args_end);
    } else if (f_of_args.length == 3 && isListCons(name)) {
      buf.append('[');
      {
        buf.append(maybeNull(f_of_args[1]));
        Object tail = f_of_args[2];
        for (;;) {
          if ("[]".equals(tail) || "nil".equals(tail))
            break;
          if (!(tail instanceof Object[])) {
            buf.append("|" + maybeNull(tail));
            break;
          }
          final Object[] list = (Object[]) tail;
          if (!(list.length == 3 && isListCons(list[0]))) {
            buf.append("|" + maybeNull(tail));
            break;
          } else {
            //if (i > 1)
            buf.append(',');
            buf.append(maybeNull(list[1]));
            tail = list[2];
          }
        }
      }
      buf.append(']');
    } else if (f_of_args.length == 2 && "$VAR".equals(name)) { // when?
      Main.println("$$$$$$$$$$ $VAR $$$$$$$$$$$$$$");
      buf.append("_" + f_of_args[1]);
    } else {
      buf.append(maybeNull(f_of_args[0]) + Term.args_start);
      String sep = "";
      for (int i = 1; i < f_of_args.length; i++) {
        buf.append(sep + maybeNull(f_of_args[i]));
        sep = Term.arg_sep;
      }
      buf.append(")");
    }
    return buf.toString();
  }

  void ppCode() {
    pp("\nSYMS:");
    pp(syms);
    pp("\nCLAUSES:\n");

    for (int i = 0; i < clauses.length; i++) {

      final Clause C = clauses[i];
      pp("Clause " + i + ":" + showClause(C));
    }
    pp("");

  }

  String showClause(final Clause s) {
    final StringBuffer buf = new StringBuffer();
    final int l = s.hgs.length;
    buf.append("\n ");
    buf.append(showTerm(s.hgs[0]));
    if (l > 1) {
      buf.append(" :- \n");
      for (int i = 1; i < l; i++) {
        final int e = s.hgs[i];
        buf.append("   ");
        buf.append(showTerm(e));
        buf.append("\n");
      }
    } else {
      buf.append("\n");
    }
    buf.append("--- base=" + s.base + " neck=" + s.neck + " -----\n");
    buf.append(showCells(s.base, s.len)); // TODO
    buf.append("\n");
    buf.append(showCell(s.hgs[0]));

    buf.append(" :- [");
    for (int i = 1; i < l; i++) {

      final int e = s.hgs[i];
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
    final IntList bs = S.goal_stack;
    pp("\nppc: trail_top=" + S.trail_top + ", k=" + S.k + " len=" + IntList.len(bs));
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

  @Override
  public boolean tryAdvance(final Consumer<Object> action) {
    final Object R = ask();
    final boolean ok = null != R;
    if (ok) {
      action.accept(R);
    }
    return ok;
  }

}
