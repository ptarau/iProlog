% if I append [] to something
%	this something is the same

append([],Ys,Ys).

% when I have Zs after I append Ys to Xs
%	there is one list X like this:
%		if I append Ys to [X|Xs]
%			I have [X|Zs]

append([X|Xs],Ys,[X|Zs]):-append(Xs,Ys,Zs).

nrev([],[]).
nrev([X|Xs],Zs):-nrev(Xs,Ys),append(Ys,[X],Zs).

next_number_after(0,1).
next_number_after(1,2).
next_number_after(2,3).
next_number_after(3,4).
next_number_after(4,5).
next_number_after(5,6).
next_number_after(6,7).
next_number_after(7,8).
next_number_after(8,9).
next_number_after(9,10).
next_number_after(10,11).
next_number_after(11,12).
next_number_after(12,13).
next_number_after(13,14).
next_number_after(14,15).
next_number_after(15,16).
next_number_after(16,17).
next_number_after(17,18).

dup(0,X,X).
dup(N,X,R):-next_number_after(N1,N),append(X,X,XX),dup(N1,XX,R).

% goal([X,Y]):-dup(2,[a,b,c,d,e,f,g,h,i],R),nrev(R,[X,Y|_]).

goal([X,Y]):-dup(18,[a,b,c,d],[X,Y|_]).
