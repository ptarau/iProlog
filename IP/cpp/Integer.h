#pragma once
// Had this class (Java code) and it may relate mostly to
// a need for a unique (pointer) handle....

#include "unordered_map"

namespace iProlog {
	using namespace std;
	class Integer {
	public:
		size_t i;
		inline Integer(size_t x) { i = x;  }
		inline Integer() { i = 0; }
		inline Integer operator =(size_t x) { i = x;  return i; }
		bool operator ==(const Integer x) const { return i == x.i;  }
		bool equal_to(const Integer x) const { return i == x.i;  }
		inline operator unsigned long () { return (unsigned long)i; }
		string toString() const { return to_string(i); }
	};
}