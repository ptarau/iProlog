doesnt_fight_in QueenColumn _0 _1 _2 and
  _0 holds list QueenColumn _3 and
  _1 holds list QueenColumn _4 and
  _2 holds list QueenColumn _5 .

doesnt_fight_in Q _0 _1 _2 and
  _0 holds list _3 Rows and
  _1 holds list _4 LeftDiags and
  _2 holds list _5 RightDiags 
if
  doesnt_fight_in Q Rows LeftDiags RightDiags .

dont_fight_on_these_lines nil _0 _1 _2 .

dont_fight_on_these_lines _0 Rows LeftDiags _1 and
  _0 holds list QueenColumn Qs and
  _1 holds list _2 RightDiags 
if
  dont_fight_on_these_lines Qs Rows _3 RightDiags and
  _3 holds list _4 LeftDiags and
  doesnt_fight_in QueenColumn Rows LeftDiags RightDiags .

can_be_in_these_places nil nil .

can_be_in_these_places _0 _1 and
  _0 holds list _2 OtherColumns and
  _1 holds list _3 OtherRows 
if
  can_be_in_these_places OtherColumns OtherRows .

qs Columns Rows 
if
  can_be_in_these_places Columns Rows and
  dont_fight_on_these_lines Columns Rows _0 _1 .

goal Rows 
if
  qs _0 Rows and
  _0 lists 0 1 2 3 4 5 6 7 .

