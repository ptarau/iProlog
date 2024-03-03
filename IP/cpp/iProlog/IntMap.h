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

#include <iostream>



namespace iProlog {

	using namespace std;

#if 0
	template <class Key, class Value, Key free_key> class IntMap {
#else
    template <class Key, class Value> class IntMap {
#endif
	public:
		static const Value NO_VALUE = 0;
		static const Key   FREE_KEY = 0;
	private:
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

		inline void move_to_next_entry(int& p) const {
			p = (p + m_stride) & m_mask2;  // masking causes wraparound indexing
		}
		inline void make_masks(int cap) { // arg must be 2^n for some n
			m_mask = cap / m_stride - 1;
			m_mask2 = cap - 1;
		}
		inline void alloc(int cap) {
			m_data = vector<int>(cap * m_stride);
		}


        // newCapacity should be 2^n for some n

        void rehash(size_t newCapacity) {
            m_threshold = (int)(newCapacity / 2 * m_fillFactor);
            make_masks(newCapacity);

            size_t      oldCapacity = m_data.capacity();
            vector<int> oldData = m_data;

            alloc(newCapacity);
            m_size = m_hasFreeKey ? 1 : 0;

            for (int i = 0; i < oldCapacity; i += m_stride) {
                int oldKey = oldData[i];
                if (!is_free(oldKey)) {
                    put(oldKey, oldData[i + 1]);
                }
            }
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
		inline Key   get_k(int p) const    { return m_data[p]; }
		inline void  set_v(int p, Value v) { m_data[p + 1] = v; }
		inline Value get_v(int p) const    { return m_data[p + 1]; }

		inline int hash_pos(Key key) const {
			return (FastUtil::phiMix(key) & m_mask) << (m_stride - 1);
		}

	public:

		inline Key get_key_at(int p) const { return get_k(p); }

		inline int size() const { return m_size; }
		
		// inline int& operator[](int i)
		inline Value& lval(int i) 
		{
			return m_data[i];
		}
#if 0
		IntMap();
		IntMap(int size);
		IntMap(int size, float fillFactor);

		Value get(Key key) const;
		Value put(Key key, Value value);
		Value remove(Key key);
#endif
		inline bool add(Key key)            { return NO_VALUE != put(key, 666); }
		inline bool contains(Key key) const { return NO_VALUE != get(key);      }
		inline bool retract(Key key)        { return NO_VALUE != remove(key);   }
		inline bool isEmpty() const         { return 0 == m_size;               }

		// some kind of inlined "bool get_next_key(int &p, Key &k)" semi-iterator
		// would be better than this -- it could hide these three, until the
		// kv_pair rewrite is worked out for IntMap.
		inline size_t capacity() const      { return m_data.capacity();         }
		inline int stride() const           { return m_stride;                  }
#if 0
		inline bool is_free(Key k) const     { return k == free_key;             }
#else
        inline bool is_free(Key k) const { return k == FREE_KEY; }
#endif

#if 0
		// avoid dragging in string library
		string toString();
#endif






        IntMap() : IntMap(1 << 2) { }


        IntMap(int size) : IntMap(size, 0.75f) { };


        IntMap(int size, float fillFactor) {
            if (fillFactor <= 0 || fillFactor >= 1)
                throw std::invalid_argument("FillFactor must be in (0, 1)");
            if (size <= 0)
                throw std::invalid_argument("Size must be positive!");

            int capacity = FastUtil::arraySize(size, fillFactor);
            make_masks(capacity);
            m_fillFactor = fillFactor;

            cout << "////// about to call alloc in IntMap with capacity=" << capacity << endl;
            alloc(capacity);
            m_threshold = (int)(capacity * fillFactor);
            m_size = 0; // added -- MT
            m_hasFreeKey = false;
        }

        Value get(Key key) const {
            if (is_free(key))
                return m_hasFreeKey ? m_freeValue : NO_VALUE;

            int ptr = hash_pos(key);
            int k = get_k(ptr);

            if (is_free(k))
                return NO_VALUE; // "end of chain already"
            if (k == key) // "we check FREE prior to this call" ???
                return (Value)get_v(ptr);

            while (true) {
                move_to_next_entry(ptr);
                k = get_k(ptr);
                if (is_free(k))
                    return (Value)NO_VALUE;
                if (k == key)
                    return (Value)get_v(ptr);
            }
        }

        Value put(Key key, Value value) {
            if (is_free(key)) {
                int ret = m_freeValue;
                if (!m_hasFreeKey)
                    ++m_size;
                m_hasFreeKey = true;
                m_freeValue = value;
                return ret;
            }

            int ptr = hash_pos(key);
            int k = get_k(ptr);
            if (is_free(k)) { // "end of chain already"
                set_kv(ptr, key, value);
                maybe_resize();
                return NO_VALUE;
            }
            else
                if (k == key) { // "we check FREE prior to this call"
                    int ret = get_v(ptr);
                    set_v(ptr, value);
                    return ret;
                }

            while (true) {
                move_to_next_entry(ptr); //that's next index calculation
                k = get_k(ptr);
                if (is_free(k)) {
                    set_kv(ptr, key, value);
                    maybe_resize();
                    cout << "NO_VALUE=" << NO_VALUE << endl;
                    return NO_VALUE;
                }
                else
                    if (k == key) {
                        int ret = get_v(ptr);
                        set_v(ptr, value);
                        return ret;
                    }
            }
        }

        Value remove(Key key) {
            if (is_free(key)) {
                if (!m_hasFreeKey)
                    return NO_VALUE;
                m_hasFreeKey = false;
                --m_size;
                return m_freeValue; //value is not cleaned
            }

            int ptr = hash_pos(key);
            int k = get_k(ptr);
            if (k == key) { // "we check FREE prior to this call" ???
                int res = get_v(ptr);
                shiftKeys(ptr);
                --m_size;
                return res;
            }
            else
                if (is_free(k))
                    return NO_VALUE; // "end of chain already"

            while (true) {
                move_to_next_entry(ptr);
                k = get_k(ptr);
                if (k == key) {
                    int res = get_v(ptr);
                    shiftKeys(ptr);
                    --m_size;
                    return res;
                }
                else
                    if (is_free(k))
                        return NO_VALUE;
            }
        }

        void shiftKeys(int pos) {
            // "Shift entries with the same hash."
            int last, slot, k;

            while (true) {
                last = pos;
                move_to_next_entry(pos);
                while (true) {
                    k = get_k(pos);
                    if (is_free(k)) {
                        set_k(last, FREE_KEY);
                        return;
                    }
                    slot = hash_pos(k);
                    if (last <= pos ? (last >= slot || slot > pos) : (last >= slot && slot > pos)) {
                        break;
                    }
                    move_to_next_entry(pos);
                }
                set_kv(last, k, get_v(pos));
            }
        }


        // @Override

        string toString() {
            //return java.util.Arrays.toString(m_data);
            string b = string("{");
            size_t l = m_data.size() / m_stride;
            int first = true;
            for (int i = 0; i < l; i += m_stride) {
                int k = get_k(i);
                if (!is_free(k)) {
                    if (!first) {
                        b.append(",");
                    }
                    first = false;
                    b.append(to_string(k - 1));
                }
            }
            b.append("}");
            return b;
        }

	};
}

