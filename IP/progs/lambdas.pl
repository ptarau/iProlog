% generates all lambda terms of size 9
% generates all lambda terms of size 3

genLambda(X,Vs,N,N):-memb(X,Vs).
genLambda(l(X,A),Vs,s(N1),N2):-genLambda(A,[X|Vs],N1,N2).
genLambda(a(A,B),Vs,s(N1),N3):-genLambda(A,Vs,N1,N2),genLambda(B,Vs,N2,N3).

memb(X,[X|_]).
memb(X,[_|Xs]):-memb(X,Xs).

genClosedLambdaTerm(L,T):-genLambda(T,[],L,zero).

some(s(s(s(s(s(s(s(s(zero))))))))).
% some(s(s(s(s(s(s(s(s(zero))))))))).

goal(Lam):-some(Size),genClosedLambdaTerm(Size,Lam).

