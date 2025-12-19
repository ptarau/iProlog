% Still haven't figured out how to translate this to English
% There's no description of a chessboard or how the squares
% relate in terms of how a queen can move
% Q is represented by column ID [0,1,...]
% No queen can be in the same column anyway, so the set of
% distinct column IDs represents that implied constraint.

% (This was called "place_queen".)
% Guess: a queen doesn't fight herself

doesnt_fight_in(
	QueenColumn,
	[QueenColumn|_],
	[QueenColumn|_],
	[QueenColumn|_] ).

% if a queen doesn't fight along some cols,rows,diags squares
%	then it doesn't fight in ... ???

doesnt_fight_in(Q,[_|Rows],[_|LeftDiags],[_|RightDiags]):-
	doesnt_fight_in(Q,Rows,LeftDiags,RightDiags).

% (This was called "place_queens".)

dont_fight_on_these_lines([],_,_,_).
dont_fight_on_these_lines(
	[QueenColumn|Qs],
	Rows,
	LeftDiags,
	[_|RightDiags]
) :-
  dont_fight_on_these_lines(Qs,Rows,[_|LeftDiags],RightDiags),
  doesnt_fight_in(QueenColumn,Rows,LeftDiags,RightDiags).

% (This was called "gen_places".)
% (Looks like it is exhaustive.)
% Loose constraint:
% A queen in some column can be in a row
%   if other queens are in other columns and other rows

can_be_in_these_places([],[]).
can_be_in_these_places([_|OtherColumns],[_|OtherRows]):-
	can_be_in_these_places(OtherColumns,OtherRows).

qs(Columns,Rows):-
	can_be_in_these_places(Columns,Rows),
	dont_fight_on_these_lines(Columns,Rows,_,_).

goal(Rows):-qs([0,1,2,3,4,5,6,7,8,9,10],Rows).
% goal(Rows):-qs([0,1,3,4],Rows).

