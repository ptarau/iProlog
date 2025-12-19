% Unary arithmetic
% X is the sum of zero and X

 add(0,X,X).

% if Z is the sum of X and Y
%   the successor of Z is the sum of the successor of X and Y

 add(s(X),Y,s(Z)):-add(X,Y,Z).
 
% gdebug(R):-the_sum_of(0,X,the_successor_of(0)).

% R is the goal if
%   R is the sum of
%	the successor of the successor of zero
%   and
%   	the successor of the successor of zero
goal(R):-add(s(s(0)),s(s(0)),R).
