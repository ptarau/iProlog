app([],Ys,Ys).
app([X|Xs],Ys,[X|Zs]):-app(Xs,Ys,Zs).

nrev([],[]).
nrev([X|Xs],Zs):-nrev(Xs,Ys),app(Ys,[X],Zs).

s(0,1).
s(1,2).
s(2,3).
s(3,4).
s(4,5).
s(5,6).
s(6,7).
s(7,8).
s(8,9).
s(9,10).
s(10,11).
s(11,12).
s(12,13).
s(13,14).
s(14,15).
s(15,16).
s(16,17).
s(17,18).

dup(0,X,X).
dup(N,X,R):-s(N1,N),app(X,X,XX),dup(N1,XX,R).

% goal([X,Y]):-dup(10,[a,b,c,d],R),nrev(R,[X,Y|_]).

goal([X,Y]):-dup(18,[a,b,c,d],[X,Y|_]).
