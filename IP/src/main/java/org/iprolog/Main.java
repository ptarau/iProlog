package org.iprolog;
import java.util.stream.Stream;

public class Main {

  static void println(final Object o) {
    System.out.println(o);
  }

  static void pp(final Object o) {
    System.out.println(o);
  }

  static void test_IntSet() {
    IntMap im = new IntMap();
    /// Main.println("************** test_IntSet() ***********************************");

    for (int i = 1; i < 9; ++i) {
      // cout << "about to call add_key";
      im.add(i);
      // cout << "about to call put";
      im.put(i, i + 1);
      // cout << " about to call get";
      int v = im.get(i);
      // cout << "test_IntSet i = " << to_string(i) << endl;
      if (!(v == i + 1)) System.exit(-1);
    }

    // Main.println("************** END test_IntSet() ***********************************");
  }
  public static void run(final String fname0) {

    Prog.println("run: about to call test_IntSet");
    test_IntSet();

    final boolean p = true;

    final String fname = fname0 + ".nl";

    Main.println ("Setting Taraulog....");
    Term.set_TarauLog();

    Prog.println("==============================================================");

    Engine P;
    if (p) {
      P = new Prog(fname);
      // P = P0.fork(); // left over from supporting clone()
      pp("CODE:");
      ((Prog) P).ppCode();
    } else {
      P = new Engine(fname, true);
    }

    Term.set_Prolog();
    if (Term.in_Prolog_mode) Main.println ("In Prolog mode:");
    else Main.println ("NOT in Prolog mode:");
    pp("RUNNING");
    final long t1 = System.nanoTime();
    P.run();
    final long t2 = System.nanoTime();
    System.out.println("time=" + (t2 - t1) / 1000000000.0);

  }

  public static void srun(final String fname0) {
    final String fname = fname0 + ".nl";
    Main.println ("Setting Taraulog....");
    Term.set_TarauLog();
;
      final Prog P = new Prog(fname);

      pp("CODE");
      P.ppCode();
      P.pp("RUNNING");
      final long t1 = System.nanoTime();

      final Stream<Object> S = P.stream();
      S.forEach(x -> Main.pp(P.showTerm(x)));

      final long t2 = System.nanoTime();
      System.out.println("time=" + (t2 - t1) / 1000000000.0);
  }

  public static void main(final String[] args) {

    String currentPath = null;
    try {
     currentPath = new java.io.File(".").getCanonicalPath();

    } catch (Exception e) {
      System.out.println ("Couldn't get current path");
    }

    System.out.println("Current dir:" + currentPath);
   
    String currentDir = System.getProperty("user.dir");
    System.out.println("Current dir using System:" + currentDir);

    String fname=args[0];

      Prog.println("about to run...");
      run(fname);
  }
}
