
package iProlog;
//import java.util.Arrays;
import java.util.stream.Stream;
import java.util.Spliterator;
import java.util.function.Consumer;
import java.util.stream.StreamSupport;

public class Prog extends Engine implements Spliterator<Object> {
  Prog(final String fname) {
    super(fname);
  }

  static void pp(final Object o) {
    Main.pp(o);
  }

  static void println(final Object o) {
    Main.println(o);
  }

  @Override
  String showTerm(final Object O) {
    if (O instanceof Object[])
      return st0((Object[]) O);
    return O.toString();
  }

  static String maybeNull(final Object O) {
    if (null == O)
      return "$null";
    if (O instanceof Object[])
      return st0((Object[]) O);
    return O.toString();
  }

  static boolean isListCons(final Object name) {
    return ".".equals(name) || "[|]".equals(name) || "list".equals(name);
  }

  static boolean isOp(final Object name) {
    return "/".equals(name) || "-".equals(name) || "+".equals(name) || "=".equals(name);
  }

  static String st0(final Object[] args) {
    final StringBuffer buf = new StringBuffer();
    final String name = args[0].toString();
    if (args.length == 3 && isOp(name)) {
      buf.append("(");
      buf.append(maybeNull(args[0]));
      buf.append(" " + name + " ");
      buf.append(maybeNull(args[1]));
      buf.append(")");
    } else if (args.length == 3 && isListCons(name)) {
      buf.append('[');
      {
        buf.append(maybeNull(args[1]));
        Object tail = args[2];
        for (;;) {

          if ("[]".equals(tail) || "nil".equals(tail)) {
            break;
          }
          if (!(tail instanceof Object[])) {
            buf.append('|');
            buf.append(maybeNull(tail));
            break;
          }
          final Object[] list = (Object[]) tail;
          if (!(list.length == 3 && isListCons(list[0]))) {
            buf.append('|');
            buf.append(maybeNull(tail));
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
    } else if (args.length == 2 && "$VAR".equals(name)) {
      buf.append("_" + args[1]);
    } else {
      final String qname = maybeNull(args[0]);
      buf.append(qname);
      buf.append("(");
      for (int i = 1; i < args.length; i++) {
        final Object O = args[i];
        buf.append(maybeNull(O));
        if (i < args.length - 1) {
          buf.append(",");
        }
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
      pp("[" + i + "]:" + showClause(C));
    }
    pp("");

  }

  String showClause(final Clause s) {
    final StringBuffer buf = new StringBuffer();
    final int l = s.hgs.length;
    buf.append("---base:[" + s.base + "] neck: " + s.neck + "-----\n");
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

    buf.append(showTerm(s.hgs[0]));
    if (l > 1) {
      buf.append(" :- \n");
      for (int i = 1; i < l; i++) {
        final int e = s.hgs[i];
        buf.append("  ");
        buf.append(showTerm(e));
        buf.append("\n");
      }
    } else {
      buf.append("\n");
    }
    return buf.toString();
  }

  /*
  String showHead(final Cls s) {
    final int h = s.gs[0];
    return showCell(h) + "=>" + showTerm(h);
  }
  */

  @Override
  void ppGoals(IntList bs) {
    while (!IntList.isEmpty(bs)) {
      pp(showTerm(IntList.head(bs)));
      bs = IntList.tail(bs);
    }

  }

  @Override
  void ppc(final Spine S) {
    //stats();
    final IntList bs = S.gs;
    pp("\nppc: t=" + S.ttop + ",k=" + S.k + "len=" + IntList.len(bs));
    ppGoals(bs);
  }

  /////////////// end of show

  // possibly finite Stream support

  public Stream<Object> stream() {
    return StreamSupport.stream(this, false);
  }

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
