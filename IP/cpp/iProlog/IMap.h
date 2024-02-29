#pragma once
/*
* iProlog / C++  [derived from Java version]
* License: Apache 2.0
* Copyright(c) 2017 Paul Tarau
*/

// for hashing implementation with a template, see:
//  https://marknelson.us/posts/2011/09/03/hash-functions-for-c-unordered-containers.html

#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "cell.h"
#include "index.h"
#include "IntMap.h"
#include "Integer.h"

namespace iProlog {

    using namespace std; 

class IMap {

        static const unsigned NBUCKETS_2_exp = 4;
        static const int NBUCKETS = 1 << NBUCKETS_2_exp;
        static const int NBUCKETS_mask = NBUCKETS + 1;

        // Should parameterize this further to avoid sizeof(pointer) = 8
        // periodicity on 128-bit machines
         static int phash(const Integer *s) {
             size_t x = (size_t) s;
             return NBUCKETS_mask  & ((x >> 10) ^ (x >> 2));
         }

         struct bucket {
             const Integer* key;
             IntMap<int,int>* vals;
             bucket() { key = nullptr; vals = nullptr; }
             bucket(const Integer* vec_elt_obj, IntMap<int,int>* vs) : key(vec_elt_obj), vals(vs) {}
         };
     
         vector<bucket> map;
public:
      IMap() {
            map = vector<bucket>(NBUCKETS);
            map.clear();
      }
      inline void clear() { map.clear(); }
      bool put(const Integer* vec_elt_obj, ClauseNumber clause_no);
      IntMap<int,int>* get(const Integer* vec_elt_obj) const;
      static vector<IMap*> create(int l);
      size_t size();

// refactor out, for micro version, or keep but
// conditionally compiled for that version:

      string toString() const;
      string show();
      static string show(const vector<IMap*> &imaps);
      static string show(const bucket &b);
      static string show(const vector<Integer *> is);
};
} // end namespace
