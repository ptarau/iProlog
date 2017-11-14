place_queen I _0 _1 _2 and
  _0 holds list I _3 and
  _1 holds list I _4 and
  _2 holds list I _5 .

place_queen I _0 _1 _2 and
  _0 holds list _3 Cs and
  _1 holds list _4 Us and
  _2 holds list _5 Ds 
if
  place_queen I Cs Us Ds .

place_queens nil _0 _1 _2 .

place_queens _0 Cs Us _1 and
  _0 holds list I Is and
  _1 holds list _2 Ds 
if
  place_queens Is Cs _3 Ds and
  _3 holds list _4 Us and
  place_queen I Cs Us Ds .

gen_places nil nil .

gen_places _0 _1 and
  _0 holds list _2 Qs and
  _1 holds list _3 Ps 
if
  gen_places Qs Ps .

qs Qs Ps 
if
  gen_places Qs Ps and
  place_queens Qs Ps _0 _1 .

goal Ps 
if
  qs _0 Ps and
  _0 lists 0 1 2 3 4 5 6 7 8 9 10 11 .

