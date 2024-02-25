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
#include "IntMap.h"
#include "Integer.h"

namespace iProlog {

    using namespace std; 

class IMap {
public:
        static const int NBUCKETS = 16;

         static size_t phash(Integer *s) {
             size_t x = (size_t) s;
             return (size_t) (0xF & ((x >> 10) ^ (x >> 2)));
         }

         struct bucket {
             Integer* key;
             IntMap<int,int>* vals;
             bucket() { key = nullptr; vals = nullptr; }
             bucket(Integer* Ip, IntMap<int,int>* vs) : key(Ip), vals(vs) {}
         };
     
         vector<bucket> map;

      IMap() {
            map = vector<bucket>(NBUCKETS);
            map.clear();
      }

      inline void clear() { map.clear(); }

      static inline int  to_clause_no(int index)      { return index + 1;      }
      static inline int  to_clause_idx(int clause_no) { return clause_no - 1;  }
      static inline bool is_var_arg(int clause_no)    { return clause_no == 0; }

      Integer *put(Integer* key, int clause_no);
      IntMap<int,int>* get(Integer* key);
      size_t size();
      string toString();
      static vector<IMap*> create(int l);
      static Integer * put_(    vector<IMap*> &imaps,
                                int pos,
                                cell derefd,
                                int clause_no );
      static vector<int> getn(  vector<IMap*> &iMaps,
		                        vector<IntMap<int,int>*> &vmaps,
		                        vector<int> &unifiables );
      string show();
      static string show(vector<IMap*> &imaps);
      static string show(bucket &b);
      static string show(vector<Integer *> is);
};
} // end namespace
