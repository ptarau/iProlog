#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <climits>
#include <stdexcept>
#include <stdint.h>
#include <assert.h>

#include "config.h"

namespace iProlog {

    using namespace std;

    // I need a more specific header file for this:
    // typedef int ClauseNumber; /* 1 ... clause array size */

    // If one of the goals is to avoid bringing in the
    // string library, this might better be put elsewhere.
    typedef const string cstr;
    inline cstr operator+(cstr s, int i) { return s + to_string(i); }
    inline cstr operator+(cstr s, size_t i) { return s + to_string(i); }
    inline cstr operator+(cstr s, long i) { return s + to_string(i); }
}

