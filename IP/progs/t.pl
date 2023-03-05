% live_([i_]).
% live_([y,[o,[u]]]).
% live_([y,o,u]).
% good_(Person) :- live_(Person).
% goal(Person):-good_(Person).
% goal(P):-live_(P).

zero_and_one([0,1,2]).
goal(V):-zero_and_one(V).
