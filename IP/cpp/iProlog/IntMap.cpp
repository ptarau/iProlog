/*
* iProlog / C++  [derived from Java version]
* License: Apache 2.0
* Copyright(c) 2017 Paul Tarau
*/

/**
 * derived from code at https://github.com/mikvor/hashmapTest
 * "for use as IntSet" [IntMap.java]
 */

// #include <cmath>   // some architecture-specific namespace issue here

#include <iostream>
#include "IntMap.h"

namespace iProlog {

    using namespace std;


  template<class Value>
  IntMap<Value>::IntMap() : IntMap<Value>::IntMap(1 << 2) { }

  template<class Value>
  IntMap<Value>::IntMap(int size) : IntMap<Value>::IntMap(size, 0.75f) {

// cout<<"@@@@@@@@@@ IntMap(int "<<size<<") constr, size="<<size
//     << " m_size=" << m_size << endl;

  };

  template<class Value>
  IntMap<Value>::IntMap(int size, float fillFactor) {

// cout<< "        IntMap(int size, float fillFactor), size="<<size<<endl;

    if (fillFactor <= 0 || fillFactor >= 1)
            throw std::invalid_argument("FillFactor must be in (0, 1)");
    if (size <= 0)
            throw std::invalid_argument("Size must be positive!");
    size_t capacity = arraySize(size, fillFactor);
    m_mask = capacity - 1;
    m_mask2 = capacity * 2 - 1;
    m_fillFactor = fillFactor;

    m_data = vector<int>(capacity * 2);
    m_threshold = (int) (capacity * fillFactor);
    m_size = 0; // added
    m_hasFreeKey = 0;

// cout << "      at end of IntMap(size,fillFactor), m_size=" << m_size << endl;
  }

  template<class Value>
  Value IntMap<Value>::get(int key) {
    int ptr = (phiMix(key) & m_mask) << 1;

    if (key == FREE_KEY)
      return m_hasFreeKey ? m_freeValue : NO_VALUE;

    int k = m_data[ptr];

    if (k == FREE_KEY)
      return NO_VALUE; //end of chain already
    if (k == key) //we check FREE prior to this call
      return m_data[ptr + 1];

    while (true) {
      ptr = ptr + 2 & m_mask2; //that's next index
      k = m_data[ptr];
      if (k == FREE_KEY)
        return NO_VALUE;
      if (k == key)
        return m_data[ptr + 1];
    }
  }

  template<class Value>
  bool IntMap<Value>::contains(int key) {
    return NO_VALUE != get(key);
  }

  template<class Value>
  bool IntMap<Value>::add(int key) {
    return NO_VALUE != put(key, 666);
  }

  template<class Value>
  bool IntMap<Value>::delete_(int key) {
    return NO_VALUE != remove(key);
  }

  template<class Value>
  int IntMap<Value>::isEmpty() {
    return 0 == m_size;
  }

  template<class Value>
  void IntMap<Value>::intersect0(IntMap<Value> &m,
				 	vector<IntMap<Value>> &maps,
				 	vector<IntMap<Value>> &vmaps,
				 	vector<int> &r) {
    vector<int> data = m.m_data;
    for (int k = 0; k < data.capacity(); k += 2) {
      int found = true;
      int key = data[k];
      if (FREE_KEY == key) {
        continue;
      }
      for (int i = 1; i < maps.capacity(); i++) {
        IntMap<Value> map = maps[i];
        int val = map.get(key);

        if (NO_VALUE == val) {
          IntMap<Value> vmap = vmaps[i];
          int vval = vmap.get(key);
          if (NO_VALUE == vval) {
            found = false;
            break;
          }
        }
      }
      if (found) {
        r.push_back(key);
      }
    }
  }

  template<class Value>
  vector<Value> IntMap<Value>::intersect(vector<IntMap<Value>> &maps,
				         vector<IntMap<Value>> &vmaps) {
    vector<int> r = vector<int>();

cout<<"------------intersect: maps"<<endl;
    for (int i = 0; i < maps.size(); ++i) {
cout<<"------------  maps["<<i<<"]:"<<endl;
cout<<"------------  ";
	for(int j = 0; j < maps[i].m_data.size(); ++j) {
cout<<maps[i].m_data[j] <<" ";
	}
cout << endl;
    }

cout<<"------------intersect: maps"<<endl;
    for (int i = 0; i < vmaps.size(); ++i) {
cout<<"------------  vmaps["<<i<<"]:"<<endl;
cout<<"------------  ";
	for(int j = 0; j < vmaps[i].m_data.size(); ++j) {
cout<<vmaps[i].m_data[j] <<" ";
	}
cout << endl;
    }

    intersect0(maps[0], maps, vmaps, r);
    intersect0(vmaps[0], maps, vmaps, r);
    return r;
  }

  // "end changes" (where?)

  template<class Value>
  Value IntMap<Value>::put(int key, Value value) {
    if (key == FREE_KEY) {
      int ret = m_freeValue;
      if (!m_hasFreeKey) {
        ++m_size;
      }
      m_hasFreeKey = true;
      m_freeValue = value;
      return ret;
    }

    int ptr = (phiMix(key) & m_mask) << 1;
    int k = m_data[ptr];
    if (k == FREE_KEY) //end of chain already
    {
      m_data[ptr] = key;
      m_data[ptr + 1] = value;
      if (m_size >= m_threshold) {
        IntMap<Value>::rehash(size_t(m_data.capacity() * 2)); //size is set inside
      } else {
        ++m_size;
      }
      return NO_VALUE;
    } else if (k == key) //we check FREE prior to this call
    {
      int ret = m_data[ptr + 1];
      m_data[ptr + 1] = value;
      return ret;
    }

    while (true) {
      ptr = ptr + 2 & m_mask2; //that's next index calculation
      k = m_data[ptr];
      if (k == FREE_KEY) {
        m_data[ptr] = key;
        m_data[ptr + 1] = value;
        if (m_size >= m_threshold) {
            IntMap<Value>::rehash(size_t(m_data.size() * 2)); //size is set inside
        } else {
          ++m_size;
        }
        return NO_VALUE;
      } else if (k == key) {
        int ret = m_data[ptr + 1];
        m_data[ptr + 1] = value;
        return ret;
      }
    }
  }

  template<class Value>
  Value IntMap<Value>::remove(int key) {
    if (key == FREE_KEY) {
      if (!m_hasFreeKey)
        return NO_VALUE;
      m_hasFreeKey = false;
      --m_size;
      return m_freeValue; //value is not cleaned
    }

    int ptr = (phiMix(key) & m_mask) << 1;
    int k = m_data[ptr];
    if (k == key) //we check FREE prior to this call
    {
      int res = m_data[ptr + 1];
      IntMap<Value>::shiftKeys(ptr);
      --m_size;
      return res;
    } else if (k == FREE_KEY)
      return NO_VALUE; //end of chain already
    while (true) {
      ptr = ptr + 2 & m_mask2; //that's next index calculation
      k = m_data[ptr];
      if (k == key) {
        int res = m_data[ptr + 1];
        IntMap<Value>::shiftKeys(ptr);
        --m_size;
        return res;
      } else if (k == FREE_KEY)
        return NO_VALUE;
    }
  }

  template<class Value>
  int IntMap<Value>::shiftKeys(int pos) {
    // Shift entries with the same hash.
    int last, slot;
    int k;
    vector<int> data = m_data;
    while (true) {
      pos = (last = pos) + 2 & m_mask2;
      while (true) {
        if ((k = data[pos]) == FREE_KEY) {
          data[last] = FREE_KEY;
          return last;
        }
        slot = (phiMix(k) & m_mask) << 1; //calculate the starting slot for the current key
        if (last <= pos ? last >= slot || slot > pos : last >= slot && slot > pos) {
          break;
        }
        pos = pos + 2 & m_mask2; //go to the next entry
      }
      data[last] = k;
      data[last + 1] = data[pos + 1];
    }
  }

  template<class Value>
  int IntMap<Value>::size() {
    return m_size;
  }

  template<class Value>
  void IntMap<Value>::rehash(size_t newCapacity) {
    m_threshold = (newCapacity / 2 * m_fillFactor);
    m_mask = newCapacity / 2 - 1;
    m_mask2 = newCapacity - 1;

    size_t oldCapacity = m_data.capacity();
    vector<int> oldData = m_data;

    m_data = vector<int>(newCapacity);
    m_size = m_hasFreeKey ? 1 : 0;

    for (int i = 0; i < oldCapacity; i += 2) {
      int oldKey = oldData[i];
      if (oldKey != FREE_KEY) {
        put(oldKey, oldData[i + 1]);
      }
    }
  }

  /** Taken from FastUtil implementation */

  /** Return the least power of two greater than or equal to the specified value.
   *
   * <p>Note that this function will return 1 when the argument is 0.
   *
   * @param x a long integer smaller than or equal to 2<sup>62</sup>.
   * @return the least power of two greater than or equal to the specified value.
   */

  template<class Value>
  size_t IntMap<Value>::nextPowerOfTwo(size_t x) {
    if (x == 0)
      return 1;
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    if (sizeof(x) > 4)
       x |= x >> 32;
    return x+1;
  }

  /** Returns the least power of two smaller than or equal to 2<sup>30</sup>
   * and larger than or equal to <code>Math.ceil( expected / f )</code>.
   *
   * @param expected the expected number of elements in a hash table.
   * @param f the load factor.
   * @return the minimum possible size for a backing array.
   * @throws IllegalArgumentException if the necessary size is larger than 2<sup>30</sup>.
   */
  template<class Value>
  int IntMap<Value>::arraySize(int expected, float f) {
    long s = nextPowerOfTwo((long) ((expected / f)+1)); 
    if (s < 2) s = 2;

    if (s > 1L << 30) {
      throw invalid_argument("Too large (< + expected + > expected elements with load factor < + f + >)");
    }
    return s;
  }

  //taken from FastUtil
  static const int INT_PHI = 0x9E3779B9;

  template<class Value>
  int IntMap<Value>::phiMix(int x) {
    int h = x * INT_PHI;
    return h ^ h >> 16;
  }

  // @Override
  
  template<class Value>
  string IntMap<Value>::toString() {
    //return java.util.Arrays.toString(m_data);
    string b = string("{");
    size_t l = m_data.size();
    int first = true;
    for (int i = 0; i < l; i += 2) {

      int v = m_data[i];
      if (v != FREE_KEY) {
        if (!first) {
          
          b.append(",");
        }
        first = false;
        b.append(to_string(v - 1));
      }
    }
    b.append("}");
    return b;
  }

// (See https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor)
// to make sure of compiling:

template class IntMap<int>; // mysterious....

// ...but I can also just make the whole thing a header file
// There will really be only two uses of IntMap(X):
//   X for imaps -- dereferenced cells(???)
//   X for var_maps -- clause #s(???)
}

