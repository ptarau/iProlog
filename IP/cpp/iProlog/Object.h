#pragma once
// not sure how derivative of iProlog this is
#include <vector>
#include <string>
#include "defs.h"

namespace iProlog {

    using namespace std;

        struct Object {
            enum { e_nullptr, e_integer, e_string, e_vector } type;
            int i;
            string s;
            vector<Object> v;

            Object() : type(e_nullptr), i(0), s("") {}

            int& operator =(const int j) { i = j; type = e_integer; return i; }
            explicit Object(int i) : type(e_integer), i(i) {}

            string& operator =(const string s_new) { type = e_string, s = s_new; return s; }
            explicit Object(string s) : type(e_string), i(int(0)), s(s) {}

            vector<Object>& operator =(const vector<Object> vo) { type = e_vector; v = vo; return v; }
            explicit Object(vector<Object> v) : type(e_vector), i(0), v(v) {}

            // avoid bringing in string lib:
            string toString() const;
    };
} // end namespace
