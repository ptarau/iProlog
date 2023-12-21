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
#include "CellStack.h"

namespace iProlog {

	using namespace std;

	class IntMap /* implements java.io.Serializable */ {
	private:
		static const int NO_VALUE = 0;
		static const int FREE_KEY = 0;

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
		size_t m_size;

		/** Mask to calculate the original position */
		size_t m_mask;
		size_t m_mask2;
	public:
		inline int& operator[](int i) { return m_data[i]; }
		IntMap();
		IntMap(int size);
		IntMap(int size, float fillFactor);
		int get(int key);
		int contains(int key);
		int add(int key);
		int delete_(int key);
		int isEmpty();
		static void intersect0(IntMap& m, vector<IntMap>& maps, vector<IntMap>& vmaps, vector<int>& r);
		static vector<int> intersect(vector<IntMap>& maps, vector<IntMap>& vmaps);
		int put(int key, int value);
		int remove(int key);
		string toString();

		size_t arraySize(int expected, float f);
		static size_t nextPowerOfTwo(size_t x);
		static int phiMix(int x);
		int shiftKeys(int pos);
		void rehash(size_t newCapacity);
		size_t size();
	};
}