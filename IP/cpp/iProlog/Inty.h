#pragma once

/* Inty -- for stronger typing in an "ints all the way down" datastructure
 * 
 * It may make sense to make it a template class, so that there can
 * be int16, int32, int64 in places where it matters for speed/space,
 * even varying according to use in data structures.
 */

namespace iProlog {
	class Inty {
		int i;
	public:
		inline int as_int() const { return i; }
		inline int set(int x) { i = x;  return i; }
		inline bool operator == (Inty x) const { return i == x.as_int(); }
	};

} // namespace