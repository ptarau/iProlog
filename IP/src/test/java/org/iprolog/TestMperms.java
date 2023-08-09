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
            //   metaint([  G|  Gs   ]):-    cls([  G  |  Bs ], Gs  ), metaint(Bs  ).
            say_(metaint(p_(G(), Gs()))).if_(cls(p_(G(), Bs()), Gs()), metaint(Bs()));

            //   cls([  sel(X,   [  X|  Xs   ], Xs  )| Tail  ], Tail  ) .
            say_(cls(p_(sel(X(), p_(X(), Xs()), Xs()), Tail()), Tail()));

            //   cls([  sel(X,   [  Y  | Xs  ], [  Y|  Ys   ]), sel(X,   Xs,   Ys  )| Tail  ], Tail  ).
            say_(cls(p_(sel(X(), p_(Y(), Xs()), p_(Y(), Ys())), sel(X(), Xs(), Ys()), Tail()), Tail()));

            //   cls([  perm([  ],[  ] )| Tail  ], Tail  ) .
            say_(cls(p_(perm(l_(), l_()), Tail()), Tail()));

            //   cls([  perm([  X  |Xs  ], Zs  ), perm(Xs,  Ys   ), sel(X,  Zs,  Ys  )| Tail  ], Tail  ) .
            say_(cls(p_(perm(p_(X(),Xs()), Zs()), perm(Xs(), Ys()), sel(X(),Zs(),Ys()), Tail()), Tail()) );
/*
            say_( input (   p_( c_("1"), p_(c_("2"), p_(c_("3"), p_(c_("4"), c_("5") )))) ,
                            p_( c_("5"), p_(c_("4"), p_(c_("3"), p_(c_("2"), c_("1") )))) )
            );
*/
            say_(input(l_(c1(), c2()),
                       l_(c2(), c1()))
            );

            say_(goal(Y())).
                    if_(    input(X(), Y()),
                            metaint(l_(perm(X(), Y()), perm(Y(),X())))

                    );

            String expected[] = {"[2,1]"};

            try_it(said, expected);
    }
}
