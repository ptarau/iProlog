package iProlog;

import java.util.stream.Stream;

enum Main {
    ;

    static void println(Object o) {
        System.out.println(o);
    }

    static void pp(Object o) {
        System.out.println(o);
    }

    private static void run(String fname0) {
        boolean p = true;

        String fname = fname0 + ".nl";
        Engine P;

        if (p) {
            P = new Prog(fname);
            pp("CODE");
            ((Prog) P).ppCode();
        } else {
            P = new Engine(fname);
        }

        pp("RUNNING");
        long t1 = System.nanoTime();
        P.run();
        long t2 = System.nanoTime();
        System.out.println("time=" + (t2 - t1) / 1000000000.0);

    }

    public static void srun(String fname0) {
        String fname = fname0 + ".nl";
        Prog P = new Prog(fname);

        pp("CODE");
        P.ppCode();

        pp("RUNNING");
        long t1 = System.nanoTime();

        Stream<Object> S = P.stream();
        S.forEach(x -> pp(P.showTerm(x)));

        long t2 = System.nanoTime();
        System.out.println("time=" + (t2 - t1) / 1000000000.0);
    }

    public static void main(String[] args) {
        String fname = args[0];
        run(fname);
    }
}
