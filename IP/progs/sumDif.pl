sumDif([X,+|OpenList],Hole) :- integer(X),sumDif(OpenList,Hole).
sumDif([X|Hole],Hole) :- integer(X).

