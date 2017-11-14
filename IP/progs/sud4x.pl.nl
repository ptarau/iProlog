s4x4 _0 and
  _0 lists _1 _2 _3 and
  _1 lists _4 _5 _6 _7 and
  _4 lists S11 S12 S13 S14 and
  _5 lists S21 S22 S23 S24 and
  _6 lists S31 S32 S33 S34 and
  _7 lists S41 S42 S43 S44 and
  _2 lists _8 _9 _10 _11 and
  _8 lists S11 S21 S31 S41 and
  _9 lists S12 S22 S32 S42 and
  _10 lists S13 S23 S33 S43 and
  _11 lists S14 S24 S34 S44 and
  _3 lists _12 _13 _14 _15 and
  _12 lists S11 S12 S21 S22 and
  _13 lists S13 S14 S23 S24 and
  _14 lists S31 S32 S41 S42 and
  _15 lists S33 S34 S43 S44 .

sudoku Xss 
if
  s4x4 _0 and
  _0 holds list Xss Xsss and
  map11 permute _1 _2 and
  _1 lists 1 2 3 4 and
  _2 holds list Xss Xsss .

map1x _0 _1 nil .

map1x F Y _0 and
  _0 holds list X Xs 
if
  F Y X and
  map1x F Y Xs .

map11 _0 _1 nil .

map11 F X _0 and
  _0 holds list Y Ys 
if
  map1x F X Y and
  map11 F X Ys .

permute nil nil .

permute _0 Zs and
  _0 holds list X Xs 
if
  permute Xs Ys and
  ins X Ys Zs .

ins X Xs _0 and
  _0 holds list X Xs .

ins X _0 _1 and
  _0 holds list Y Xs and
  _1 holds list Y Ys 
if
  ins X Xs Ys .

goal Xss 
if
  sudoku Xss .

