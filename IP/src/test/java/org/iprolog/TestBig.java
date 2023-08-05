package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestBig extends TestTerm {

    Term append(Term this_, Term that, Term result) {
        return s_(m_(), this_, that, result);
    }
    Term nrev(Term x, Term y)           { return s_(m_(), x, y); }
    Term dup(Term a, Term b, Term c)    { return s_(m_(), a, b, c); }
    Term next_number_after(Term a, Term a_plus_1) {
        return s_(m_(), a, a_plus_1);
    }

    @Test
    public void mainTest() {
        start_new_test();

        say_(append(l_(), Ys(), Ys()));
        say_(append(p_(X(), Xs()), Ys(), p_(X(), Zs()))).
                if_(append(Xs(), Ys(), Zs()));
        say_(nrev(l_(), l_()));
        say_(nrev(p_(X(),Xs()), Zs())).
                if_(    nrev(Xs(), Ys()),
                        append(Ys(), l_(X()), Zs()));

        for (Integer i = 0; i < 18; ++i) {
            Integer i_next = i + 1;
            say_(next_number_after(c_(i.toString()), c_(i_next.toString())));
        }
        say_(dup(c0(), X(), X()));

        Term R = v_("R");
        Term XX = v_("XX");
        Term N = v_("N");
        Term N1 = v_("N1");
        // dup(N,X,R):-next_number_after(N1,N),append(X,X,XX),dup(N1,XX,R).
        said.add(Clause.f__(dup(N, X(), R)).
                if_(    next_number_after(N1, N),
                        append(X(), X(), XX),
                        dup(N1, XX, R)));

        // goal([X,Y]):-dup(18,[a,b,c,d],[X,Y|_]).
        Term l_a_b_c_d = l_(c_("a"), c_("b"), c_("c"), c_("d"));

        say_(goal(l_(X(), Y())))
                //        .if_(dup(c_("18"), l_a_b_c_d, l_(X(), p_(Y(), _()))));
                //        .if_(dup(c_("18"), l_a_b_c_d, p_(X(), p_(Y(), _()))));
                .if_(dup(c_("18"), l_a_b_c_d, p_(X(), Y(), _())));
        String expected[] = {"[a,b]"};
        try_it(said, expected);
    }
}
