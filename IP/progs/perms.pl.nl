eq X X .

sel X _0 Xs and
  _0 holds list X Xs .

sel X _0 _1 and
  _0 holds list Y Xs and
  _1 holds list Y Ys 
if
  sel X Xs Ys .

perm nil nil .

perm _0 Zs and
  _0 holds list X Xs 
if
  perm Xs Ys and
  sel X Zs Ys .

app nil Xs Xs .

app _0 Ys _1 and
  _0 holds list X Xs and
  _1 holds list X Zs 
if
  app Xs Ys Zs .

nrev nil nil .

nrev _0 Zs and
  _0 holds list X Xs 
if
  nrev Xs Ys and
  app Ys _1 Zs and
  _1 lists X .

input _0 and
  _0 lists 1 2 3 4 5 6 7 8 9 10 11 .

goal Y 
if
  input X and
  nrev X Y and
  perm X Y and
  perm Y X .

