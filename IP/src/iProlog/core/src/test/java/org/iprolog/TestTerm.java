package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;


import java.util.LinkedList;
import java.util.Arrays;

public class TestTerm {

    private static final Term v_(String s) { return Term.variable(s); }
    private static final Term c_(String s) { return Term.constant(s); }
    private static final Term s_(String s) { return Term.compound(s); }
    private static final Term s_(String s, Term... ts) {
                                        Term xt = Term.compound(s); 
                                        for (Term t : ts)
                                            xt = xt.takes_this(t);
                                        return xt;
                                    }
    private static final Term e_(Term lhs, Term rhs) { return Term.equation (lhs,rhs); }
    private static final Term l_(Term... ts) {
                                        if (ts.length == 0)
                                            return c_("nil");
                                        return Term.termlist(ts);
                                    }
    private static final Term p_(Term car, Term cdr) {
                                        return Term.termpair(car,cdr);
                                    }

    /**
   * Initiator and consumer of the stream of answers
   * generated by this engine.
   */
    private void expect_from(Prog P, String[] whats_expected) {
        Object POJO_Ans;

        if (whats_expected != null) {
            Main.println ("whats_expected =");
            for (String s : whats_expected) {
                Main.println ("  Looks like " + s + " when prolog_mode =  " + Term.in_Prolog_mode);
            }
        }

        // Going through the Java Object interface, unfortunately.

        int limit = 40;
        while ((POJO_Ans = P.POJO_ask()) != null) {
            Main.println ("\nIn POJO_ask() loop....");
            assert POJO_Ans instanceof Object[];  // because it'll be "[goal, <answer>]"

            Object[] POJO_goal_answers = (Object[]) POJO_Ans;

            LinkedList<Term> llt = Prog.make_terms_from(POJO_goal_answers);

            assert (llt.size() == 1);
            Term goal_ans = llt.get(0);
            assert goal_ans.is_a_compound();
            assert goal_ans.c().compareTo("goal") == 0;
            Term args = goal_ans.args();

            int i = 0;
            String to_compare = "";
            for (Term t = args; t != null; t = t.next) {
                to_compare += t.toString();
                Prog.println ("  to_compare = " + to_compare);
                Prog.println ("  goal ["+(i++)+"] = " + t);
            }
 
            String sg = P.showTerm(POJO_goal_answers[0]);
            assert sg.equals("goal");   // because it'll be "[goal, <answer>]"
            assert POJO_goal_answers.length > 1;
            assert POJO_goal_answers.length < 3;
            Main.println ("POJO_goal_answers[1] is " + POJO_goal_answers[1]);
            String show_POJO_object = P.showTerm(POJO_goal_answers[1]);

            if (whats_expected != null) {
                Prog.println ("  to_compare = " + to_compare);
                Prog.println ("  show_POJO_object = " + show_POJO_object);
                // assert Arrays.asList(whats_expected).contains(so);
            } else {
                Main.println (" yielding: " + show_POJO_object);
            }
            if (--limit < 0) {
                Main.println ("Exceeded temporary limit on answers");
                assert (false);
            }
        }
        Main.println ("... expect_from exiting.");
    }

    private void try_t() {
        Main.println ("\n==== try_t ====");
        /*
         * live_(i_).
         * live_(you_).
         * good_(Person) :- live_(Person).
         * goal(Person):-good_(Person).
         * 
                * live_ i_ .
                * live_ you_ .
                * good_ Person
                * if
                *   live_ Person .
                *
                * goal Person
                * if
                *   good_ Person .

         */
        Main.println ("\n try_t: Construct data structures for try_t() case and ...");

        String expected[] = {"I"
                            ,"you"
                            };

        Term vPerson = v_("person");

        LinkedList<Clause> llc = new LinkedList<Clause>();

        for (String s : expected) {
             Clause cl = Clause.f__("live_", l_(c_(s)));
             llc.add (cl);
             Main.println ("try_t: Adding clause: ");
             Main.println ("try_t: " + cl.toString());
             Main.println ("");
            llc.add (Clause.f__("live_", c_(s)));
        }

        // llc.add (Clause.f__("good_", vPerson).if__(s_("live_", vPerson)));
        // llc.add (Clause.f__("goal",  vPerson).if__(s_("good_", vPerson)));
        llc.add (Clause.f__("goal",  vPerson).if__(s_("live_", vPerson)));

        try_it (llc, expected);
    }

    private void try_it(LinkedList<Clause> llc, String[] whats_expected) {

        assert !llc.isEmpty();
        Main.println ("  try_it() entering....");
        String s = "[";
        if (whats_expected == null) s += "<null>";
        else for (int i = 0; i < whats_expected.length; ++i) s += whats_expected[i];
        s += "]";
        Main.println ("     whats_expected = " + s);

        String x_out;
        Term.reset_gensym();
        Term.set_TarauLog();

        Main.println ("   ===== try_it: before flatten ========");

        x_out = "";
        for (Clause cl : llc) {
            x_out += cl.toString()+"\n";
            Main.println ("Pulling out body list:");
            for (Term t = cl.body; t != null; t = t.next)
                Main.println ("               .... " + t);
        }

        Main.println (x_out);

        Main.println ("   ===== try_it: flatten transform =======");
        x_out = "";
        for (Clause cl : llc) {
            cl.flatten();
            x_out += cl.toString()+"\n";
        }

        Main.println (x_out);

        Main.println ("   ===== try_it: Calling new Prog: ===============");
        Prog P = new Prog(x_out, false);
        // expect_from(P, whats_expected);
        P.run();
        Main.println ("  exiting try_it()");
    }

    private void try_simple() {
        Main.println ("try_simple() entering....");

        LinkedList<Clause> llc = new LinkedList<Clause>();

    // dookie([]).
        Clause cl_nil = Clause.f__("dookie", l_());
        Main.println ("try_simple: cl_nil = " + cl_nil);
        llc.add(cl_nil);

    // dookie(0).
        Clause cl0 = Clause.f__("dookie", c_("0"));
        llc.add(cl0);
        Main.println ("try_simple: cl0 = " + cl0);

        Term Foo = v_("Foo");

    // goal(Foo):-dookie(Foo).
        Clause cl1 = Clause.f__("goal", Foo).if__(s_("dookie", Foo));
        assert cl1.body != null;
        Main.println ("try_simple: cl1 = " + cl1);
        llc.add(cl1);

        try_it (llc, null);
    }

    private void try_big() {  // based on Tarau's original progs/big.pl
        Main.println ("\n===== try_big() entering ....");

        LinkedList<Clause> llc = new LinkedList<Clause>();
        String expected[] = { "[a,b,c]" };

        Term Ys = v_("Ys");  Term X = v_("X");  Term Xs = v_("Xs");  Term Zs = v_("Zs");
    // append([],Ys,Ys).
        llc.add (Clause.f__("append", l_(), Ys, Ys));


        llc.add (Clause.f__("append", l_(X, Xs), Ys, l_(X,Zs)).
                            if__(s_("append",Xs,Ys,Zs)));
    // nrev([],[]).
        llc.add (Clause.f__("nrev", l_(), l_()));
    // nrev([X|Xs],Zs):-nrev(Xs,Ys),append(Ys,[X],Zs).
        llc.add (Clause.f__("nrev", l_(X,Xs),Zs).
                            if__(s_("nrev",Xs,Ys),
                                 s_("append",Ys,l_(X),Zs)
                            ));
    
        for (Integer i = 0; i < 18; ++i) {
            Term it = c_(i.toString());
            Integer i_next = i + 1;
            Term it_next = c_(i_next.toString());
            llc.add(Clause.f__ ("next_number_after", it, it_next));
        }

    // dup(0,X,X).
        llc.add(Clause.f__("dup", c_("0"), X,X));

        Term R = v_("R");  Term XX = v_("XX");  Term N = v_("N");  Term N1 = v_("N1");
    // dup(N,X,R):-next_number_after(N1,N),append(X,X,XX),dup(N1,XX,R).
        llc.add(Clause.f__("dup", N,X,R).
                            if__(s_("next_number_after", N1, N),
                                 s_("append",X,X,XX),
                                 s_("dup",N1,XX,R)));

        Term Y = v_("Y");

    // goal([X,Y]):-dup(18,[a,b,c,d],[X,Y|_]).
        Term l_a_b_c_d = l_(c_("a"),c_("b"),c_("c"),c_("d"));
        llc.add (Clause.f__("goal", l_(X,Y)).
                            if__(s_("dup", c_("18"), l_a_b_c_d, l_(X,l_(Y,v_("_"))))));

        try_it (llc, expected);

        Main.println ("... try_big() exiting.");
    }

    private void try_bar() {

        Main.println ("\n==== try_bar (list composition with | symbol) ====");


        LinkedList<Clause> llc = new LinkedList<Clause>();

        Term X  = v_("X");  Term Y  = v_("Y"); Term F = v_("F");
        
    // eq(X,X).
        Clause eqXX = Clause.f__("eq",X,X);
        llc.add(eqXX);
        llc.add(Clause.f__("foo", p_(X,Y)).
            if__(s_("eq", X, c_("1")),
                 s_("eq", Y, c_("nil")))
        );
        llc.add(Clause.f__("foo", p_(X,Y)).
        if__(s_("eq", X, c_("2")),
             s_("eq", Y, c_("3"))
             )
        );
        llc.add(Clause.f__("foo", l_(X,Y)).
        if__(s_("eq", X, c_("2")),
             s_("eq", Y, c_("3"))
             )
        );
        llc.add(Clause.f__("goal", F).
            if__(s_("foo", F)));


        Main.println ("\n==== try_bar calling try_it .... ====");
        try_it (llc, null);
        

        Main.println ("\n==== try_bar exiting .... ====");
    }

    private void try_perms() {

        LinkedList<Clause> llc = new LinkedList<Clause>();

        Term X  = v_("X");  Term Y  = v_("Y");
        Term Xs = v_("Xs"); Term Ys = v_("Ys");
        
    // eq(X,X).
        Clause eqXX = Clause.f__("eq",X,X);
        llc.add(eqXX);

    // sel(X,[X|Xs],Xs).
        llc.add (Clause.f__("sel", X, p_(X,Xs), Xs));

        // sel(X,[Y|Xs],[Y|Ys]):-sel(X,Xs,Ys).
        llc.add(Clause.f__("sel", X, p_(Y,Xs), p_(Y,Ys)).
            if__(s_("sel", X, Xs, Ys)));
        
        // To be continued ....
        // First I need to make sure I can do p_(Something, SomethingElse)
        
    }

    private void try_t_J() {
        Main.println ("\n==== try_t_J ====");
        
        Term.reset_gensym();
        Term.set_TarauLog();

        Main.println ("\n try_t_J: Construct data structures for try_t_J() case and ...");

        String expected[] = {"私", "あなた"};
        Term vPerson = v_("人");
        LinkedList<Clause> llc = new LinkedList<Clause>();
        for (String s : expected)
            llc.add (Clause.f__("いきる", c_(s)));
        llc.add (Clause.f__("いいです", vPerson).if__(s_("いきる", vPerson)));
        llc.add (Clause.f__("goal",  vPerson).if__(s_("いいです", vPerson)));
        String x_out = "";
        for (Clause cl : llc)  x_out += cl.toString()+"\n";
        Main.println (x_out);

        Prog P = new Prog(x_out, false);

        Term.set_Prolog();
        /*
        Main.println ("\n=== try_t_J: Pretty-print Prolog from it ===");
        P.ppCode();
        */

        // expect_from(P, expected);
        expect_from(P,null);
    }

    private void list_test() {
        Main.println ("============ list_test entered...");

        Term.reset_gensym();
        Term.set_TarauLog();

        Term c0 = c_("0");
        Term c1 = c_("1");
        Term c2 = c_("2");
        Term l = l_(c0,c1,c2);
        assert l != null;
        assert l.is_a_termlist();

        Main.println ("l = " + l);

        Main.println ("\n list_test: Construct data structures for try_t() case and ...");

        String expected[] = {"list(0,1,2)", "[1|0]"};

        LinkedList<Clause> llc = new LinkedList<Clause>();

        Term V = v_("V");

        Clause x;
        
        x = Clause.f__("zero_and_one", l);
        x.flatten();
        llc.add (x);

        x = Clause.f__("zero_and_one", l_(c1,c0));
        x.flatten();
        llc.add (x);

        x = Clause.f__("goal",  V).if__(s_("zero_and_one", V));
        x.flatten();
        llc.add (x);

        String x_out = "";
        for (Clause cl : llc)  x_out += cl.toString()+"\n";
        Main.println (x_out);

        Prog P = new Prog(x_out, false);

        Term.set_Prolog();
        /*
        Main.println ("\n=== list_test: 'Pretty-print' Prolog from it ===");
        P.ppCode();
        */

        expect_from(P, expected);

        Main.println ("list_test exited...");       
    }

    private void try_add() {
        Main.println ("\n==== try_add() ====");
        /*
        the_sum_of(0,X,X).
        the_sum_of(the_successor_of(X),Y,the_successor_of(Z)):-the_sum_of(X,Y,Z).
        goal(R):-
         the_sum_of(the_successor_of(the_successor_of(0)),the_successor_of(the_successor_of(0)),R).

            the_sum_of 0 X X .

            the_sum_of _0 Y _1 and
              _0 holds the_successor_of X and
              _1 holds the_successor_of Z
            if
            the_sum_of X Y Z .

            goal R
            if
              the_sum_of _0 _1 R and
              _0 holds the_successor_of _2 and
              _2 holds the_successor_of 0 and
              _1 holds the_successor_of _3 and
              _3 holds the_successor_of 0 .
         */

        // out = "";
        Term.reset_gensym();

        LinkedList<Clause> llc = new LinkedList<Clause>();
        Term vX = v_("X");  Term vY = v_("Y"); Term vZ = v_("Z"); Term c0 = c_("0");
        Term succ_X = s_("the_successor_of", vX);
        Term succ_Z = s_("the_successor_of", vZ);
        Term succ_0 = s_("the_successor_of", c0);
        Term vR = v_("R");

        llc.add(Clause.f__("the_sum_of", c0, vX, vX));
        llc.add(Clause.f__("the_sum_of", succ_X,vY,succ_Z).if__(s_("the_sum_of",vX,vY,vZ)));        

        Term two = s_("the_successor_of", succ_0);
        llc.add(Clause.f__("goal", vR).if__(s_("the_sum_of", two, two, vR)));

        Main.println ("----- try_add: Calling new Prog: --------");
        String[] these_answers = {
            "the_successor_of(the_successor_of(the_successor_of(the_successor_of(0))))"
        };
        try_it(llc,these_answers);
        Main.println ("...exiting try_add");
    }

    private void test_gensym() {
        String gs = Term.gensym();
        assert gs.compareTo("_0") == 0;
        String gs1 = Term.gensym();
        assert gs1.compareTo("_1") == 0;
        Term.reset_gensym();
    }

    private Term check_flattening (Term x, Term expected[]) {

        Term.reset_gensym();
        Term r = x.flatten();
        Term.reset_gensym();

        // if (expected != null)
        //    assert r.size() == expected.length;

        if (expected != null) {
            // assert expected.length == r.size();
            int i = 0;
            for (Term t = r; t != null; t = t.next) {
                Main.println ("Comparing t <<<"+t+">>> to expected["+i+"] <<<"+expected[i]+">>>");
                assert t.is_same_as (expected[i]);
                ++i;
            }
        }

        return r;
    }

    private void test_is_same_as() {
        Term a = c_("a");
        Term a1 = c_("a");

        assert a1.is_same_as(a);
        assert a.is_same_as(a1);

        Term v = v_("X");
        Term v1 = v_("X");
        Term v2 = v_("Y");
        assert (v.is_same_as (v1));
        assert (v1.is_same_as(v));
        assert (!v1.is_same_as(v2));
        assert (!v2.is_same_as(v));

        Term l = l_(a,a1);
        Term l1 = l_(a,a1);
        Term l2 = l_(a,v);
        Main.println ("\n ---------> l is <<<" + l + ">>>, l1 = <<<" + l1 + ">>>");
        assert l.is_same_as(l1);
        Main.println ("\n ---------> l2 is <<<"+l2+" l1 is <<<"+l1+">>>");
        assert l2.is_same_as(l1);

        Term s = s_("foo");
        Term s1 = s_("foo");
        Term s2 = s_("bar");
        Term s3 = s_("bar", s);
        Term s4 = s_("bar", s_("foo"));
        assert s.is_same_as (s1);
        assert !s1.is_same_as (s2);
        assert !s3.is_same_as (s2);
        assert s3.is_same_as (s4);
    }

    private void test_flatten() {
        Main.println ("\n-----====< test_flatten entered >====-----");

        test_is_same_as();
 
        Term a = c_("a");
        Term glom_a = s_("glom", a);
        Term exp0[] = {glom_a};
        check_flattening (glom_a, exp0);

        Term.reset_gensym();
        Term v_0 = v_(Term.gensym());
        Term v_1 = v_(Term.gensym());
        Term v_2 = v_(Term.gensym());

        // quux(blah(a)).
        Term blah_a = s_("blah", a);
        Term quux_blah_a = s_("quux", blah_a);
        Term exp1[] = { s_("quux", v_0), e_(v_0, blah_a) };
        check_flattening (quux_blah_a, exp1);

        // whiz([a,a]).
        Term l_a_a =  l_(c_("a"), c_("a"));
        Term whiz_a_a = s_("whiz", l_a_a);
        Term exp2[] = { s_("whiz", v_0), e_(v_0, l_a_a) };
        check_flattening (whiz_a_a, exp2);

        // foo(bar(X),r).
        Term bar_X = s_("bar", v_("X"));
        Term foo_bar_X_r = s_("foo", bar_X, c_("r"));
        Term exp3[] = { s_("foo", v_0, c_("r")), e_(v_0,bar_X) };
        check_flattening (foo_bar_X_r, exp3);

       // foo(bar(X),r):-glom(X),quux(blah(X)),whiz([a,a]).
        Clause cl = Clause.f__("foo", bar_X, c_("r")).
                                if__(s_("glom", v_("X")),
                                     s_("quux", s_("blah", v_("X"))),
                                     s_("whiz", l_(c_("a"), c_("a"))));

        check_flattening (cl.head, exp3);

        // not really checking anything yet
        for (Term t = cl.body; t != null; t = t.next) {
            check_flattening (t, null);
        }
        
        Main.println ("==== moo ===============================================");
        Term nnn = s_("moo", l_(l_(l_(c_("a")))));

        Term exp5[] = { s_("moo", v_0), e_(v_0,l_(v_1)), e_(v_1,l_(v_2)), e_(v_2,l_(c_("a"))) };

        check_flattening(nnn, exp5);

        Term mmm = s_("goo", s_("x", s_("y", c_("a"))));
        Term exp6[] = { s_("goo", v_0), e_(v_0, s_("x", v_1)), e_(v_1,s_("y",a)) };
        check_flattening(mmm, exp6);

        Main.println ("\n-----====< test_flatten exiting... >====-----\n");
    }

    @Test
    public void mainTest() {
        Main.println ("************* Start Term test ********************");

        Term tt = s_("a",c_("b"));
        // Term.CustomIterator it = new Term.CustomIterator(tt);
        for (Term x = tt; x != null; x = x.next)
            { Main.println ("!!!!!!!!!!! x = " + x + "!!!!!!!!!!!!!!"); }

        test_gensym();
/*
        Object A[] = {};
        String mmm = Prog.make_string_from(A);
        Main.println ("mmm = " + mmm);
 */
        String var_X = "X";
        Term vX = v_(var_X);
        assert vX.is_a_variable();

        String const_ooh = "ooh";
        Term cOoh = c_(const_ooh);
        assert cOoh.is_a_constant();

        String cmpnd_fnctr = "compound";
        Term C = s_(cmpnd_fnctr, vX,cOoh);
        assert C != null;
        assert C.is_a_compound();

        Term L = l_(vX,cOoh);
        assert L != null;
        assert L.is_a_termlist();
/*
        try_simple();
        list_test();
        test_flatten();
        try_t();
        try_add();
        try_big();
*/
        try_bar();  // so basic, should be earlier
/*
        try_perms();
        try_t_J();
*/
      
        Main.println ("\n======== End Term test ====================");
    }
}
