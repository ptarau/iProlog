genLambda X Vs N N 
if
  memb X Vs .

genLambda _0 Vs _1 N2 and
  _0 holds l X A and
  _1 holds s N1 
if
  genLambda A _2 N1 N2 and
  _2 holds list X Vs .

genLambda _0 Vs _1 N3 and
  _0 holds a A B and
  _1 holds s N1 
if
  genLambda A Vs N1 N2 and
  genLambda B Vs N2 N3 .

memb X _0 and
  _0 holds list X _1 .

memb X _0 and
  _0 holds list _1 Xs 
if
  memb X Xs .

genClosedLambdaTerm L T 
if
  genLambda T nil L zero .

some _0 and
  _0 holds s _1 and
  _1 holds s _2 and
  _2 holds s _3 and
  _3 holds s _4 and
  _4 holds s _5 and
  _5 holds s _6 and
  _6 holds s _7 and
  _7 holds s zero .

goal Lam 
if
  some Size and
  genClosedLambdaTerm Size Lam .

