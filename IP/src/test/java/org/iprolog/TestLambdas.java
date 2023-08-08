package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestLambdas extends TestTerm {

    Term Vs()           {  return v_(m_());                }
    Term N()            {  return v_(m_());                }
    Term A()            {  return v_(m_());                }
    Term B()            {  return v_(m_());                }
    Term T()            {  return v_(m_());                }
    Term L()            {  return v_(m_());                }
    Term N1()           {  return v_(m_());                }
    Term N2()           {  return v_(m_());                }
    Term N3()           {  return v_(m_());                }
    Term Lam()          {  return v_(m_());                }
    Term Size()         {  return v_(m_());                }

    Term zero()         {  return c_(m_());                }

    Term l(Term a, Term b)        { return s_(m_(), a,b);    }
    Term s(Term a)                { return s_(m_(), a);      }
    Term a(Term x, Term y)        { return s_(m_(), x,y);    }

    Term genLambda(Term a, Term b, Term c, Term d)
    { return s_(m_(),a,b,c,d); }
    Term memb(Term a,Term b)      { return s_(m_(), a,b);    }
    Term genClosedLambdaTerm(Term L, Term T)
    { return s_(m_(), L,T);    }
    Term some(Term x)             { return s_(m_(), x);      }

    Term goal(Term Lam)           { return s_(m_(), Lam);    }

    @Test
    public void mainTest() {
        start_new_test();

        //   genLambda(X  ,Vs,  N  ,N   ) :- memb(X,Vs).
        say_(genLambda(X(),Vs(),N(),N()))
                .if_(   memb(X(),Vs())  );

        //   genLambda(l(X,  A  ), Vs  ,s(N1  ),N2):-genLambda(A,[X|Vs],N1,N2).
        say_(genLambda(l(X(),A()), Vs(),s(N1()),N2()))
                .if_(   genLambda(A(),p_(X(),Vs()),N1(),N2())  );

        //   genLambda(a(A,  B  ), Vs,   s(N1),  N3):-genLambda(A,Vs,N1,N2),genLambda(B,Vs,N2,N3).
        say_(genLambda(a(A(),B()), Vs(), s(N1()),N3()))
                .if_(   genLambda(A(),Vs(),N1(),N2()),
                        genLambda(B(),Vs(),N2(),N3())  );

        // memb(X,[X|_]).
        say_(memb(X(),p_(X(),_())));

        //   memb(X,  [  _|  Xs])  :- memb(X,Xs).
        say_(memb(X(),p_(_(),Xs())))
                .if_(   memb(X(),Xs())  );

        //   genClosedLambdaTerm(L,  T   ) :-genLambda(T,[],L,zero).
        say_(genClosedLambdaTerm(L(),T()))
                .if_(   genLambda(T(),l_(),L(),zero())  );

        // % nine(s(s(s(s(s(s(s(s(s(zero)))))))))).

        //   some((s(s(zero)))).
        say_(some(s(s(zero()))));

        // goal(Lam):-some(Size),genClosedLambdaTerm(Size,Lam).
        say_(goal(Lam()))
                .if_(   some(Size()),
                        genClosedLambdaTerm(Size(),Lam()) );

        String[] these_answers = {
                "l(V134,l(V157,V157))",
                "l(V134,l(V157,V134))",
                "l(V134,a(V134,V134))"
        };
        // try_it(said, these_answers);
        try_it(said, these_answers,false);
    }
}
