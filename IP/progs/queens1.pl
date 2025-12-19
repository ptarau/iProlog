% Q represented by row (or col) number

place_queen(Q,[Q|_],[Q|_],[Q|_]).
place_queen(Q,[_|Cols],[_|Rows],[_|Diags]):-place_queen(Q,Cols,Rows,Diags).

there_are_places_where_these_queens_dont_fight([],_,_,_).
there_are_places_where_these_queens_dont_fight([Q|Qs],Cols,Rows,[_|Diags]):-
  there_are_places_where_these_queens_dont_fight(Qs,Cols,[_|Rows],Diags),
  place_queen(Q,Cols,Rows,Diags).

these_queens_can_be_in_these_places([],[]).
these_queens_can_be_in_these_places([_|Qs],[_|Ps]):-
	these_queens_can_be_in_these_rows(Qs,Ps).

qs(Qs,Ps):-
	these_queens_can_be_in_these_places(Qs,Ps),
	there_are_places_where_these_queens_dont_fight(Qs,Ps,_,_).

goal(Ps):-qs([0,1,2,3,4,5,6,7,8,9,10,11],Ps).
