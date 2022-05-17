eq(X,X).

sel(X,[X|Xs],Xs).
sel(X,[Y|Xs],[Y|Ys]):-sel(X,Xs,Ys).

perm([],[]).
perm([X|Xs],Zs):-
  perm(Xs,Ys),
  sel(X,Zs,Ys).  

app([],Xs,Xs).
app([X|Xs],Ys,[X|Zs]):-app(Xs,Ys,Zs).

nrev([],[]).
nrev([X|Xs],Zs):-nrev(Xs,Ys),app(Ys,[X],Zs).

% goal(X):-eq(X,[1,2,3,4,5,6,7,8,9]),perm(X,P),nrev(P,X).

% goal(P):-eq(X,[1,2,3,4,5,6,7,8,9,10,11]),nrev(X,P),perm(X,P).


input([1,2,3,4,5,6,7,8,9,10,11]).

goal(Y):-input(X),nrev(X,Y),perm(X,Y),perm(Y,X).
