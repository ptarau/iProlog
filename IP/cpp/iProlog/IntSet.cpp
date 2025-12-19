

// Was IntMap.java; Tarau uses it as an intset, hence the "666"
// value in the key-value pairs.

#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "cell.h"
#include "index.h"
#include "IMap.h"

namespace iProlog {

    // https://stackoverflow.com/questions/108318/how-can-i-test-whether-a-number-is-a-power-of-2)

    bool is_a_power_of_2(int size) {
        return size > 0 && (size & (size - 1)) == 0;
    }

    IntSet *IntSet::init(IntSet* isp) {
        int capacity = arraySize(init_cap, m_fillFactor_pc);

        isp->set_mask(capacity);
        isp->m_data = isp->alloc(capacity);
        isp->m_threshold = (capacity * m_fillFactor_pc) / 100;
        isp->m_size = 0;

        return isp;
    }

    void IntSet::dealloc(IntSet* isp) {
        if (isp == nullptr) abort();
        isp->md_free(isp->m_data);
        free((void*)isp);
    }

    IntSet::IntSet() {

        assert(is_a_power_of_2(init_cap));

        init(this);
    }

    void IntSet::rehash(size_t newCapacity) {
#define TR if(0)
        TR cout << "           !!!!!!!!!!!!! calling rehash, newCapacity="
             << to_string(newCapacity) << " !!!!!!!!!!!!!!!!!!" << endl;
        m_threshold = (int)(newCapacity * ((float)m_fillFactor_pc/100));
        set_mask(newCapacity);

        TR cout << "             m_mask is now " << m_mask << endl;

        size_t oldCapacity = length();
        Vec oldData = m_data;

        TR for (int i = 0; i < oldCapacity; i++)
            cout << "             oldData[" << i << "].key=" << oldData[i].key
                 << " oldData[i].val=" << oldData[i].val << endl;

        m_data = alloc(newCapacity);  // sets md_capacity = newCapacity
        m_size = m_hasFreeKey ? 1 : 0;

        for (int i = 0; i < oldCapacity; i++) {
            int oldKey = oldData[i].key;
            if (oldKey != FREE_KEY) {
                put(oldKey, oldData[i].val);
                assert(contains(oldKey));
            }
        }
        md_free(oldData);
#undef TR
    }

    int IntSet::get(int key) const {
#define TR if(0)
        int ptr = mk_ptr(key);

        TR cout << "      IntSet::get(" << key << "), ptr="
            << ptr <<  endl;

        if (key == FREE_KEY) {
            int r = m_hasFreeKey ? m_freeValue : NO_VALUE;
            TR cout << "       key==FREE_KEY m_hasFreeKey=" << m_hasFreeKey << endl;
            return r;
        }
        int k = get_key_at(ptr);

        if (k == FREE_KEY) {
            // TR cout << "       k==m_data[" << ptr << "]==" << k << " == FREE_KEY" << ", m_data[ptr + 1] = " << m_data[ptr + 1] << endl;
            return NO_VALUE; //end of chain already
        }

        if (k == key) { //we check FREE prior to this call
            // TR cout << "       k==key, returning m_data[" << ptr + 1 << "]=" << m_data[ptr + 1] << endl;
            return get_val_at(ptr);
        }

        while (true) {
            // cout << "get loop" << endl;
            ptr = next(ptr);
            k = get_key_at(ptr);
            if (k == FREE_KEY) {
                // TR cout << "       k=get_key_at(" << ptr << ")==FREE_KEY " << FREE_KEY << " m_data[ptr+1]="<< m_data[ptr+1] << endl;
                return NO_VALUE;
            }
            if (k == key) {
                TR cout << "       k==key at end of chain==" << k << endl;
                return get_val_at(ptr);
            }
        }
#undef TR
    }

    int IntSet::put(int key, int value) {
#define TR if(0)
        TR cout << endl
                << "       IntSet::put(key=" << to_string(key)
                << ", value=" << to_string(value) << ")"
                << " md_capacity=" << to_string(md_capacity)
                << " m_size()=" << to_string(m_size)
                << " m_threshold=" << to_string(m_threshold)
                << endl;

        if (key == FREE_KEY) {
            int ret = m_freeValue;
            if (!m_hasFreeKey) {
                ++m_size;
            }
            m_hasFreeKey = true;
            m_freeValue = value;
            TR cout << "         (1) key == FREE_KEY, returning " << ret << endl;
            return ret;
        }

        int ptr = mk_ptr(key);
        int k = get_key_at(ptr); // m_data[ptr];
        if (k == FREE_KEY) //end of chain already
        {
            set_key_at(ptr,key);
            set_val_at(ptr,value);
            if (m_size >= m_threshold) {
                TR cout << "**** about to call rehash, m_size=" << m_size << " m_threshold=" << m_threshold << endl;
                // TR cout << "****   m_data.capacity()=" << m_data.capacity() << endl;
                rehash(length() * 2); //size is set inside
            }
            else {
                ++m_size;
            }
            TR cout << "         (2) k == FREE_KEY, returning NO_VALUE" << endl;
            return NO_VALUE;
        }
        else if (k == key) //we check FREE prior to this call
        {
            int ret = get_val_at(ptr);
            set_val_at(ptr, value);
            TR cout << "         (3) k == key, get_val_at(ptr)<-value==" << get_val_at(ptr) << " returning past value " << ret << endl;
            return ret;
        }

        int count = 0;
        while (true) {
            TR cout << "ptr=" << to_string(ptr) << endl;
            ptr = next(ptr);
            k = get_key_at(ptr);
            TR cout << "k=" << k << endl;
            if (k == FREE_KEY) {
                set_key_at(ptr, key);
                set_val_at(ptr, value);
                if (m_size >= m_threshold) {
                    TR cout << "     needing to rehash" << endl;
                    rehash(length() * 2); //size is set inside
                }
                else {
                    ++m_size;
                }
                TR cout << "         (4) at end of chain, returning NO_VALUE" << endl;
                return NO_VALUE;
            }
            else if (k == key) {
                int ret = get_val_at(ptr);
                set_val_at(ptr, value);
                TR cout << "         (5) found something, returning " << ret << endl;
                return ret;
            }
        }
    }

#if 0 // not called
    int IntSet::remove(int key) {
        if (key == FREE_KEY) {
            if (!m_hasFreeKey)
                return NO_VALUE;
            m_hasFreeKey = false;
            --m_size;
            return m_freeValue; //value is not cleaned
        }

        int ptr = mk_ptr(key);
        int k = get_key_at(ptr);
        if (k == key) //we check FREE prior to this call
        {
            int res = get_val_at(ptr);
            shiftKeys(ptr);
            --m_size;
            return res;
        }
        else if (k == FREE_KEY)
            return NO_VALUE; //end of chain already
        while (true) {
            ptr = next(ptr); 
            k = get_key_at(ptr);
            if (k == key) {
                int res = get_val_at(ptr);
                shiftKeys(ptr);
                --m_size;
                return res;
            }
            else if (k == FREE_KEY)
                return NO_VALUE;
        }
    }

    int IntSet::shiftKeys(int pos) {
        // Shift entries with the same hash.
        int last, slot;
        int k;

        while (true) {
            cout << "shiftkeys loop" << endl;
            last = pos;
            pos = next(last);
            while (true) {
                cout << "shiftKeys subloop";
                k = get_key_at(pos);
                if (k == FREE_KEY) {
                    set_key_at(last,FREE_KEY);
                    return last;
                }
                slot = mk_ptr(k); //calculate the starting slot for the current key
                if (last <= pos ? last >= slot || slot > pos : last >= slot && slot > pos) {
                    break;
                }
                pos = next(pos);
            }
            set_key_at(last) = k;
            set_val_at(last, get_val_at(pos));
        }
    }
#endif

    /** Taken from FastUtil implementation */

/** Return the least power of two greater than or equal to the specified value.
 *
 * <p>Note that this function will return 1 when the argument is 0.
 *
 * @param x a long integer smaller than or equal to 2<sup>62</sup>.
 * @return the least power of two greater than or equal to the specified value.
 */
    long IntSet::nextPowerOfTwo(long x) {
        if (x == 0)
            return 1;
        x--;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return (x | x >> 32) + 1;
    }

    // always init with size = 2^n for n >= 0

    /** Returns the least power of two smaller than or equal to 2<sup>30</sup>
     * and larger than or equal to <code>Math.ceil( expected / f )</code>.
     *
     * @param expected the expected number of elements in a hash table.
     * @param f the load factor.
     * @return the minimum possible size for a backing array.
     * @throws IllegalArgumentException if the necessary size is larger than 2<sup>30</sup>.
     */
    int IntSet::arraySize(int expected, int f_pc) {
        long s = std::max(2L, IntSet::nextPowerOfTwo((long)ceil(expected / ((float) f_pc/100))));
        if (s > 1 << 30) {
            string emsg = "Too large " + expected;
            // emsg += " expected elements with load factor " + f;
            throw new logic_error(emsg);
        }
        return (int)s;
    }

    // @Override
string IntSet::show() const {
    //return java.util.Arrays.toString(m_data);
    string b = "{";
    size_t l = md_capacity;
    // cout << "IntSet::show() md_capacity=" << to_string(md_capacity) << endl;
    bool first = true;
    for (int i = 0; i < l; i++) {
        int v = get_key_at(i);
        if (v != FREE_KEY) {
            if (!first) {
                b += ", ";
            }
            first = false;
#if 0
            b += "[" + to_string(i);
            b += "]:#";
#endif
            b += to_string(v);
#if 0
            b += "->" + to_string(get_val_at(i));
#endif
        }
    }
    b += "}";
    return b;
}
}