package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestMperms extends TestTerm {

    LP_ G,Gs,Bs,Tail;
    LP_ X,Xs,Y,Ys,Z,Zs;

    TestMperms() {
        say_(metaint(L_()));
        say_(metaint(P_(G, Gs))).if_(cls(P_(G, Bs), Gs), metaint(Bs));
        say_(cls(P_(sel(X, P_(X, Xs), Xs), Tail), Tail));
        say_(cls(P_(sel(X, P_(Y, Xs), P_(Y, Ys)), sel(X, Xs, Ys), Tail), Tail));
        say_(cls(P_(perm(L_(), L_()), Tail), Tail));
        say_(cls(P_(perm(P_(X,Xs), Zs), perm(Xs, Ys), sel(X,Zs,Ys), Tail), Tail) );

        say_( input (   L_( C_("1"), C_("2"), C_("3"), C_("4"), C_("5") ) ,
                L_( C_("5"), C_("4"), C_("3"), C_("2"), C_("1") ) ) );

        say_(good_(Y)).
                if_(    input(X, Y),
                        metaint(L_(perm(X, Y), perm(Y,X)))  );
    }

    LP_ sel(LP_ a, LP_ b, LP_ c)   { return S_(a, b, c); }
    LP_ perm(LP_ x, LP_ y)         { return S_(x, y);    }
    LP_ input(LP_ x, LP_ y)        { return S_(x, y);    }
    LP_ metaint(LP_ x)             { return S_(x);       }
    LP_ cls(LP_ x, LP_ tail)       { return S_(x, tail); }

    @Test
    public void mainTest() {
        String expected[] = { "[5,4,3,2,1]" };
        try_it(said, expected);
    }
}
