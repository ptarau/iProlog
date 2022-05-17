#pragma once

#include <vector>

namespace iProlog
{
	/**
	 * representation of a clause
	 */
	class Clause
	{
  public:
	  Clause(int const len, std::vector<int> &hgs, int const base, int const neck, std::vector<int> &xs);

	  const int len;
	  const std::vector<int> hgs;
	  const int base;
	  const int neck;
	  const std::vector<int> xs;
	};

}
