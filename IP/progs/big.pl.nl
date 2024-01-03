append nil Ys Ys .

append _0 Ys _1 and
  _0 holds list X Xs and
  _1 holds list X Zs 
if
  append Xs Ys Zs .

nrev nil nil .

nrev _0 Zs and
  _0 holds list X Xs 
if
  nrev Xs Ys and
  append Ys _1 Zs and
  _1 lists X .

next_number_after 0 1 .

next_number_after 1 2 .

next_number_after 2 3 .

next_number_after 3 4 .

next_number_after 4 5 .

next_number_after 5 6 .

next_number_after 6 7 .

next_number_after 7 8 .

next_number_after 8 9 .

next_number_after 9 10 .

next_number_after 10 11 .

next_number_after 11 12 .

next_number_after 12 13 .

next_number_after 13 14 .

next_number_after 14 15 .

next_number_after 15 16 .

next_number_after 16 17 .

next_number_after 17 18 .

dup 0 X X .

dup N X R 
if
  next_number_after N1 N and
  append X X XX and
  dup N1 XX R .

goal _0 and
  _0 lists X Y 
if
  dup 2 _1 R and
  _1 lists a b c d and
  nrev R _2 and
  _2 holds list X _3 and
  _3 holds list Y _4 .

