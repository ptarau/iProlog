package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestPerms extends TestTerm {

    LPvar sel(LPvar a, LPvar b, LPvar c) { return S_(a, b, c); }
    LPvar perm(LPvar x, LPvar y)         { return S_(x, y);    }
    LPvar app(LPvar a, LPvar b, LPvar c) { return S_(a, b, c); }
    LPvar nrev(LPvar x, LPvar y)         { return S_(x, y);    }
    LPvar input(LPvar x)                 { return S_(x);       }

    LPvar eq(LPvar x, LPvar y)           { return S_(x,y);     }
    LPvar X,Xs,Y,Ys,Z,Zs;
    LPvar goal(LPvar x)                  { return S_(x);       }

    @Test
    public void mainTest() {
        start_new_test();

        int difficulty = 2;

        say_( eq(X,X) );
        say_( sel(X, P_(X, Xs), Xs) );
        say_( sel(X, P_(Y, Xs), P_(Y, Ys))).
                if_(    sel(X, Xs, Ys) );
        say_( perm(L_(), L_()) );
        say_( perm(P_(X, Xs), Zs)).
                if_(    perm(Xs, Ys),
                        sel(X, Zs, Ys) );
        say_( app(L_(), Xs, Xs) );
        say_( app(P_(X, Xs), Ys, P_(X, Zs))).
                if_(app(Xs, Ys, Zs) );
        say_( nrev(L_(), L_()));
        say_( nrev(P_(X, Xs), Zs)).
                if_(    nrev(Xs, Ys),
                        app(Ys, L_(X), Zs) );

        assert difficulty > 0;
        Term nlist = l_(c1());
        assert nlist.is_a_termlist();
        String output = "1";
        for (Integer i = 2; i <= difficulty; ++i) {
            nlist.takes_this(c_(i.toString()));
            output = i.toString() + "," + output;
        }
        output = "[" + output + "]";
        String expected[] = {output};
        LPvar nL = new LPvar();
        nL.run = ()->(nlist);
        say_( input(nL) );

        say_(goal(Y)).
                if_(    input(X),
                        nrev(X, Y),
                        perm(X, Y),
                        perm(Y, X)  );

        try_it(said, expected);
    }
}
