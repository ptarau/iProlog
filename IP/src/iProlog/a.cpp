#include "a.h"

namespace iProlog
{

	Clause::Clause(int const len, std::vector<int> &hgs, int const base, int const neck, std::vector<int> &xs) : len(len) / * length of heap slice */, hgs(hgs) / * head + goals pointing to cells in cs */, base(base) / * heap where this starts */, neck(neck) / * first after the end of the head */, xs(xs) / * indexables in head */
	{
	}
}
