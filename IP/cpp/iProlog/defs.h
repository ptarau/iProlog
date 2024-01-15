#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <memory>
#include "Integer.h"
#include "CellStack.h"

const int bitwidth = CHAR_BIT * sizeof(int);

using namespace std;

namespace iProlog {
    typedef const string cstr;
    inline cstr operator+(cstr s, int i) { return s + to_string(i); }
    inline cstr operator+(cstr s, size_t i) { return s + to_string(i); }
    inline cstr operator+(cstr s, long i) { return s + to_string(i); }

    const int MINSIZE = 1 << 10;
    const int MAXIND = 3;       // "number of index args" [Engine.java]
    const int START_INDEX = 1; // "if # of clauses < START_INDEX, turn off indexing" [Engine.java]

    typedef array<int, MAXIND> t_index_vector; // deref'd cells
}

