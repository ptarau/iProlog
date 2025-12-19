#pragma once

// #define NDEBUG
#include <assert.h>

#include "Inty.h"

namespace iProlog {
	const int MIN_HEAP_SIZE = 1 << 10;

#define COUNTING_MATCHES

	// For tuning speed/space tradeoffs.
	// See https://en.cppreference.com/w/cpp/types/integer

	typedef int cell_int;		// could go down to int_least16_t but getting compile
								// errors in xhash about IMap even with "short int"
	typedef int cls_no_int;		// could go down to uint_least8_t
								// for logic programs with <256 clauses

	typedef int sym_idx_int;

	typedef Inty<cls_no_int> ClauseNumber; /* 1 ... clause array size */

	typedef Inty<cls_no_int> ClauseIndex;

	/* RAW=true says to go with a less-safe, faster implementation
	 *    than STL vectors, with no bounds check, and less header info, to
	 *    save a little space. The fast-copy cell heap-to-heap relocation may
	 *    end up in this class eventually.
	 * "if (is_raw)..." ensures that code code gets checked by the
	 * compiler, except there there's some #ifdef conditional
	 * compilation. Constant-folding and dead code elimination
	 * does the rest.
	 */
#define RAW_CELL_HEAP
#ifdef RAW_CELL_HEAP
	const bool has_raw_cell_heap = true;
#else
	const bool has_raw_cell_heap = false;
#endif

	// Some way to pass these on compiler command line????

	const int MAXIND = 3;       // "number of index args" [Engine.java]
	const int IV_LEN = (MAXIND + 1); // for sentinel search
	const int START_INDEX = 1;	// "if # of clauses < START_INDEX,
								// turn off indexing" [Engine.java]

	const bool indexing = true;

	const int INTMAP_INIT_SIZE = (1 << 2);
	const float INTMAP_FILL_FACTOR = 0.75f;

// use int *instead of vector for sorting in indexing
#define RAW_QUICKSORT

#define USE_SIGN_BIT true

	const bool integrity_check = false;
	void checkit_();
	inline void checkit() { if (integrity_check) checkit_(); }

#define COUNTING_MATCHES

#define TRY_CATCHING

#define RAW_IMAPS

#define RAW_CELL_LIST
#define MEASURE_CELL_LIST

#define RAW_SKEL

}