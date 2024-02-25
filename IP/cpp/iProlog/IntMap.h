#pragma once

/*
* iProlog / C++  [derived from Java version]
* Original author: Mikhail Vorontsov
* License: public domain
*    https://github.com/mikvor/hashmapTest/blob/master/LICENSE
* Java code modified by Paul Tarau
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
#include "FastUtil.h"

namespace iProlog {

	using namespace std;

	// typedef int Key;

	template <class Key, class Value> class IntMap {
	private:
		static const Value NO_VALUE = 0;
		static const Key   FREE_KEY = 0;

		/* eventually refactor to this, with "Key key" */
		struct kv_pair {
		   Key   key; 
		   Value val;
		};

		/** "Keys and values" */
		/** (Alternating k0,v0,k1,v1....) */
		std::vector<int> m_data;

		/** "Do we have 'free' key in the map?" */
		bool m_hasFreeKey;
		/** Value of 'free' key */
		Value m_freeValue;

		/** "Fill factor, must be between (0 and 1)" */
		float m_fillFactor;
		/** "We will resize a map once it reaches this size" */
		int m_threshold;
		/** "Current map size" */
		/** This is # of entries, not # of ints */
		int m_size;

		/** "Mask to calculate the original position" */
		/** (m_stride == 1 i.e., kv_pair array, will make these two equal */
		int m_mask;
		int m_mask2;

		/** Switch for int/kv_pair array traversal, allocation, etc. */
		static const int m_stride = 2; // 2=int array, 1=kv_pair array

		void shiftKeys(int pos);
		void rehash(size_t newCapacity);

		inline void move_to_next_entry(int& p) {
			p = (p + m_stride) & m_mask2;  // masking causes wraparound indexing
		}
		inline void make_masks(int cap) { // arg must be 2^n for some n
			m_mask = cap / m_stride - 1;
			m_mask2 = cap - 1;
		}
		inline void alloc(int cap) {
			m_data = vector<int>(cap * m_stride);
		}

		inline void maybe_resize() {
			if (m_size >= m_threshold)
				rehash(size_t(m_data.capacity() * 2)); // double each time; "size is set inside"
			else
				++m_size;
		}
		inline void set_kv(int p, Key k, Value v) {
			m_data[p] = k;
			m_data[p + 1] = v;
		}

		inline void  set_k(int p, Key k)   { m_data[p] = k; }
		inline Key   get_k(int p)          { return m_data[p]; }
		inline void  set_v(int p, Value v) { m_data[p + 1] = v; }
		inline Value get_v(int p)          { return m_data[p + 1]; }

		inline int hash_pos(Key key) {
			return (FastUtil::phiMix(key) & m_mask) << (m_stride - 1);
		}

	public:

		inline Key get_key_at(int p) { return get_k(p); }

		inline int size() { return m_size; }
		
		// inline int& operator[](int i)
		inline Value& lval(int i)
		{
			return m_data[i];
		}

		IntMap();
		IntMap(int size);
		IntMap(int size, float fillFactor);

		Value get(Key key);
		Value put(Key key, Value value);
		Value remove(Key key);

		inline bool add(Key key)      { return NO_VALUE != put(key, 666); }
		inline bool contains(Key key) { return NO_VALUE != get(key);      }
		inline bool retract(Key key)  { return NO_VALUE != remove(key);   }
		inline bool isEmpty()         { return 0 == m_size;               }

		// some kind of inlined "bool get_next_key(int &p, Key &k)" semi-iterator
		// would be better than this -- it could hide these three, until the
		// kv_pair rewrite is worked out for IntMap.
		inline size_t capacity()      { return m_data.capacity();         }
		inline int stride()           { return m_stride;                  }
		inline bool is_free(Key k)    { return k == FREE_KEY;             }

		string toString();
	};
}
