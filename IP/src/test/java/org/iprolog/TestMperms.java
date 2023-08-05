package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestMperms extends TestTerm {

    private Term G()     {  return v_(m_());     }
    private Term Gs()    {  return v_(m_());    }
    private Term Bs()    {  return v_(m_());    }
    private Term Tail()  {  return v_(m_());  }

    private Term sel(Term a, Term b, Term c) { return s_(m_(), a, b, c); }
    private Term perm(Term x, Term y)        { return s_(m_(), x, y);    }
    private Term input(Term x, Term y)       { return s_(m_(), x, y);    }
    private Term metaint(Term x)             { return s_(m_(), x);       }
    private Term cls(Term x, Term tail)      { return s_(m_(), x, tail); }

    @Test
    public void mainTest() {

            start_new_test();

            // metaint([]).
            say_(metaint(l_()));
            //            metaint([  G|  Gs   ]):-    cls([  G|  Bs  ], Gs  ), metaint(Bs  ).
            say_(metaint(p_(G(), Gs()))).if_(cls(p_(G(), Bs()), Gs()), metaint(Bs()));

            //            cls([  sel(X,   [  X|  Xs  ], Xs  ) | Tail  ], Tail  ) .
            say_(cls(l_(sel(X(), p_(X(), Xs()), Xs()), Tail()), Tail()));

            //            cls([  sel(X,   [  Y|  Xs  ], [  Y|  Ys  ]),    sel(X,  Xs,  Ys  ) |  Tail   ], Tail   ) .
            say_(cls(l_(sel(X(), p_(Y(), Xs()), p_(Y(), Ys())), p_(sel(X(), Xs(), Ys()), Tail())), Tail()));

            //            cls([  perm([  ],[  ]) | Tail  ], Tail   ) .
            say_(cls(p_(perm(l_(), l_()), Tail()), Tail()));

            //            cls([  perm([     X  |  Xs ], Zs  ), perm(Xs,  Ys  ),    sel(X,  Zs,  Ys  ) | Tail   ], Tail  ) .
            //      say_( cls(l_(perm(l_(p_(X(),Xs())), Zs()), perm(Xs(),Ys()), p_(sel(X(),Zs(),Ys()) , Tail())), Tail()) );

            //      cls([  perm([  X  |Xs  ], Zs  ),    perm(Xs,  Ys  ),    sel(X,  Zs,  Ys  ) | Tail     ], Tail  ) .
            say_(cls(p_(perm(p_(X(), Xs()), Zs()), p_(perm(Xs(), Ys()), p_(sel(X(), Zs(), Ys()), Tail()))), Tail()));
/*
            say_( input (   p_( c_("1"), p_(c_("2"), p_(c_("3"), p_(c_("4"), c_("5") )))) ,
                            p_( c_("5"), p_(c_("4"), p_(c_("3"), p_(c_("2"), c_("1") )))) )
            );
*/
            say_(input(l_(c_("1"), c_("2")),
                    l_(c_("2"), c_("1")))
            );
            // goal(Y):-input(X,Y),metaint([perm(X,Y),perm(Y,X)]).
            Term lll = metaint(l_(perm(X(), Y()), perm(Y(), X())));
            Main.println("lll = " + lll);
            lll.flatten();
            Main.println("lll after flatten = " + lll);
            assert lll.next != null;
            Main.println("lll.next = " + lll.next);

            say_(goal(Y())).
                    if_(input(X(), Y()),
                            // metaint(l_(perm(X(),Y()),perm(Y(),X())))     // originally
                            // metaint (l_( c1(),c2()))
                            metaint(l_(perm(X(), Y()), c2()))   // looks OK, generates "_0 lists _1 2"
                            // metaint (l_( perm(X(),Y()),perm(Y(),X())))  // BAD: generates _0 holds list _1 _2
                    );

            String expected[] = {"[5,4,3,2,1]"};
            // try_it(said, expected);
            try_it(said, null);
    }
}
