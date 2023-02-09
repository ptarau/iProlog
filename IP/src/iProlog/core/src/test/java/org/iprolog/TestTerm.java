package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;


import java.util.LinkedList;

public class TestTerm {

    String out = "";

    private void emit_as_fact(Term t) {
        assert t != null;
        out += t.as_fact() + "\n";
        System.out.println (t.as_fact());
    }
    private void emit_as_head(Term t) {
        assert t != null;
        out += t.as_head() + "\n";
        System.out.println (t.as_head());
    }
    private void emit_as_head(LinkedList<Term> eqns) {
        int n = eqns.size();
        for (Term t : eqns) {
            if (--n == 0)
                emit_as_head (t);
            else
                emit_as_expr (t, Term.and_op);
        }
    }
    private void emit_as_body(LinkedList<Term> eqns) {
        int n = eqns.size();
        for (Term t : eqns) {
            if (--n == 0)
                emit_as_fact (t);
            else
                emit_as_expr (t, Term.and_op);
        }
    }
    private void emit_as_expr(Term t, String ended_with) {
        assert t != null;
        out += t.as_expr(ended_with) + "\n";
        System.out.println (t.as_expr(ended_with));
    }

    private void try_t() {
        System.out.println ("\n==== try_t ====");
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
        out = "";
        Term.reset_gensym();

        Term live_of_i_   = Term.compound("live_", Term.variable("i_"));
        Term live_of_you_ = Term.compound("live_", Term.variable("you_"));
        Term good_Person  = Term.compound("good_", Term.variable("Person"));
        Term live_Person  = Term.compound("live_", Term.variable("Person"));
        Term goal_Person  = Term.compound("goal", Term.variable("Person"));

        Term.set_TarauLog();
        
        emit_as_fact (live_of_i_);
        emit_as_fact (live_of_you_);
        emit_as_head (good_Person);
        emit_as_expr (live_Person, Term.clause_end);
        emit_as_head (goal_Person);
        emit_as_expr (good_Person, Term.clause_end);

        Prog P = new Prog(out, false);

        P.ppCode();

        System.out.println ("\n===<<< Starting to run >>>===");
        
        P.run();
    }

    private void try_add() {
        System.out.println ("\n==== try_add ====");
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

        out = "";
        Term.reset_gensym();
        LinkedList<Term> args_0_X_X = new LinkedList<Term>();
        Term c0 = Term.constant("0");
        args_0_X_X.add (c0);
        Term vX = Term.variable("X");
        args_0_X_X.add (vX);
        args_0_X_X.add (vX);

        Term the_sum_of_0_X_X = Term.compound("the_sum_of",args_0_X_X);

        emit_as_fact(the_sum_of_0_X_X);

        // Build this then flatten it:
        //  the_sum_of(the_successor_of(X),Y,the_successor_of(Z))
        Term vY = Term.variable("Y");
        Term vZ = Term.variable("Z");
        Term succ_X = Term.compound("the_successor_of", vX);
        Term succ_Z = Term.compound("the_successor_of", vZ);
        LinkedList<Term> args_list = new LinkedList<>();
        args_list.add (succ_X);
        args_list.add (vY);
        args_list.add (succ_Z);
        Term hd_start = Term.compound("the_sum_of",args_list);
        System.out.println ("hd_start = "+ hd_start);

        System.out.println ("------");
        LinkedList<Term> ll = hd_start.flatten();
        emit_as_head(ll);
        Term body = Term.compound("the_sum_of");
        body.takes_this (vX);
        body.takes_this (vY);
        body.takes_this (vZ);
        emit_as_fact(body);
        System.out.println ("------");

        Term vR = Term.variable("R");
        Term goal = Term.compound("goal");
        goal.takes_this(vR);
        emit_as_head (goal);

        // the_sum_of(the_successor_of(the_successor_of(0)),the_successor_of(the_successor_of(0)),R)
        Term the_sum_of = Term.compound("the_sum_of");
        Term s_of_0 = Term.compound("the_successor_of", c0);
        Term s_of_s_of_0 = Term.compound("the_successor_of", s_of_0);
        the_sum_of.takes_this (s_of_s_of_0);
        the_sum_of.takes_this (s_of_s_of_0);
        the_sum_of.takes_this (vR);
        // emit_as_expr (the_sum_of, Term.clause_end);

        // System.out.println ("... and flattening that:");
        Term.reset_gensym();

        LinkedList<Term> add_s_s_0_s_s_0_R = the_sum_of.flatten();

          ////////////
         /// should be "emit_as_body" I guess:
        ////////////

        emit_as_body(add_s_s_0_s_s_0_R);

        // comes out with different gensym sequencing
        // maybe because of DFS rather than BFS?

        System.out.println ("===<<< as a single String >>>===");
        System.out.println (out);

        Prog P = new Prog(out, false);

        P.ppCode();

        System.out.println ("\n===<<< Starting to run >>>===");
        
        P.run();
        
        Term.set_TarauLog();  // His "assembly language" for virtual machine
    }

    @Test
    public void mainTest() {
        System.out.println ("Start Term test");
      
        String gs = Term.gensym();
        assert gs.compareTo("_0") == 0;
        String gs1 = Term.gensym();
        assert gs1.compareTo("_1") == 0;
        Term.reset_gensym();

        String var_s = "X";
        String const_s = "ooh";
        String cmpnd_fnctr = "compound";
        String cmpnd_fnctr1 = "compendium";

        Term v = Term.variable(var_s);
        assert v.is_a_variable();

        Term c = Term.constant(const_s);
        assert c.is_a_constant();

        LinkedList<Term> tl = new LinkedList<Term>();
        tl.add(v);
        tl.add(c);
        Term C = Term.compound(cmpnd_fnctr, tl);
        assert C.is_a_compound();

        // System.out.println ("C=" + C);
        String CC = cmpnd_fnctr + "(" + var_s + "," + const_s + ")";
        assert C.toString().compareTo(CC.toString()) == 0;

        LinkedList<Term> fv = v.flatten();
        assert fv.size() == 1;
        assert fv.peekFirst().v == v.v;

        LinkedList<Term> fc = c.flatten();
        assert fc.size() == 1;
        assert fc.peekFirst().c == c.c;

        LinkedList<Term> fC = C.flatten();
        assert fC.size() == 1;
        assert fC.peekFirst().c == C.c;
        // System.out.println ("Term C = " + C + ", C.is_flat()=" + C.is_flat());

        Term C1 = Term.compound (cmpnd_fnctr1);
        assert C1.is_a_compound();
        C1.takes_this (C);
        LinkedList<Term> fC1 = C1.flatten();
        // System.out.println ("fC1.size() = " + fC1.size());
        // for (Term t : fC1) System.out.println ("a flattened term t=" + t + ", t.is_flat()=" + t.is_flat());

        Term eqn1 = Term.equation (v,c);
        assert eqn1 != null;

        // System.out.println ("eqn1 = " + eqn1);

        Term v1 = Term.variable("Y");
        assert v1.is_a_variable();

        LinkedList<Term> lC1 = new LinkedList<Term>();
        lC1.add(C1);
        Term C2 = Term.compound("foo",lC1);
        // System.out.println ("\n\n\nC2 = " + C2 + ", C2.is_flat() = " + C2.is_flat());

        Term eqn2 = Term.equation (v1, C2);
        // System.out.println ("eqn2 = " + eqn2 + ", eqn2.is_flat() = " + eqn2.is_flat());

        LinkedList<Term> flatcat = eqn2.flatten();
        // System.out.println ("Flattened: ");
        // for (Term t : flatcat) System.out.println ("  -- " + t);

        System.out.println ("-----------------------------------------");

        String sum = "the_sum_of";
        String s = "the_successor_of";
        Term an_X = Term.variable("X");
        Term a_Y  = Term.variable("Y");
        Term s_of_X = Term.compound (s, an_X);
        Term s_of_Y = Term.compound (s, a_Y);
        LinkedList<Term> lll = new LinkedList<Term>();
        lll.add(s_of_X);
        lll.add(an_X);
        lll.add(s_of_Y);
        Term summer_head = Term.compound(sum,lll);

        System.out.println ("summer_head = "+ summer_head);

        LinkedList<Term> flattery = summer_head.flatten();
        for (Term t : flattery) System.out.println ("flattery has " + t);

        try_t();
        try_add();

        System.out.println ("\n========\nEnd Term test");
    }
}
