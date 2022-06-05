c:-[pl2nl].

% progs has sample code w/.pl suffix, arg to go.sh has no .pl, so add
% these to get relative path to source code
addprogpl(F,PL) :- concat_atom(['progs/',F,'.pl'],PL).

% entry point:
%
pl(F):-
 addprogpl(F,PL),     % PL <- proper source file path
 pl2nl(PL),           % make .nl source ("natural language assembly code")
 % jpl_call('iProlog.Main',run,[PL],_R),
 writeln(done).

pl2nl(PL):-
  atom_concat(PL,'.nl',NL),   % <PL path> - add .nl for output
  pl2nl(PL,NL).               % translate .pl to .nl

% pl2nl - translate contents of PL.pl to equational English-like
% "sentence" form, writing to file named PL.pl.nl (=NL)
%
pl2nl(PL,NL):-
  see(PL),            % Open PL for reading, make it current input
  tell(NL),           % Open NL for writing, make it current output
  repeat,             % for infinite choice points
   clause2sentence(EOF),  %   send translation of clause to sentence to NL
   EOF==yes,          %   if EOF hit instead of clause
   !,                 %     cut
  seen,               % Close PL. New input stream becomes user_input
  told.               % Close NL

% ------------------------------------------
% Pretty good explanation here:
%

read_with_names(T,T0):-
  write(user_error, '%---- read_with_names: '),nl(user_error),

  read_term(T0,[variable_names(Es)]), % Read term from <PL>.pl, unify it with T0.
                                      % "Unify Es with list of Name = Var,
                                      % where Name is an atom describing
                                      % the variable name and Var is a variable
                                      % that shares with the corresponding
                                      % variable in T0."???
                                      % The variables appear in read-order.

% https://stackoverflow.com/questions/7947910/converting-terms-to-atoms-preserving-variable-names-in-yap-prolog/7948525#7948525
%
%    ?- read_term(T,[variable_names(Eqs)]).
%    |: X+3*Y+X.
%  Eqs = ['X'=_A,'Y'=_B], % mapping of var-as-atom to gen'ed vars in T:
%  T = _A+3*_B+_A

  write(user_error,'T0 = '),write(user_error,T0),nl(user_error),
  write(user_error,'Es = '),write(user_error,Es),nl(user_error),

  copy_term(T0,T),                    % "Create a version of T0 with renamed
                                      % (fresh) variables and unify it to T.
                                      % Attributed variables have their
                                      % attributes copied"

  write(user_error,'After copy_term(T0,T) T = '),
  write(user_error,T),nl(user_error),

  maplist(call,Es).                   % Try to unify all Es?
                                      % ????????????????????
                                      % ????????????????????


write_sym(if)  :- !,  nl, write(if),  nl, write('  ').
write_sym(and) :- !,      write(and), nl, write('  ').
write_sym((.)) :- !,      write((.)), nl.
write_sym(W)   :-         write(W),       write(' ').

% clause2sentence - translate a clause to a "sentence"
%
clause2sentence(EOF):-
   read_with_names(T,T0),             % get some T0 from input,
                                      % T is copy with renamed variables,
                                      % T0 unified to T
   (
     T==end_of_file->EOF=yes          % like a possible result from read/1
   ;                                  % ;/2 -- like ||, and with -> above
                                      % "transparent to cuts"
     EOF=no,
     cls2nat(T,Ns),                   % Ns <- "naturalized" T
     T=T0,                            % unify T0 to T
     clean_up_nats(Ns,Xs),            %
     maplist(write_sym,Xs),nl
  ).

% Not sure what the apparent two final implicit params in DCG rules are
% about.
%
clean_up_nats(Ns,Xs):- clean_up_nats(Ns,Xs,0,_).

% Use of "-->" Definite Clause Grammar (DCG) operator
% https://stackoverflow.com/questions/32579266/what-does-the-operator-in-prolog-do

clean_up_nats([],[])-->[].
clean_up_nats([N|Ns],[X|Xs])-->
  clean_up_nat(N,X),
  clean_up_nats(Ns,Xs).

clean_up_nat(N,X,K1,K2):-
  var(N),                % True if Term currently is a free variable
  !,
  succ(K1,K2),           % https://www.swi-prolog.org/pldoc/doc_for?object=succ/2
                         % "True if K2 = K1 + 1 and K1 >= 0."
  atom_concat('_',K1,X), % X <- K1 with _ prepended
  N=X.
clean_up_nat(('[|]'),X,K,K):-
  !,
  X=list.
clean_up_nat(([]),X,K,K):-
  !,
  X=nil.
clean_up_nat(X,X,K,K).


% cls2nat -
%
cls2nat(C,Ns):-
    cls2eqs(C,_,Es),
    eqss2nat(Es,Ns).


cls2eqs(C,Xs,Es):-
  (C=(H:-Bs)->true;C=H,Bs=true),
  cls2list(H,Bs,Ts),
  maplist(term2eqs,Xs,Ts,Es).

cls2list(H,Bs,Cs):-
  body2list((H,Bs),Cs).

body2list(B,R):-
  var(B),
  !,
  R=[B].
body2list((B,Cs),[B|Bs]):-
  !,
  body2list(Cs,Bs).
body2list(true,[]):-
  !.
body2list(C,[C]).


eqss2nat(Xs,Ns):-
  eqss2nat(Xs,Ns,[]).

eqss2nat([H])-->
  !,
  pred2nat(H),
  [(.)].
eqss2nat([H|Bs])-->
  pred2nat(H),
  [(if)],
  body2nat(Bs),
  [(.)].

body2nat([])-->
  !,
  [].
body2nat([B])-->
  !,
  pred2nat(B).
body2nat([B|Bs])-->
  pred2nat(B),
  [and],
  body2nat(Bs).


pred2nat([_=P|Ts])-->
  {P=..Xs,trim_call(Xs,Ys)},
  eq2words(Ys),
  eqs2nat(Ts).


trim_call([call|Xs],R):-
  !,
  R=Xs.
trim_call(Xs,Xs).

eqs2nat([])-->
  !,
  [].
eqs2nat([X=T|Es])-->
  [and],
  {T=..Xs},
  [X],
  holds_lists_eqs(Xs),
  eqs2nat(Es).

holds_lists_eqs([lists|Xs])-->
  !,
  [lists],
  eq2words(Xs).
holds_lists_eqs(Xs)-->
  [holds],
  eq2words(Xs).

eq2words([])-->
  [].
eq2words([X|Xs])-->
  [X],
  eq2words(Xs).


% terms to equations

term2eqs(X,T,[X=T]):-
  var(T),
  !.
term2eqs(X,T,[X=T]):-
 atomic(T),
 !.
term2eqs(X,T,Es):-
 compound(T),
 term2eqs(X,T,Es,[]).

term2eqs(X,T)-->
 {var(T)},
 !,
 {X=T}.
term2eqs(X,T)-->
 {atomic(T)},
 !,
 {X=T}.
term2eqs(X,Xs)-->
 {is_list(Xs)},
 !,
 {T=..[lists|Xs]},
 term2eqs(X,T).
term2eqs(X,T)-->
  {compound(T),functor(T,F,N),functor(TT,F,N)},
  [X=TT],
  term2arg_eqs(0,N,TT,T).

term2arg_eqs(N,N,_,_)-->
  !,
  [].
term2arg_eqs(I,N,X,T)-->
  {I1 is I+1},
  {arg(I1,X,V)},
  {arg(I1,T,A)},
  term2eqs(V,A),
  term2arg_eqs(I1,N,X,T).


go:-
 pl2nl('progs/perms.pl'),
 shell('cat progs/perms.pl.nl').
go1:-
 pl2nl('progs/queens.pl'),
 shell('cat progs/queens.pl.nl').
go2:-
 pl2nl('progs/sud4x.pl'),
 shell('cat progs/sud4x.pl.nl').
