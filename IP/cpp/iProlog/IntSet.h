#pragma once

/* "for use as IntSet" - Paul Tarau
 * iProlog / C++  [derived from Java version]
 * Original (Java) author: Mikhail Vorontsov
 * License: public domain
 *    https://github.com/mikvor/hashmapTest/blob/master/LICENSE
 * Java code modified by Paul Tarau
 * 
 * derived from code at https://github.com/mikvor/hashmapTest
 *
 * Article by Vorontsov:
 * https://web.archive.org/web/20170606024914/http://java-performance.info/implementing-world-fastest-java-int-to-int-hash-map/
 */

#include <cstddef>
#include <assert.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <system_error>
#include "defs.h"

#include <iostream>

#include "IMap.h"

namespace iProlog {
    class is_bucket {
    public:
        int key;
        int val;
    };

    typedef is_bucket* Vec;

    using namespace std;

    // For short logic programs, these are already pretty bulky
    // compared to their actual indexing maps.
    // Parameterizing for different kinds of maps
    // has an additional cost.
    // Unsatisfactory, but solvable with C++ "concept"?
    // If I stick with m_data size = 2^n, what are now some per-instance
    // members could be separated into a "hash table header" class
    // and each could have an index into a table of these.

    class IntSet {
      private:

        static const  int FREE_KEY = 0;
        static const  int NO_VALUE = 0;

        static const  int init_cap = (1 << 2);

        /** Fill factor, must be between (0 and 100) */
        static const int m_fillFactor_pc = 75;
        /** We will resize a map once it reaches this size */
        int m_threshold;

        /** Keys and values */
        Vec m_data;

    private:
        /** Do we have 'free' key in the map? */
        bool m_hasFreeKey;
        /** Value of 'free' key */
        int m_freeValue;

        /** Current map size */
        int m_size;

        /** Mask to calculate the original position */
        int m_mask;

        /** number of elts if ever full */
        size_t md_capacity;

    public:
        IntSet();
        ~IntSet() { md_free(m_data); }

        inline static int no_value()      { return NO_VALUE;    } 
        inline size_t     length() const  { return md_capacity; }

        inline bool is_free(int i)           const { return m_data[i].key == FREE_KEY; }
        inline int  get_key_at(int i)        const { return m_data[i].key; }
        inline void set_key_at(int i, int v)       { m_data[i].key = v; }
        inline int  get_val_at(int i)        const { return m_data[i].val; }
        inline void set_val_at(int i, int v)       { m_data[i].val = v; }

        inline bool contains(int key) const { return NO_VALUE != get(key); }
        inline bool add_key(int key) { return NO_VALUE != put(key, 666); }
        // inline bool delete_(int key)        { return NO_VALUE != remove(key); }
        inline bool isEmpty()         const { return 0 == m_size; }
        inline int  size()            const { return m_size; }
        
        static IntSet* alloc_intset() { return (IntSet*)malloc(sizeof(IntSet)); }
        static IntSet* init(IntSet *isp);
        static void dealloc(IntSet* isp);

        int get(int key) const;
        int put(int key, int value);
        // int remove(int key);
    private:
        inline void md_free(Vec vs) {
            free(vs);
        }
        inline int  wraparound(int i) const { return i & m_mask;             }
        inline int  next(int ptr)     const { return wraparound(ptr+1);      }
        inline void set_mask(int cap)       { m_mask = cap - 1;              }
        inline int  mk_ptr(int key)   const { return (phiMix(key) & m_mask); }

        inline Vec alloc(int cap) {
            md_capacity = cap;
            size_t sz = cap * sizeof(is_bucket);
            void* p = malloc(sz);
            memset(p, 0, sz);
            return (Vec) p;
        }

        static long nextPowerOfTwo(long x);
        static int arraySize(int expected, int f_pc);
        int shiftKeys(int pos);
        void rehash(size_t newCapacity);

        //taken from FastUtil
        static const int INT_PHI = 0x9E3779B9;

        inline int phiMix(int x) const {
            int h = x * INT_PHI;
            return h ^ h >> 16;
        }

    public:
        string show() const;
  };
}