c:-[pl_to_nl].

% progs has sample code w/.pl suffix, arg to go.sh has no .pl, so add
% these to get path to source code
addprogpl(F,PL) :- concat_atom(['progs/',F,'.pl'],PL).


% entry point:

pl(F):-
 addprogpl(F,PL),     % PL <- proper source file path
 pl_to_nl(PL),        % make .nl source ("natural language assembly code")
 % jpl_call('iProlog.Main',run,[PL],_R),
 writeln(done).

pl_to_nl(PL):-
  atom_concat(PL,'.nl',NL),   % <PL path> - add .nl for output
  pl_to_nl(PL,NL).               % translate .pl to .nl


% pl_to_nl - translate contents of PL.pl to equational English-like
% "sentence" form, writing to file named PL.pl.nl (=NL)

pl_to_nl(PL,NL):-
  see(PL),            % Open PL for reading, make it current input
  tell(NL),           % Open NL for writing, make it current output
  repeat,             % for infinite choice points
   clause_to_sentence(EOF),  %   send translation of clause to sentence to NL
   EOF==yes,          %   if EOF hit instead of clause
   !,                 %     cut
  seen,               % Close PL. New input stream becomes user_input
  told.               % Close NL

% ------------------------------------------
% Pretty good explanation here:
%

read_with_names(Term,T0):-
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
%    ?- read_term(Term,[variable_names(Eqs)]).
%    |: X+3*Y+X.
%  Eqs = ['X'=_A,'Y'=_B], % mapping of var-as-atom to gen'ed vars in Term:
%  Term = _A+3*_B+_A

  write(user_error,'   T0 = '),write(user_error,T0),nl(user_error),
  write(user_error,'   Es = '),write(user_error,Es),nl(user_error),

  copy_term(T0,Term),                 % "Create a version of T0 with renamed
                                      % (fresh) variables and unify it to Term.
                                      % Attributed variables have their
                                      % attributes copied"

  write(user_error,'After copy_term(T0,Term) Term = '),
  write(user_error,Term),nl(user_error),

  maplist(call,Es).                   % Try to unify all Es?
                                      % ????????????????????
                                      % ????????????????????


write_sym( if  ) :- !,  nl, write( if  ), nl, write('  ').
write_sym( and ) :- !,      write( and ), nl, write('  ').
write_sym( (.) ) :- !,      write( (.) ), nl.
write_sym( W   ) :-         write( W   ),     write(' ').

% clause_to_sentence - translate a clause to a "sentence"
%
clause_to_sentence(EOF):-
   read_with_names(Term,T0),          % get some T0 from input,
                                      % Term is copy with renamed variables,
                                      % T0 unified to Term
   (
     Term==end_of_file->EOF=yes       % like a possible result from read/1
   ;                                  % ;/2 -- like ||, and with -> above
                                      % "transparent to cuts"
     EOF=no,
     clause_to_nats(Term,Nats),       % Nats <- "naturalized" Term
     Term=T0,                         % unify T0 to Term
     clean_up_nats(Nats,Xs),          %
     maplist(write_sym,Xs),nl
  ).

% Not sure what the apparent two final implicit params in DCG rules are
% about.
%
clean_up_nats(Nats,Xs):- clean_up_nats(Nats,Xs,0,_),
                         write(user_error,'---- clean_up_nats:'), nl(user_error),
                         write(user_error,'   Nats='),
                         write(user_error,    Nats),              nl(user_error),
                         write(user_error,'   Xs='),
                         write(user_error,    Xs),                nl(user_error).

% Use of "-->" Definite Clause Grammar (DCG) operator
% https://stackoverflow.com/questions/32579266/what-does-the-operator-in-prolog-do

clean_up_nats( [],       []       ) --> [].
clean_up_nats( [N|Nats], [X|Xs] ) --> clean_up_nat(N,X), clean_up_nats(Nats,Xs).

% atom_concat('_',K1,X)  -- generates the _0, _1, etc. vars

clean_up_nat( Nat,     X, K1, K2 ) :-     var(Nat),
                                       !, succ(K1,K2), atom_concat('_',K1,X), Nat=X.
clean_up_nat( ('[|]'), X, K,  K  ) :-  !, X=list.
clean_up_nat( ([]),    X, K,  K  ) :-  !, X=nil.
clean_up_nat( X,       X, K,  K  ) .


% clause_to_nats -

clause_to_nats(Clause,Nats) :- clause_to_eqns(Clause,_,Es)
                            ,  eqnss_to_nat(Es,Nats)
                            .

clause_to_eqns(Clause,Xs,Es) :- (Clause = (Head:-Bodies)->true ; Clause=Head,Bodies=true)
                             ,  clause_to_list(Head,Bodies,Terms)
                             ,  maplist(term_to_eqns,Xs,Terms,Es)
                             .

clause_to_list(Head,Bodies,Cs) :- body_to_list((Head,Bodies),Cs).

body_to_list( Body,      R             ) :- var(Body), !, R=[Body].
body_to_list( (Body,Cs), [Body|Bodies] ) :-            !, body_to_list(Cs,Bodies).
body_to_list( true,      []            ) :-            !.
body_to_list( C,         [C]           ) .


eqnss_to_nat(Xs,Nats) :- eqnss_to_nat(Xs,Nats,[]).

eqnss_to_nat( [Head]        ) --> !, pred_to_nat(Head), [(.)].
eqnss_to_nat( [Head|Bodies] ) -->    pred_to_nat(Head)
                               ,    [(if)]
			       ,    body_to_nat(Bodies), [(.)]
			       .

body_to_nat( []            ) --> !, [].
body_to_nat( [Body]        ) --> !, pred_to_nat(Body).
body_to_nat( [Body|Bodies] ) -->    pred_to_nat(Body), [and], body_to_nat(Bodies).


pred_to_nat([_=P|Terms])--> {P=..Xs,trim_call(Xs,Ys)}
                          , eqn_to_words(Ys)
			  , eqns_to_nats(Terms)
			  , {write(user_error,"****** pred to nats **** ")}
			  , {nl(user_error)}
			  , {write(user_error,"  P=")}
			  , {write(user_error,P)}
			  , {nl(user_error)}
			  , {write(user_error,"  Xs=")}
			  , {write(user_error,Xs)}
			  , {nl(user_error)}
			  , {write(user_error,"  Ys=")}
			  , {write(user_error,Ys)}
			  , {nl(user_error)}
			  , {write(user_error,"  Terms=")}
			  , {write(user_error,Terms)}
			  , {nl(user_error)}
			  .

trim_call( [call|Xs], R  ) :- !, R=Xs.
trim_call( Xs,        Xs ) .

eqns_to_nats( []          ) --> !, [].
eqns_to_nats( [X=Term|Es] ) -->   [and]
                             ,    {Term=..Xs}
			     , {write(user_error,"*** begin eqns to nats ***")}
			     , {write(user_error,"  Term=")}
			     , {write(user_error,Term)}
			     , {nl(user_error)}
			     , {write(user_error,"  Xs=")}
			     , {write(user_error,Xs)}
			     , {nl(user_error)}
			     ,    [X]
			     ,    holds_lists_eqns(Xs)
			     , {nl(user_error)}
			     , {write(user_error,"  X=")}
			     , {write(user_error,X)}
			     , {nl(user_error)}
			     ,    eqns_to_nats(Es)
			     , {write(user_error,"  Es=")}
			     , {write(user_error,Es)}
			     , {nl(user_error)}
			     , {write(user_error,"*** end eqns to nats ***")}
			     , {nl(user_error)}
			     .

holds_lists_eqns( [lists|Xs] ) --> !, [lists], eqn_to_words(Xs).
holds_lists_eqns( Xs         ) -->    [holds], eqn_to_words(Xs).

eqn_to_words( []     ) --> [].
eqn_to_words( [X|Xs] ) --> [X], eqn_to_words(Xs).


% terms to equations

term_to_eqns( X, Term, [X=Term] ) :-   var(Term)
				  ,    write(user_error,'****** in var term_to_eqns')
			          ,    {nl(user_error)}
				  ,    write(user_error,"  X=")
				  ,    write(user_error, X)
			          ,    {nl(user_error)}
				  ,    write(user_error,"  Term=")
				  ,    write(user_error,Term)
				  ,    nl(user_error)
                                  ,    !
				  .
term_to_eqns( X, Term, [X=Term] ) :-   atomic(Term)
				  ,    write(user_error,'****** in atomic term_to_eqns')
				  ,    nl(user_error)
				  ,    write(user_error,"  X=")
				  ,    write(user_error,X)
				  ,    nl(user_error)
				  ,    write(user_error,"  Term=")
				  ,    write(user_error,Term)
				  ,    nl(user_error)
                                  ,    !
				  .
term_to_eqns( X, Term, Es       ) :-   compound(Term)
				  ,    write(user_error,'****** begin compound term_to_eqns')
                                  ,    term_to_eqns(X,Term,Es,[])
				  ,    nl(user_error)
				  ,    write(user_error,"  X=")
				  ,    write(user_error,X)
				  ,    nl(user_error)
				  ,    write(user_error,"  Term=")
				  ,    write(user_error,Term)
				  ,    nl(user_error)
				  ,    write(user_error,'****** end compound term_to_eqns')
				  ,    nl(user_error)
				  .

term_to_eqns( X, Term  ) --> {var(Term)},    !, {X=Term}.
term_to_eqns( X, Term  ) --> {atomic(Term)}, !, {X=Term}.
term_to_eqns( X, Xs    ) --> {is_list(Xs)},  !, {Term=..[lists|Xs]}, term_to_eqns(X,Term).
term_to_eqns( X, Term  ) --> {compound(Term)
                           ,  functor(Term,F,N)
			   ,  functor(TT,F,N)}
			   , [X=TT]
			   , term_to_arg_eqns(0,N,TT,Term)
			   .

term_to_arg_eqns( N, N, _, _    ) --> !, [].
term_to_arg_eqns( I, N, X, Term ) --> {I1 is I+1}
                                    , {arg(I1,X,V)}
				    , {arg(I1,Term,A)}
				    , term_to_eqns(V,A)
				    , term_to_arg_eqns(I1,N,X,Term)
				    .

go:-
 pl_to_nl('progs/perms.pl'),
 shell('cat progs/perms.pl.nl').
go1:-
 pl_to_nl('progs/queens.pl'),
 shell('cat progs/queens.pl.nl').
go2:-
 pl_to_nl('progs/sud4x.pl'),
 shell('cat progs/sud4x.pl.nl').
