package org.iprolog;
import java.util.stream.Stream;

public class Main {

  static void println(final Object o) {
    System.out.println(o);
  }

  static void pp(final Object o) {
    System.out.println(o);
  }

  public static void run(final String fname0) {
    final boolean p = true;

    final String fname = fname0 + ".nl";

    Main.println ("Setting Taraulog....");
    Term.set_TarauLog();

    Engine P;

    if (p) {
      P = new Prog(fname);
      pp("CODE");
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
    final Prog P = new Prog(fname);

    pp("CODE");
    P.ppCode();

    pp("RUNNING");
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
    run(fname);
  }
}
