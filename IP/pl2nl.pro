c:-[pl2nl].

go:-pl2nl('progs/perms.pl'),shell('cat progs/perms.pl.nl').
go1:-pl2nl('progs/queens.pl'),shell('cat progs/queens.pl.nl').
go2:-pl2nl('progs/sud4x.pl'),shell('cat progs/sud4x.pl.nl').


pl(F):-
 concat_atom(['progs/',F,'.pl'],PL),
 pl2nl(PL),
 % jpl_call('iProlog.Main',run,[PL],_R),
 writeln(done).


pl2nl(PL):-
  atom_concat(PL,'.nl',NL),
  pl2nl(PL,NL).

pl2nl(PL,NL):-
  see(PL),
  tell(NL),
  repeat,
   clause2sent(EOF),
   EOF==yes,
   !,
  seen,
  told.

read_with_names(T,T0):-
  read_term(T0,[variable_names(Es)]),
  copy_term(T0,T),
  maplist(call,Es).

writes(if):-!,nl,write(if),nl,write('  ').
writes(and):-!,write(and),nl,write('  ').
writes((.)):-!,write((.)),nl.
writes(W):-write(W),write(' ').

clause2sent(EOF):-
   read_with_names(T,T0),
   (
     T==end_of_file->EOF=yes
   ;
     EOF=no,
     cls2nat(T,Ns),
     T=T0,
     clean_up_nats(Ns,Xs),
     maplist(writes,Xs),nl
  ).

clean_up_nats(Ns,Xs):- clean_up_nats(Ns,Xs,0,_).

clean_up_nats([],[])-->[].
clean_up_nats([N|Ns],[X|Xs])-->
  clean_up_nat(N,X),
  clean_up_nats(Ns,Xs).

clean_up_nat(N,X,K1,K2):-var(N),!,succ(K1,K2),atom_concat('_',K1,X),N=X.
clean_up_nat(('[|]'),X,K,K):-!,X=list.
clean_up_nat(([]),X,K,K):-!,X=nil.
clean_up_nat(X,X,K,K).



cls2nat(C,Ns):-
    cls2eqs(C,_,Es),
    eqss2nat(Es,Ns).


cls2eqs(C,Xs,Es):-
  (C=(H:-Bs)->true;C=H,Bs=true),
  cls2list(H,Bs,Ts),
  maplist(term2eqs,Xs,Ts,Es).

cls2list(H,Bs,Cs):-
  body2list((H,Bs),Cs).

body2list(B,R):-var(B),!,R=[B].
body2list((B,Cs),[B|Bs]):-!,body2list(Cs,Bs).
body2list(true,[]):-!.
body2list(C,[C]).


eqss2nat(Xs,Ns):-eqss2nat(Xs,Ns,[]).

eqss2nat([H])-->!,pred2nat(H),[(.)].
eqss2nat([H|Bs])-->pred2nat(H),[(if)],body2nat(Bs),[(.)].

body2nat([])--> !,[].
body2nat([B])-->!,pred2nat(B).
body2nat([B|Bs])-->pred2nat(B),[and],body2nat(Bs).


pred2nat([_=P|Ts])-->{P=..Xs,trim_call(Xs,Ys)},eq2words(Ys),eqs2nat(Ts).


trim_call([call|Xs],R):-!,R=Xs.
trim_call(Xs,Xs).

eqs2nat([])-->!,[].
eqs2nat([X=T|Es])-->[and],{T=..Xs},[X],holds_lists_eqs(Xs),eqs2nat(Es).

holds_lists_eqs([lists|Xs])-->!,[lists],eq2words(Xs).
holds_lists_eqs(Xs)-->[holds],eq2words(Xs).

eq2words([])-->[].
eq2words([X|Xs])-->[X],eq2words(Xs).



% terms to equations

term2eqs(X,T,[X=T]):-var(T),!.
term2eqs(X,T,[X=T]):-atomic(T),!.
term2eqs(X,T,Es):-compound(T),term2eqs(X,T,Es,[]).

term2eqs(X,T)-->{var(T)},!,{X=T}.
term2eqs(X,T)-->{atomic(T)},!,{X=T}.
term2eqs(X,Xs)-->{is_list(Xs)},!,{T=..[lists|Xs]},term2eqs(X,T).
term2eqs(X,T)-->{compound(T),functor(T,F,N),functor(TT,F,N)},
  [X=TT],
  term2arg_eqs(0,N,TT,T).

term2arg_eqs(N,N,_,_)-->!,[].
term2arg_eqs(I,N,X,T)-->
  {I1 is I+1},
  {arg(I1,X,V)},
  {arg(I1,T,A)},
  term2eqs(V,A),
  term2arg_eqs(I1,N,X,T).

