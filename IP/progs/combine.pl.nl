member X _0 and
  _0 holds list X _1 .

member X _0 and
  _0 holds list _1 L 
if
  member X L .

maplist _0 nil nil .

maplist P _0 _1 and
  _0 holds list L1 L1s and
  _1 holds list L2 L2s 
if
  P L1 L2 and
  maplist P L1s L2s .

combine Ls Rs 
if
  maplist member Rs Ls .

goal R 
if
  combine _0 R and
  _0 lists _1 _2 _3 _4 and
  _1 lists 1 2 and
  _2 lists a b and
  _3 lists 8 9 and
  _4 lists x y .

