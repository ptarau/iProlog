place_queen(I,[I|_],[I|_],[I|_]).
place_queen(I,[_|Cs],[_|Us],[_|Ds]):-place_queen(I,Cs,Us,Ds).

place_queens([],_,_,_).
place_queens([I|Is],Cs,Us,[_|Ds]):-
  place_queens(Is,Cs,[_|Us],Ds),
  place_queen(I,Cs,Us,Ds).

gen_places([],[]).
gen_places([_|Qs],[_|Ps]):-gen_places(Qs,Ps).

qs(Qs,Ps):-gen_places(Qs,Ps),place_queens(Qs,Ps,_,_).

goal(Ps):-qs([0,1,2,3,4,5,6,7,8,9,10,11],Ps).
