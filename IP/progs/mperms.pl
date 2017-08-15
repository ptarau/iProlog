metaint([]).
metaint([G|Gs]):-cls([G|Bs],Gs),metaint(Bs).

cls([sel(X,[X|Xs],Xs)|Tail],Tail).
cls([sel(X,[Y|Xs],[Y|Ys]),sel(X,Xs,Ys)|Tail],Tail).

cls([perm([],[])|Tail],Tail).
cls([perm([X|Xs],Zs),perm(Xs,Ys),sel(X,Zs,Ys)|Tail],Tail).  
    
input([1,2,3,4,5,6,7,8,9,10,11],[11,10,9,8,7,6,5,4,3,2,1]).

goal(Y):-input(X,Y),metaint([perm(X,Y),perm(Y,X)]).
