package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestBig extends TestTerm {

    LPvar append(LPvar this_, LPvar that, LPvar result) {
                                              return S_(this_, that, result);   }
    LPvar nrev(LPvar x, LPvar y)            { return S_(x, y);                  }
    LPvar dup(LPvar a, LPvar b, LPvar c)    { return S_(a, b, c);               }
    LPvar next_number_after(LPvar a, LPvar a_plus_1) {
                                              return S_(a, a_plus_1);           }
    LPvar goal(LPvar x)                     { return S_(x);                     }

    LPvar R,XX,N,N1;
    LPvar X,Xs,Y,Ys,Z,Zs;
    LPvar _;

    @Test
    public void mainTest() {
        start_new_test();
        Integer hardness_level =
                4
                // 18
        ;
        LPvar difficulty = C_(hardness_level.toString());

        LPvar a = C_("a");
        LPvar b = C_("b");
        LPvar c = C_("c");
        LPvar d = C_("d");
        LPvar zero = C_("0");

        say_(append(L_(), Ys, Ys));
        say_(append(P_(X, Xs), Ys, P_(X, Zs))).
                if_(    append(Xs, Ys, Zs) );

        say_(nrev(L_(), L_()));
        say_(nrev(P_(X,Xs), Zs)).
                if_(    nrev(Xs, Ys),
                        append(Ys, L_(X), Zs) );

        for (Integer i = 0; i < hardness_level; ++i) {
            Integer i_next = i + 1;
            say_(next_number_after(C_(i.toString()), C_(i_next.toString())));
        }
        say_(dup(zero, X, X));

        say_(dup(N, X, R)).
                if_(    next_number_after(N1, N),
                        append(X, X, XX),
                        dup(N1, XX, R) );

        say_(goal(L_(X, Y)))
                .if_(   dup(difficulty, L_(a, b, c, d), P_(X, Y, _)) );

        String[] expected = {"[a,b]"};
        try_it(said, expected);
    }
}
