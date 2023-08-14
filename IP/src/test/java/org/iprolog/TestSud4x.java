package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;

// TBD
public class TestSud4x extends TestTerm {

    Term S11() {return v_(m_());}
    Term S12() {return v_(m_());}
    Term S13() {return v_(m_());}
    Term S14() {return v_(m_());}

    Term S21() {return v_(m_());}
    Term S22() {return v_(m_());}
    Term S23() {return v_(m_());}
    Term S24() {return v_(m_());}

    Term S31() {return v_(m_());}
    Term S32() {return v_(m_());}
    Term S33() {return v_(m_());}
    Term S34() {return v_(m_());}

    Term S41() {return v_(m_());}
    Term S42() {return v_(m_());}
    Term S43() {return v_(m_());}
    Term S44() {return v_(m_());}

    Term s4x4(Term a) {return s_(m_(),a); }

    Term sudoku(Term a) {return s_(m_(),a); }
    Term map1x(Term a, Term b, Term c) {return s_(m_(),a,b,c);}
    Term map11(Term a, Term b, Term c) {return s_(m_(),a,b,c);}
    Term permute(Term a, Term b) {return s_(m_(),a,b);}
    Term ins(Term a, Term b, Term c) {return s_(m_(),a,b,c);}
    Term goal(Term a) {return s_(m_(),a); }

    Term Xss()  { return v_(m_()); }
    Term Xsss() { return v_(m_()); }
    Term F()    { return v_(m_()); }

    @Test
    public void mainTest() {
        start_new_test();

        say_(s4x4(l_(
              l_(
                 l_(S11(),S12(), S13(),S14()),
                 l_(S21(),S22(), S23(),S24()),

                 l_(S31(),S32(), S33(),S34()),
                 l_(S41(),S42(), S43(),S44())
              ),
              l_(
                 l_(S11(),S21(), S31(),S41()),
                 l_(S12(),S22(), S32(),S42()),

                 l_(S13(),S23(), S33(),S43()),
                 l_(S14(),S24(), S34(),S44())
              ),
              l_(
                 l_(S11(),S12(), S21(),S22()),
                 l_(S13(),S14(), S23(),S24()),

                 l_(S31(),S32(), S41(),S42()),
                 l_(S33(),S34(), S43(),S44())
              )
        )))
                .if_(true_());
    // :-
    //            true.
    say_(sudoku(Xss()))
            .if_(   s4x4(p_(Xss(),Xsss())),
                    map11(  c_("permute"),
                            l_(c1(),c2(),c3(),c4()),
                            p_(Xss(),Xsss())  )  );

    say_(map1x(_(),_(),l_()));

    //   map1x(F,  Y,  [  X|Xs    ]):- call(F,Y,X),map1x(F,Y,Xs).
    say_(map1x(F(),Y(),p_(X(),Xs())))
            .if_(   call(F(),Y(),X()),map1x(F(),Y(),Xs())
    );

    //   map11(_  ,_  ,[  ]).
    say_(map11(_(),_(),l_()));
    //   map11(F,  X,  [  Y|  Ys  ] ):-map1x(F,X,Y),map11(F,X,Ys).
    say_(map11(F(),X(),p_(Y(),Ys())))
        .if_(   map1x(F(),X(),Y()),
                map11(F(),X(),Ys()) );

    //   permute([ ],[ ]).
    say_(permute(l_(),l_()));
    //   permute([  X  |Xs  ],Zs  ):-permute(Xs,Ys),ins(X,Ys,Zs).
    say_(permute(p_(X(),Xs()),Zs()))
        .if_(   permute(Xs(),Ys()),ins(X(),Ys(),Zs())  );

    say_(ins(X(),Xs(),p_(X(),Xs())));
    //   ins(X,  [  Y  |Xs  ],[  Y  |Ys  ]):-ins(X,Xs,Ys).
    say_(ins(X(),p_(Y(),Xs()),p_(Y(),Ys())))
            .if_(   ins(X(),Xs(),Ys())  );

    say_(goal(Xss())).if_(  sudoku(Xss()));

    String expected[] = {
            "[[1,2,3,4],[3,4,1,2],[2,3,4,1],[4,1,2,3]]",
            "[[1,2,3,4],[3,4,1,2],[2,1,4,3],[4,3,2,1]]",
            "[[1,2,3,4],[3,4,1,2],[4,1,2,3],[2,3,4,1]]",
            "[[1,2,3,4],[3,4,1,2],[4,3,2,1],[2,1,4,3]]",
            "[[1,2,3,4],[3,4,2,1],[2,1,4,3],[4,3,1,2]]"
    };

    try_it(said, expected, false);
    }
}
