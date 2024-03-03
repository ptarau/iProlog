#pragma once
namespace iProlog {
	const int MIN_HEAP_SIZE = 1 << 10;

	/* RAW=true says to go with a less-safe, faster implementation
	 *    than STL vectors, with no bounds check, and less header info, to
	 *    save a little space. The fast-copy cell heap-to-heap relocation may
	 *    end up in this class eventually.
	 * "if (is_raw)..." ensures that code code gets checked by the
	 * compiler, except there there's an #ifdef conditional
	 * compilation. Constant-folding and dead code elimination
	 * does the rest.
	 */
// #define RAW
#ifdef RAW
	const bool is_raw = true;
#else
	const bool is_raw = false;
#endif

	const bool indexing = false;

	const int MAXIND = 3;       // "number of index args" [Engine.java]
	const int START_INDEX = 1;	// "if # of clauses < START_INDEX,
								// turn off indexing" [Engine.java]
}