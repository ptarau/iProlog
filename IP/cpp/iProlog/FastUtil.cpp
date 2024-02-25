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

#include <stdexcept>
#include "FastUtil.h"

namespace iProlog {

    using namespace std;

    // "taken from FastUtil"
    static const int INT_PHI = 0x9E3779B9;

    int FastUtil::phiMix(int x) {
        int h = x * INT_PHI;
        return h ^ h >> 16;
    }


    /** Taken from FastUtil implementation */

      /** Return the least power of two greater than or equal to the specified value.
       *
       * <p>Note that this function will return 1 when the argument is 0.
       *
       * @param x a long integer smaller than or equal to 2<sup>62</sup>.
       * @return the least power of two greater than or equal to the specified value.
       */


    size_t FastUtil::nextPowerOfTwo(size_t x) {
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
        return x + 1;
    }

    /** Returns the least power of two smaller than or equal to 2<sup>30</sup>
     * and larger than or equal to <code>Math.ceil( expected / f )</code>.
     *
     * @param expected the expected number of elements in a hash table.
     * @param f the load factor.
     * @return the minimum possible size for a backing array.
     * @throws IllegalArgumentException if the necessary size is larger than 2<sup>30</sup>.
     */
    long FastUtil::arraySize(int expected, float f) {
        long s = nextPowerOfTwo((long)((expected / f) + 1));
        if (s < 2) s = 2;

        if (s > 1L << 30) {
            throw invalid_argument("Too large (< + expected + > expected elements with load factor < + f + >)");
        }
        return s;
    }

} // namespace
