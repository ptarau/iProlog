member(X,[X|_]).
member(X,[_|L]):-member(X,L).
/*
maplist(_,[]).
maplist(P,[L1|L1s]) :- call(P,L1), maplist(P,L1s).
*/
maplist(_,[],[]).
maplist(P,[L1|L1s],[L2|L2s]) :- call(P,L1,L2), maplist(P,L1s,L2s).
/*
maplist(_,[],[],[]).
maplist(P,[L1|L1s],[L2|L2s],[L3|L3s]) :- call(P,L1,L2,L3), maplist(P,L1s,L2s,L3s).

maplist(_,[],[],[],[]).
maplist(P,[L1|L1s],[L2|L2s],[L3|L3s],[L4|L4s]) :- call(P,L1,L2,L3,L4), maplist(P,L1s,L2s,L3s,L4s).
*/
combine(Ls,Rs) :- maplist(member,Rs,Ls).
goal(R) :- combine([[1,2],[a,b],[8,9],[x,y]],R).
