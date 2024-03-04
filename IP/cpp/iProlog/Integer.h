#pragma once
// Had this class (Java code) and it may relate mostly to
// a need for a unique (pointer) handle....

#include "Inty.h"

namespace iProlog {
	using namespace std;
	class Integer : public Inty<hashable_int_ptr_int> {
	public:
		inline Integer(int x) { set(x);  }
		inline Integer(Inty x) { set(x.as_int()); }
		inline Integer() { set(0); }
		inline Integer operator =(int x) { set(x);  return x; }
		bool operator ==(const Integer x) const { return as_int() == x.as_int(); }
		bool equal_to(const Integer x) const { return as_int() == x.as_int(); }
		inline operator unsigned long () { return (unsigned long) as_int(); }
	};
}
