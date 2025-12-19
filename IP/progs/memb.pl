memb(E,[E|_]).
memb(E,[_|T]) :- memb(E,T).

goal(E):-memb(E,[0,1,2,3,4]).
