someone_(i_).  % I am someone

one_(i_).  % There is only one me
other_(i_) :- false.  % There are no other mes.
other_(someone_). % there are other people
not_(there_is_(other_(i_))).

not_(good_(someone_(X))) :- bad_(someone_(X)).  % if someone is being bad, this person is not being good
not_(bad_(someone_(X))) :- good_(someone_(X)).  % if someone is being good, this person is not being bad


can_(bad_,someone_).  % There can be a bad person
can_(good_,someone_). % There are good people
can_(i_,say_(X)) :- not(X).  % Grice - quantity; do not be redundant

% if someone is good, someone else is bad

% bad_(dummy) :- false.   % just to make sure it's defined in CLI
% bad_(someone_(other_)). % just to test that this can be satisfied

bad_(i_). % test whether comb(bad_,i_) rejects if already bad; works
% good_(i_).  % test whether comb(bad_,i_) rejects if good; works
% assert neither;

good_(someone_) :- bad_(someone_(other_)).
good_(dummy).
bad_(dummy).


comb(Modifier,Modified) :-
    not(call(Modifier,Modified)), % not already asserted
%    call(Modifier,X), % others possible
    other_(Modified),
    call(can_(Modifier),X).

goal(X) :- comb_(good_,someone_),someone_(X).
