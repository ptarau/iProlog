package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestMperms extends TestTerm {

    LPvar G,Gs,Bs,Tail;
    LPvar X,Xs,Y,Ys,Z,Zs;

    LPvar  sel(LPvar a, LPvar b, LPvar c) { return S_(a, b, c); }
    LPvar  perm(LPvar x, LPvar y)         { return S_(x, y);    }
    LPvar  input(LPvar x, LPvar y)        { return S_(x, y);    }
    LPvar  metaint(LPvar x)               { return S_(x);       }
    LPvar  cls(LPvar x, LPvar tail)       { return S_(x, tail); }

    @Test
    public void mainTest() {

        start_new_test();

        say_(metaint(L_()));
        say_(metaint(P_(G, Gs))).if_(cls(P_(G, Bs), Gs), metaint(Bs));
        say_(cls(P_(sel(X, P_(X, Xs), Xs), Tail), Tail));
        say_(cls(P_(sel(X, P_(Y, Xs), P_(Y, Ys)), sel(X, Xs, Ys), Tail), Tail));
        say_(cls(P_(perm(L_(), L_()), Tail), Tail));
        say_(cls(P_(perm(P_(X,Xs), Zs), perm(Xs, Ys), sel(X,Zs,Ys), Tail), Tail) );

        say_( input (   L_( C_("1"), C_("2"), C_("3"), C_("4"), C_("5") ) ,
                        L_( C_("5"), C_("4"), C_("3"), C_("2"), C_("1") ) )
        );

        say_(goal(Y())).
                if_(    input(X, Y),
                        metaint(L_(perm(X, Y), perm(Y,X)))

                );

        String expected[] = { "[5,4,3,2,1]" };

        try_it(said, expected);
    }
}
