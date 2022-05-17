
s4x4([
  [
     [S11,S12, S13,S14],
     [S21,S22, S23,S24],

     [S31,S32, S33,S34],
     [S41,S42, S43,S44]
  ],
  [
     [S11,S21, S31,S41],
     [S12,S22, S32,S42],

     [S13,S23, S33,S43],
     [S14,S24, S34,S44]
  ],
  [
     [S11,S12, S21,S22],
     [S13,S14, S23,S24],

     [S31,S32, S41,S42],
     [S33,S34, S43,S44]
  ]
]):-
  true.

/* this generates the 288 puzzles - and solves one if given clues */
sudoku(Xss):-
  s4x4([Xss|Xsss]),
  map11(permute,[1,2,3,4],[Xss|Xsss]).

map1x(_,_,[]).
map1x(F,Y,[X|Xs]):- call(F,Y,X),map1x(F,Y,Xs).
 
map11(_,_,[]).
map11(F,X,[Y|Ys]):-
  map1x(F,X,Y),
  map11(F,X,Ys).
   
permute([],[]).
permute([X|Xs],Zs):-permute(Xs,Ys),ins(X,Ys,Zs).

ins(X,Xs,[X|Xs]).
ins(X,[Y|Xs],[Y|Ys]):-ins(X,Xs,Ys).


goal(Xss):-sudoku(Xss).
