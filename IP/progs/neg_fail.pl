something(b).
bad(a).

goal(P) :-
  not(something(P)),
  bad(P).
