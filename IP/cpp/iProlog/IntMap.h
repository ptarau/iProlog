#pragma once

/* iProlog / C++[derived from Java version]
 * License: Apache 2.0
 * Copyright(c) 2017 Paul Tarau
 */
#include <cstddef>
#include <assert.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <system_error>
#include "defs.h"

namespace iProlog {

	using namespace std;

	template <class Value> class IntMap {
	private:
		static const int NO_VALUE = 0;
		static const int FREE_KEY = 0;

		struct kv_pair {
		   int key; 
		   Value val;
		};

		/** Keys and values */
		std::vector<int> m_data;

		/** Do we have 'free' key in the map? */
		int m_hasFreeKey;
		/** Value of 'free' key */
		int m_freeValue;

		/** Fill factor, must be between (0 and 1) */
		float m_fillFactor;
		/** We will resize a map once it reaches this size */
		size_t m_threshold;
		/** Current map size */
		// size_t m_size;
		int m_size;

		/** Mask to calculate the original position */
		size_t m_mask;
		size_t m_mask2;
	public:
		// inline int& operator[](int i)
		inline Value& lval(int i)
		{
			return m_data[i];
		}
		IntMap();
		IntMap(int size);
		IntMap(int size, float fillFactor);
		Value get(int key);
		bool contains(int key);
		bool add(int key);
		bool delete_(int key);
		int isEmpty();
		static void intersect0(
				       IntMap& m,
				       vector<IntMap>& maps,
				       vector<IntMap>& vmaps,
				       vector<int>& r);
		static vector<Value> intersect(vector<IntMap>& maps,
					       vector<IntMap>& vmaps);
		Value put(int key, Value value);
		Value remove(int key);
		string toString();

		int arraySize(int expected, float f);
		static size_t nextPowerOfTwo(size_t x);
		static int phiMix(int x);
		int shiftKeys(int pos);
		void rehash(size_t newCapacity);

		// size_t size();
		int size();
	};
}
