#pragma once

/* Inty -- for stronger typing in an "ints all the way down" datastructure
 * 
 * It may make sense to make it a template class, so that there can
 * be int16, int32, int64 in places where it matters for speed/space,
 * even varying according to use in data structures.
 */

namespace iProlog {
	template <class int_type> class Inty {
	private:
		int_type i;
	public:

		inline Inty<int_type>() { i = 0; }
		Inty<int_type>(int_type x) : Inty<int_type>() { i = x; }

		inline int_type as_int() const { return i; }
		inline bool operator == (Inty x) const { return i == x.as_int(); }
		inline bool operator != (Inty x) const { return i != x.as_int(); }
		inline Inty<int_type> dec() const { return Inty<int_type>(i - 1); }
		// inline Inty<int_type>& operator =(const Inty<int_type>& x) { i = x.as_int(); return *this;  }
		inline void operator =(const Inty<int_type>& x) { i = x.as_int();  }
		inline bool operator<(const Inty<int_type>& x) const { return i < x.as_int(); }
		inline bool operator>(const Inty<int_type>& x) const { return i > x.as_int(); }
	};

} // namespace