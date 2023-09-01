package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestLambdas extends TestTerm {

    LPvar X,Xs,Vs,N,A,B,T,L,N1,N2,N3,Lam,Size;
    LPvar _;

    LPvar zero()         {  return C_("0");                }

    LPvar l(LPvar a, LPvar b)       { return S_(a,b);     }
    LPvar s(LPvar a)                { return S_(a);       }
    LPvar a(LPvar x, LPvar y)       { return S_(x,y);     }

    LPvar genLambda(LPvar a, LPvar b, LPvar c, LPvar d)
                                    { return S_(a,b,c,d); }
    LPvar memb(LPvar a,LPvar b)     { return S_(a,b);     }
    LPvar genClosedLambdaTerm(LPvar L, LPvar T)
                                    { return S_(L,T);     }
    LPvar some(LPvar x)             { return S_(x);       }

    LPvar goal(LPvar Lam)           { return S_(Lam);    }

    @Test
    public void mainTest() {
        start_new_test();

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

        say_(goal(Lam))
                .if_(   some(Size),
                        genClosedLambdaTerm(Size,Lam) );

        String[] these_answers = {
                "l(V134,l(V157,V157))",
                "l(V134,l(V157,V134))",
                "l(V134,a(V134,V134))"
        };

        try_it(said, these_answers,false);
    }
}
