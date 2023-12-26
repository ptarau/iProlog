metaint nil .

metaint _0 and
  _0 holds list G Gs 
if
  cls _1 Gs and
  _1 holds list G Bs and
  metaint Bs .

cls _0 Tail and
  _0 holds list _1 Tail and
  _1 holds sel X _2 Xs and
  _2 holds list X Xs .

cls _0 Tail and
  _0 holds list _1 _2 and
  _1 holds sel X _3 _4 and
  _3 holds list Y Xs and
  _4 holds list Y Ys and
  _2 holds list _5 Tail and
  _5 holds sel X Xs Ys .

cls _0 Tail and
  _0 holds list _1 Tail and
  _1 holds perm nil nil .

cls _0 Tail and
  _0 holds list _1 _2 and
  _1 holds perm _3 Zs and
  _3 holds list X Xs and
  _2 holds list _4 _5 and
  _4 holds perm Xs Ys and
  _5 holds list _6 Tail and
  _6 holds sel X Zs Ys .

input _0 _1 and
  _0 lists 1 2 3 4 5 6 7 8 9 10 11 and
  _1 lists 11 10 9 8 7 6 5 4 3 2 1 .

goal Y 
if
  input X Y and
  metaint _0 and
  _0 lists _1 _2 and
  _1 holds perm X Y and
  _2 holds perm Y X .

