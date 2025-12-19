something(b).
bad(a).

goal(P) :-
  bad(P),
  not(something(P)).
