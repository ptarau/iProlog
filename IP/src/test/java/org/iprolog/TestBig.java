package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestBig extends TestTerm {

    LP_ append(LP_ this_, LP_ that, LP_ result) {
                                              return S_(this_, that, result);   }
    LP_ nrev(LP_ x, LP_ y)            { return S_(x, y);                  }
    LP_ dup(LP_ a, LP_ b, LP_ c)    { return S_(a, b, c);               }
    LP_ next_number_after(LP_ a, LP_ a_plus_1) {
                                              return S_(a, a_plus_1);           }
    LP_ good_(LP_ x)                     { return S_(x);                     }

    LP_ R,XX,N,N1;
    LP_ X,Xs,Y,Ys,Z,Zs;

    @Test
    public void mainTest() {
        start_new_test();
        Integer hardness_level =
                4
                // 18
        ;
        LP_ difficulty = C_(hardness_level.toString());

        LP_ a = C_("a");
        LP_ b = C_("b");
        LP_ c = C_("c");
        LP_ d = C_("d");
        LP_ zero = C_("0");

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

        say_(good_(L_(X, Y)))
                .if_(   dup(difficulty, L_(a, b, c, d), P_(X, Y, _)) );

        String[] expected = {"[a,b]"};
        try_it(said, expected);
    }
}
