package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestPerms extends TestTerm {
    private Term eq(Term x, Term y) { return s_("eq", x, y); }

    private Term sel(Term a, Term b, Term c) { return s_(m_(), a, b, c); }
    private Term perm(Term x, Term y)        { return s_(m_(), x, y);    }
    private Term app(Term a, Term b, Term c) { return s_(m_(), a, b, c); }
    private Term nrev(Term x, Term y)        { return s_(m_(), x, y);    }
    private Term input(Term x)               { return s_(m_(), x);       }

    @Test
    public void mainTest() {
        start_new_test();
        // int difficulty = 10;
        int difficulty = 2;

        say_( eq(X(), X()) );
        say_( sel(X(), p_(X(), Xs()), Xs()) );
        say_( sel(X(), p_(Y(), Xs()), p_(Y(), Ys()))).
                if_(    sel(X(), Xs(), Ys()) );
        say_( perm(l_(), l_()) );
        say_( perm(p_(X(), Xs()), Zs())).
                if_(    perm(Xs(), Ys()),
                        sel(X(), Zs(), Ys()) );
        say_( app(l_(), Xs(), Xs()) );
        say_( app(p_(X(), Xs()), Ys(), p_(X(), Zs()))).
                if_(app(Xs(), Ys(), Zs()) );
        say_( nrev(l_(), l_()));
        say_( nrev(p_(X(), Xs()), Zs())).
                if_(    nrev(Xs(), Ys()),
                        app(Ys(), l_(X()), Zs()) );
        assert difficulty > 0;
        Term nlist = l_(c1());
        assert nlist.is_a_termlist();
        String output = "1";
        for (Integer i = 2; i <= difficulty; ++i) {
            nlist.takes_this(c_(i.toString()));
            output = i.toString() + "," + output;
        }
        output = "[" + output + "]";

        say_( input(nlist) );
        say_(goal(Y())).
                if_(    input(X()),
                        nrev(X(), Y()),
                        perm(X(), Y()),
                        perm(Y(), X())  );

        // String expected[] = {"[10,9,8,7,6,5,4,3,2,1]"};
        String expected[] = {output};
        try_it(said, expected);
    }
}