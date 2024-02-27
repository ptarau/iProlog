/*
* iProlog / C++  [derived from Java version]
* IntMap original author: Mikhail Vorontsov
* License: public domain
*    https://github.com/mikvor/hashmapTest/blob/master/LICENSE
* Java code modified by Paul Tarau
*
 * Article by Vorontsov:
 * https://web.archive.org/web/20170606024914/http://java-performance.info/implementing-world-fastest-java-int-to-int-hash-map/
 */

// #include <cmath>   // an architecture-specific C++ namespace issue here

#include <iostream>
#include "IntMap.h"

namespace iProlog {

    using namespace std;

  template<class Key, class Value>
  IntMap<Key,Value>::IntMap() : IntMap<Key,Value>::IntMap(1 << 2) { }

  template<class Key, class Value>
  IntMap<Key,Value>::IntMap(int size) : IntMap<Key,Value>::IntMap(size, 0.75f) { };

  template<class Key, class Value>
  IntMap<Key,Value>::IntMap(int size, float fillFactor) {
    if (fillFactor <= 0 || fillFactor >= 1)
            throw std::invalid_argument("FillFactor must be in (0, 1)");
    if (size <= 0)
            throw std::invalid_argument("Size must be positive!");

    int capacity = FastUtil::arraySize(size, fillFactor);
    make_masks(capacity);
    m_fillFactor = fillFactor;

    alloc(capacity);
    m_threshold = (int) (capacity * fillFactor);
    m_size = 0; // added -- MT
    m_hasFreeKey = false;
  }

  template<class Key,class Value>
  Value IntMap<Key,Value>::get(Key key) const {
      if (key == FREE_KEY)
          return m_hasFreeKey ? m_freeValue : NO_VALUE;

      int ptr = hash_pos(key);
      int k = get_k(ptr);

      if (k == FREE_KEY)
            return NO_VALUE; // "end of chain already"
      if (k == key) // "we check FREE prior to this call" ???
            return (Value) get_v(ptr);

      while (true) {
        move_to_next_entry(ptr);
        k = get_k(ptr);
        if (k == FREE_KEY)
          return (Value) NO_VALUE;
        if (k == key)
          return (Value) get_v(ptr);
      }
  }

  template<class Key, class Value>
  Value IntMap<Key, Value>::put(Key key, Value value) {
    if (key == FREE_KEY) {
      int ret = m_freeValue;
      if (!m_hasFreeKey)
        ++m_size;
      m_hasFreeKey = true;
      m_freeValue = value;
      return ret;
    }

    int ptr = hash_pos(key);
    int k = get_k(ptr);
    if (k == FREE_KEY) { // "end of chain already"
        set_kv(ptr, key, value);
        maybe_resize(); 
        return NO_VALUE;
    } else
    if (k == key) { // "we check FREE prior to this call"
          int ret = get_v(ptr);
          set_v(ptr, value);
          return ret;
    }

    while (true) {
      move_to_next_entry(ptr); //that's next index calculation
      k = get_k(ptr);
      if (k == FREE_KEY) {
          set_kv(ptr, key, value);
          maybe_resize();        
          cout << "NO_VALUE=" << NO_VALUE << endl;
          return NO_VALUE;
      } else
      if (k == key) {
            int ret = get_v(ptr);
            set_v(ptr, value);
            return ret;
      }
    }
  }

  template<class Key, class Value>
  Value IntMap<Key,Value>::remove(Key key) {
    if (key == FREE_KEY) {
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
    } else
    if (k == FREE_KEY)
      return NO_VALUE; // "end of chain already"

    while (true) {
      move_to_next_entry(ptr);
      k = get_k(ptr);
      if (k == key) {
          int res = get_v(ptr);
          shiftKeys(ptr);
          --m_size;
          return res;
      } else
      if (k == FREE_KEY)
          return NO_VALUE;
    }
  }

  template<class Key, class Value>
  void IntMap<Key,Value>::shiftKeys(int pos) {
    // "Shift entries with the same hash."
    int last, slot, k;

    while (true) {
        last = pos;
        move_to_next_entry(pos);
        while (true) {
            k = get_k(pos);
            if (k == FREE_KEY) {
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

  // newCapacity should be 2^n for some n

  template<class Key, class Value>
  void IntMap<Key,Value>::rehash(size_t newCapacity) {
    m_threshold = (newCapacity / 2 * m_fillFactor);
    make_masks(newCapacity);

    size_t      oldCapacity = m_data.capacity();
    vector<int> oldData     = m_data;

    alloc(newCapacity);
    m_size = m_hasFreeKey ? 1 : 0;

    for (int i = 0; i < oldCapacity; i += m_stride) {
      int oldKey = oldData[i];
      if (oldKey != FREE_KEY) {
        put(oldKey, oldData[i + 1]);
      }
    }
  }

  

  // @Override
  
  template<class Key, class Value>
  string IntMap<Key,Value>::toString() {
    //return java.util.Arrays.toString(m_data);
    string b = string("{");
    size_t l = m_data.size() / m_stride;
    int first = true;
    for (int i = 0; i < l; i += m_stride) {
      int k = get_k(i);
      if (k != FREE_KEY) {
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

// (See https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor)
// to make sure of compiling:

template class IntMap<int,int>; // mysterious....

// ...but I can also just make the whole thing a header file
// There will really be only two uses of IntMap(X):
//   X for imaps -- dereferenced cells(???)
//   X for var_maps -- clause #s(???)
}

