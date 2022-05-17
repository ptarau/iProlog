#pragma once

#include "IntStack.h"
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include "stringbuilder.h"

/**
 * derived from code at https://github.com/mikvor/hashmapTest
 */
namespace iProlog
{
	class IntMap // : public java::io::Serializable
	{
  private:
	  static constexpr long long serialVersionUID = 1LL;

	  static constexpr int FREE_KEY = 0;

  public:
	  static constexpr int NO_VALUE = 0;

	  /** Keys and values */
  private:
	  std::vector<int> m_data;

	  /** Do we have 'free' key in the map? */
	  bool m_hasFreeKey = false;
	  /** Value of 'free' key */
	  int m_freeValue = 0;

	  /** Fill factor, must be between (0 and 1) */
	  const float m_fillFactor;
	  /** We will resize a map once it reaches this size */
	  int m_threshold = 0;
	  /** Current map size */
	  int m_size = 0;

	  /** Mask to calculate the original position */
	  int m_mask = 0;
	  int m_mask2 = 0;

  public:
	  IntMap();

	  IntMap(int const size);

	  IntMap(int const size, float const fillFactor);

	  int get(int const key);

	  // for use as IntSet - Paul Tarau

	  bool contains(int const key);

	  bool add(int const key);

	  virtual bool Delete(int const key);

	  bool isEmpty();

	  static void intersect0(IntMap *const m, std::vector<IntMap*> &maps, std::vector<IntMap*> &vmaps, IntStack *const r);

	  static IntStack *intersect(std::vector<IntMap*> &maps, std::vector<IntMap*> &vmaps);

	  // end changes

	  int put(int const key, int const value);

	  int remove(int const key);

  private:
	  int shiftKeys(int pos);

  public:
	  int size();

  private:
	  void rehash(int const newCapacity);

	  /** Taken from FastUtil implementation */

	  /** Return the least power of two greater than or equal to the specified value.
	   *
	   * <p>Note that this function will return 1 when the argument is 0.
	   *
	   * @param x a long integer smaller than or equal to 2<sup>62</sup>.
	   * @return the least power of two greater than or equal to the specified value.
	   */
	  static long long nextPowerOfTwo(long long x);

	  /** Returns the least power of two smaller than or equal to 2<sup>30</sup>
	   * and larger than or equal to <code>Math.ceil( expected / f )</code>.
	   *
	   * @param expected the expected number of elements in a hash table.
	   * @param f the load factor.
	   * @return the minimum possible size for a backing array.
	   * @throws IllegalArgumentException if the necessary size is larger than 2<sup>30</sup>.
	   */
	  static int arraySize(int const expected, float const f);

	  //taken from FastUtil
	  static constexpr int INT_PHI = 0x9E3779B9;

	  static int phiMix(int const x);

  public:
	  virtual std::wstring toString();

	};

}
