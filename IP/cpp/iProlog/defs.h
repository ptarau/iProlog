#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <climits>
#include <stdexcept>


using namespace std;

namespace iProlog {
    typedef const string cstr;
    inline cstr operator+(cstr s, int i) { return s + to_string(i); }
    inline cstr operator+(cstr s, size_t i) { return s + to_string(i); }
    inline cstr operator+(cstr s, long i) { return s + to_string(i); }

    const int MINSIZE = 1 << 10;

/* RAW, when defined, says to go with a less-safe, faster implementation
 *    than STL vectors, with no bounds check, and less header info, to
 *    save a little space. The fast-copy cell heap-to-heap relocation may
 *    end up in this class eventually.
 */
// #define RAW
#ifdef RAW
    const bool is_raw = true;
#else
    const bool is_raw = false;
#endif
}

