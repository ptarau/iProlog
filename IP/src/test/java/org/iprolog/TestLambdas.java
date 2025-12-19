package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestLambdas extends TestTerm {

    LP_ X,Xs,Vs,N,A,B,T,L,N1,N2,N3,Lam,Size;

    TestLambdas() {
        say_(genLambda(X,Vs,N,N))
                .if_(   memb(X,Vs)  );
        say_(genLambda(l(X,A), Vs,s(N1),N2))
                .if_(   genLambda(A,P_(X,Vs),N1,N2)  );
        say_(genLambda(a(A,B), Vs, s(N1),N3))
                .if_(   genLambda(A,Vs,N1,N2),
                        genLambda(B,Vs,N2,N3)  );
        say_(memb(X,P_(X,_)));
        say_(memb(X,P_(_,Xs)))
                .if_(   memb(X,Xs)  );
        say_(genClosedLambdaTerm(L,T))
                .if_(   genLambda(T,L_(),L,zero())  );

        say_(some(s(s(zero()))));

        say_(good_(Lam))
                .if_(   some(Size),
                        genClosedLambdaTerm(Size,Lam) );
    }

    LP_ zero()                { return C_("0");  }

    LP_ l(LP_ a, LP_ b)       { return S_(a,b);     }
    LP_ s(LP_ a)              { return S_(a);       }
    LP_ a(LP_ x, LP_ y)       { return S_(x,y);     }

    LP_ genLambda(LP_ a, LP_ b, LP_ c, LP_ d)
                              { return S_(a,b,c,d); }
    LP_ memb(LP_ a, LP_ b)    { return S_(a,b);     }
    LP_ genClosedLambdaTerm(LP_ L, LP_ T)
                              { return S_(L,T);     }
    LP_ some(LP_ x)           { return S_(x);       }

    LP_ good_(LP_ Lam)        { return S_(Lam);     }

    @Test
    public void mainTest() {
        String[] these_answers = {
                "l(V134,l(V157,V157))",
                "l(V134,l(V157,V134))",
                "l(V134,a(V134,V134))"
        };

        try_it(said, these_answers,false);
    }
}
