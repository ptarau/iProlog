package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;


import java.util.LinkedList;
import java.util.ArrayList;
import java.util.Arrays;

public class TestTerm {

    static Term v_(String s) {    return Term.variable(s); }
    Term v_()                {    return v_(m_());         }

    static Term _()          {    return v_("_");        }
    static Term c_(String s) {    return Term.constant(s); }
    static Term s_(String s) {    return Term.compound(s); }
    static Term s_(String s, Term... ts) {
                                        Term xt = Term.compound(s); 
                                        for (Term t : ts)
                                            xt = xt.takes_this(t);
                                        return xt;
                                    }
    static Term e_(Term lhs, Term rhs) { return Term.equation (lhs,rhs); }
    static Term l_(Term... ts) {
                                        if (ts.length == 0)
                                            return c_("nil");
                                        return Term.termlist(ts);
                                }
    static Term pal_(Term[] tal, int i) {
        if (i == tal.length-2)
            return Term.termpair(tal[i], tal[i+1]);
        return Term.termpair (tal[i], pal_(tal, i+1));
    }
    static Term p_(Term... Ts) { return pal_(Ts, 0);  }

    private Clause yes_ (Term hd) {     return Clause.f__(hd); }

    public String m_() {
        return Thread.currentThread().getStackTrace()[2].getMethodName();
    }
    private String f_() {
        return Thread.currentThread().getStackTrace()[2].getMethodName();
    }

    Term call(Term f, Term... ts) { return s_(f.v(),ts); }
    Term true_(){ return null;     }

    Term c0() { return c_("0"); }
    Term c1() { return c_("1"); }
    Term c2() { return c_("2"); }
    Term c3() { return c_("3"); }
    Term c4() { return c_("4"); }

    Term V()  { return v_(f_()); }
    Term X()  { return v_(f_()); };
    Term Y()  { return v_(f_()); };
    Term Z()  { return v_(f_()); };

    Term Xs() { return v_(f_()); }
    Term Ys() { return v_(f_()); }
    Term Zs() { return v_(f_()); }

    Term goal(Term x)  { return s_(f_(),x); }

    LinkedList<Clause> said;

    protected void start_new_test() {
        if (said == null)
            said = new LinkedList<Clause>();
        else
            said.clear();
        Term.reset_gensym();
        Term.set_TarauLog();
    }

    private Clause say_(Clause cl) {
        assert said != null;
        said.add (cl);
        return cl;
    }
    protected Clause say_(Term hd)   {
        assert said != null;
        Clause cl = yes_(hd);
        said.add (cl);
        return cl;
    }

    /**
   * Initiator and consumer of the stream of answers
   * generated by this engine.
   */
    private void expect_from(Prog P, String[] whats_expected, boolean complete) {
        Object POJO_Ans;

        Term.set_Prolog();
/*
        if (whats_expected != null) {
            Main.println ("whats_expected =");
            for (String s : whats_expected) {
                Main.println ("  Looks like " + s + " when prolog_mode =  " + Term.in_Prolog_mode);
            }
        }
 */
        // Going through the Java Object interface, unfortunately.

        int n_expected = whats_expected.length;
        // Main.println ("n_expected = " + n_expected);

        Boolean yielded_something = false;
        while ((POJO_Ans = P.POJO_ask()) != null) {

            yielded_something = true;

            if (--n_expected < 0) {
                if (!complete)
                    break;
                assert false;
            }

            // Main.println ("\nIn POJO_ask() loop....");
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
                // Prog.println ("  to_compare = " + to_compare);
                // Prog.println ("  goal ["+(i++)+"] = " + t);
            }
 
            String sg = P.showTerm(POJO_goal_answers[0]);
            assert sg.equals("goal");   // because it'll be "[goal, <answer>]"
            assert POJO_goal_answers.length > 1;
            assert POJO_goal_answers.length < 3;
            // Main.println ("POJO_goal_answers[1] is " + POJO_goal_answers[1]);
            String show_POJO_object = P.showTerm(POJO_goal_answers[1]);

            if (whats_expected != null) {
                // Prog.println ("  to_compare = " + to_compare);
                // Prog.println ("  show_POJO_object = " + show_POJO_object);
                assert Arrays.asList(whats_expected).contains(show_POJO_object);
            } else {
                // Main.println (" yielding: " + show_POJO_object);
            }
        }
        // Main.println ("whats_expected: " + whats_expected);
        // Main.println ("yielded_something = " + yielded_something);
        // Main.println ("complete = " + complete);
        assert whats_expected == null || yielded_something;

        // Main.println ("... expect_from exiting.");
    }

    protected void try_it(LinkedList<Clause> said, String[] whats_expected) {
        try_it(said,whats_expected,true);
    }

    protected void try_it(LinkedList<Clause> said, String[] whats_expected, boolean complete) {

        assert !said.isEmpty();
        // Main.println (" ===== try_it() entering....");
        String s = "[";

        if (whats_expected == null)
            s += "<null>";
        else {  String sep = "";
                for (int i = 0; i < whats_expected.length; ++i) {
                    s += sep;
                    sep = ",";
                    s += whats_expected[i];
                }
        }
        s += "]";
        // Main.println ("     whats_expected = " + s);

        String asm_txt;
        Term.reset_gensym();
        Term.set_TarauLog();

        // Main.println ("   ===== try_it(): before flatten =======");
        asm_txt = "";
        for (Clause cl : said) {
            asm_txt += cl.toString()+System.lineSeparator();
        }
        // Main.println ("asm_txt = \n" + asm_txt);

        // Main.println ("   ===== try_it(): flattening transform =======");
        asm_txt = "";
        for (Clause cl : said) {
            cl.flatten();
            asm_txt += cl.toString()+System.lineSeparator();
        }

        // Main.println ("   ===== try_it(): Calling new Prog: ===============");
        Prog P = new Prog(asm_txt, false);

        expect_from(P, whats_expected, complete);

        // Main.println ("  ===== exiting try_it()");
    }

    private class TryT {
        private  Term good_(Term x)    { return s_(m_(), x); }
        private  Term live_(Term x)    { return s_(m_(), x); }
        private  Term Person()         { return v_(m_());    }

        private void test() {
            // Main.println("\n==== TryT.test() entered ... ====");

            start_new_test();

            String expected[] = {"I", "you", "them", "us"};

            for (String s : expected)
                say_(live_(c_(s)));

            say_(good_(Person())).if_(live_(Person()));
            say_(goal(Person())).if_(good_(Person()));

            try_it(said, expected);
        }
    }

private class TrySimple {
    private Term   Foo()            { return  v_(m_());    }
    private Term   dookie(Term x)   { return  s_(m_(), x); }

    private void test() {
        // Main.println(" ======== TrySimple.test() entering....");

        start_new_test();

        say_(dookie(l_()));
        say_(dookie(c0()));
        say_(goal(Foo())).if_(dookie(Foo()));

        String expected[] = {"nil", "0"};
        try_it(said, expected);

        // Main.println (" ======= TrySimple.test() exiting ....");
    }
};

    private class TryBar {
        private Term F()                { return v_(m_());       }
        private Term eq(Term x, Term y) { return s_(m_(), x, y); }
        private Term foo(Term x)        { return s_(m_(), x);    }

        private void test() {

            // Main.println("\n==== TryBar.test() (list composition with | symbol) entered ... ====");

            start_new_test();

            say_(eq( X(), X()));
            say_(foo(p_(X(), Y()))).
                    if_(    eq( X(), c1()),
                            eq( Y(), c_("nil")));
            say_(foo(p_(X(), Y()))).
                    if_(    eq( X(), c2()),
                            eq( Y(), c3()));
            say_(foo(l_(X(), Y()))).
                    if_(    eq( X(), c2()),
                            eq( Y(), c3()));
            say_(goal(F())).
                    if_(    foo(F()));

            String expected[] = {"[1]", "[2|3]", "[2,3]"};
            try_it(said, expected);

            // Main.println("\n==== TryBar.test() exiting .... ====");
        }
    }

private class TryList {

    private Term zero_and_one(Term x) { return s_(m_(), x); }
    private Term metaint(Term x)      { return s_(m_(), x); }
    private Term perm(Term x )        { return s_(m_(), x); }
    private Term dumb2(Term x, Term y) { return s_(m_(), x, y); }
    private Term dumb3(Term x, Term y) { return s_(m_(), x, y); }

    private void flatten_and_show(Term g) {
        // Main.println ("\n     g was " + g);
        g.flatten();
        // Main.println ("     after flatten: ");
        for (Term t = g; t != null; t = t.next) {
            // Main.println ("                elt = " + t);
        }
        Term.reset_gensym();
    }

    private void test() {

        Term.reset_gensym();
        Term.set_TarauLog();

            flatten_and_show(metaint(l_(c2(), perm(X()))));         // BAD: _0 holds list 2 _1

        Term l = l_(c0(), c1(), c2());
        assert l != null;
        assert l.is_a_termlist();

        String expected[] = {"[0,1,2]", "[1|0]"};

        LinkedList<Clause> said = new LinkedList<Clause>();

        Clause x;

        x = Clause.f__(zero_and_one(l));
        x.flatten();
        said.add(x);

        x = Clause.f__(zero_and_one(p_(c1(), c0())));
        x.flatten();
        said.add(x);

        x = Clause.f__(goal(V())).if_(zero_and_one(V()));
        x.flatten();
        said.add(x);

        String asm_txt = "";
        for (Clause cl : said) asm_txt += cl.toString() + System.lineSeparator();
        // Main.println(asm_txt);

        Prog P = new Prog(asm_txt, false);

        Term.set_Prolog();

        expect_from(P, expected, true);

        // Main.println(" ======== TryList.test() exiting...");
    }
}

    private void test_gensym() {
        Term.reset_gensym();
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

        if (expected != null) {
            // assert expected.length == r.size();
            int i = 0;
            for (Term t = r; t != null; t = t.next) {
                // Main.println ("Comparing t <<<"+t+">>> to expected["+i+"] <<<"+expected[i]+">>>");
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
        // Main.println ("\n ---------> l is <<<" + l + ">>>, l1 = <<<" + l1 + ">>>");
        assert l.is_same_as(l1);
        // Main.println ("\n ---------> l2 is <<<"+l2+" l1 is <<<"+l1+">>>");
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

    private class TestFlatten {
        Term a()            { return c_(m_());    }
        Term glom(Term x)   { return s_(m_(), x); }

        private void test() {
            // Main.println("\n-----====< TestFlatten.test entered >====-----");

            test_is_same_as();

            // Term a = c_("a");
            Term glom_a = glom(a());
            Term exp0[] = {glom_a};
            check_flattening(glom_a, exp0);

            Term.reset_gensym();
            Term v_0 = v_(Term.gensym());
            Term v_1 = v_(Term.gensym());
            Term v_2 = v_(Term.gensym());

            // quux(blah(a)).
            // Main.println("quux(blah(a)).");
            Term blah_a = s_("blah", a());
            Term quux_blah_a = s_("quux", blah_a);
            Term exp1[] = {s_("quux", v_0), e_(v_0, blah_a)};
            check_flattening(quux_blah_a, exp1);

            // whiz([a,a]).
            // Main.println ("whiz([a,a]).");
            Term l_a_a = l_(a(), a());
            Term whiz_a_a = s_("whiz", l_a_a);
            Term exp2[] = {s_("whiz", v_0), e_(v_0, l_a_a)};
            check_flattening(whiz_a_a, exp2);

            // foo(bar(X),r).
            // Main.println ("foo(bar(X),r).");
            Term bar_X = s_("bar", v_("X"));
            Term foo_bar_X_r = s_("foo", bar_X, c_("r"));
            Term exp3[] = {s_("foo", v_0, c_("r")), e_(v_0, bar_X)};
            check_flattening(foo_bar_X_r, exp3);

            // foo(bar(X),r):-glom(X),quux(blah(X)),whiz([a,a]).
            // Main.println ("foo(bar(X),r):-glom(X),quux(blah(X)),whiz([a,a]).");
            Clause cl = Clause.f__("foo", bar_X, c_("r")).
                    if_(s_("glom", v_("X")),
                            s_("quux", s_("blah", v_("X"))),
                            s_("whiz", l_(c_("a"), a())));

            check_flattening(cl.head, exp3);

            // not really checking anything yet
            //for (Term t = cl.body; t != null; t = t.next) {
            //    check_flattening (t, null);
            //}
            check_flattening(cl.body, null);

            // Main.println("==== moo ===============================================");

            Term nnn = s_("moo", l_(l_(l_(a()))));

            Term exp5[] = { s_("moo", v_0), e_(v_0,l_(v_1)), e_(v_1,l_(v_2)), e_(v_2,l_(a())) };
            // Term exp5[] = {s_("moo", v_0), e_(v_2, l_(a())), e_(v_1, l_(v_2)), e_(v_0, l_(v_1))};

            check_flattening(nnn, exp5);

            Term mmm = s_("goo", s_("x", s_("y", a())));
            Term exp6[] = {s_("goo", v_0), e_(v_0, s_("x", v_1)), e_(v_1, s_("y", a()))};
            check_flattening(mmm, exp6);

            // Main.println("\n-----====< TestFlatten.test... >====-----\n");
        }
    }

    @Test
    public void mainTest() {
        Main.println ("************* Start Term test ********************");

        Term tt = s_("a",c_("b"));

        test_gensym();

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

        Term xxx = l_(c0(),c1(),c2());
        /*
        Main.println ("xxx = " + xxx);
        for (Term t = xxx; t != null; t = t.next)
            Main.println ("   >>>>>>> " + t);
         */
        Term yyy = p_(xxx,c3());

        new TryBar().test();
        new TrySimple().test();
        new TryList().test();
        new TestFlatten().test();
        new TryT().test();

        Main.println ("\n======== End Term test ====================");
    }
}