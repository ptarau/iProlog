#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <climits>
#include <stdexcept>

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

#ifdef TRY_CATCHING
#define TRY try
#define CATCH(s) catch (exception &e__) { cout<<"!!!"<<s<<": "<<e__.what()<<endl; abort(); }
#else
#define TRY
#define CATCH(S)
#endif

    void set_engine(void *ep);
    void checkit();

}

