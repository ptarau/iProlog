#pragma once

#include "defs.h"
#include "cell.h"
#include <vector>

namespace iProlog {
	using namespace std;

#ifdef RAW_SKEL
    //  "Unlike _alloca, which doesn't require or permit a call
    //  to free to free the memory so allocated, _malloca requires
    //  the use of _freea to free memory." This can't necessarily
    //  work even as an inlined member function because it allocates
    //  within the existing C++ stack frame. alloca() may be
    //  preferable because of lower overhead, but is dangerous
    //  because it won't check stack bounds against C++ heap
    //  bounds and call malloc() if there's memory-corruption risk.
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/malloca?view=msvc-170
    
    typedef cell* clause_skel;

#   define alloc_skel(skel_len) (cell*)_malloca(skel_len * sizeof(cell));
#   define free_skel(s) _freea(s)
#   define skel_data(sk) sk
#   define init_skel(sk) sk = nullptr
#else
    typedef vector<cell> clause_skel;

    #define alloc_skel(skel_len) vector<cell>(skel_len)
    #define free_skel(s) s.clear()
#   define skel_data(sk) sk.data()
#   define init_skel(sk) sk.clear()
#endif
}

