#pragma once

#include "IntList.h"
#include <vector>

namespace iProlog
{
	/**
	 * runtime representation of an immutable list of goals
	 * together with top of heap and trail pointers
	 * and current clause tried out by head goal
	 * as well as registers associated to it
	 *
	 * note that parts of this immutable lists
	 * are shared among alternative branches
	 */
	class Spine
	{

	  /**
	   * creates a spine - as a snapshot of some runtime elements
	   */
  public:
	  virtual ~Spine()
	  {
		  delete gs;
	  }

	  Spine(std::vector<int> &gs0, int const base, IntList *const gs, int const ttop, int const k, std::vector<int> &cs);

	  /**
	   * creates a specialized spine returning an answer (with no goals left to solve)
	   */
	  Spine(int const hd, int const ttop);

	  const int hd; // head of the clause to which this corresponds
	  const int base; // top of the heap when this was created

	  IntList *const gs; // goals - with the top one ready to unfold
	  const int ttop; // top of the trail when this was created

	  int k = 0;

	  std::vector<int> xs; // index elements
	  std::vector<int> cs; // array of  clauses known to be unifiable with top goal in gs
	};

}
