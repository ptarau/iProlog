//====================================================================================================
//The Free Edition of Java to C++ Converter limits conversion output to 100 lines per file.

//To purchase the Premium Edition, visit our website:
//https://www.tangiblesoftwaresolutions.com/order/order-java-to-cplus.html
//====================================================================================================

#include "IntMap.h"

namespace iProlog
{

	IntMap::IntMap() : IntMap(1 << 2)
	{
	}

	IntMap::IntMap(int const size) : IntMap(size, 0.75f)
	{
	}

	IntMap::IntMap(int const size, float const fillFactor)
	{
	  if (fillFactor <= 0 || fillFactor >= 1)
	  {
		throw std::invalid_argument("FillFactor must be in (0, 1)");
	  }
	  if (size <= 0)
	  {
		throw std::invalid_argument("Size must be positive!");
	  }
	  constexpr int capacity = arraySize(size, fillFactor);
	  m_mask = capacity - 1;
	  m_mask2 = capacity * 2 - 1;
	  m_fillFactor = fillFactor;

	  m_data = std::vector<int>(capacity * 2);
	  m_threshold = static_cast<int>(capacity * fillFactor);
	}

	int IntMap::get(int const key)
	{
	  int ptr = (phiMix(key) & m_mask) << 1;

	  if (key == FREE_KEY)
	  {
		return m_hasFreeKey ? m_freeValue : NO_VALUE;
	  }

	  int k = m_data[ptr];

	  if (k == FREE_KEY)
	  {
		return NO_VALUE; //end of chain already
	  }
	  if (k == key) //we check FREE prior to this call
	  {
		return m_data[ptr + 1];
	  }

	  while (true)
	  {
		ptr = ptr + 2 & m_mask2; //that's next index
		k = m_data[ptr];
		if (k == FREE_KEY)
		{
		  return NO_VALUE;
		}
		if (k == key)
		{
		  return m_data[ptr + 1];
		}
	  }
	}

	bool IntMap::contains(int const key)
	{
	  return NO_VALUE != get(key);
	}

	bool IntMap::add(int const key)
	{
	  return NO_VALUE != put(key, 666);
	}

	bool IntMap::delete(int const key)
	{
	  return NO_VALUE != remove(key);
	}

	bool IntMap::isEmpty()
	{
	  return 0 == m_size;
	}

	void IntMap::intersect0(IntMap *const m, std::vector<IntMap*> &maps, std::vector<IntMap*> &vmaps, IntStack *const r)
	{
	  const std::vector<int> data = m->m_data;
	  for (int k = 0; k < data.size(); k += 2)
	  {
		bool found = true;
		constexpr int key = data[k];
		if (FREE_KEY == key)
		{
		  continue;
		}
		for (int i = 1; i < maps.size(); i++)
		{
		  IntMap * const map = maps[i];
		  constexpr int val = map->get(key);

		  if (NO_VALUE == val)
		  {
			IntMap * const vmap = vmaps[i];
			constexpr int vval = vmap->get(key);
			if (NO_VALUE == vval)
			{
			  found = false;
			  break;
			}
		  }
		}
		if (found)
		{
		  r->push(key);
		}
	  }
	}

	IntStack *IntMap::intersect(std::vector<IntMap*> &maps, std::vector<IntMap*> &vmaps)
	{
	  IntStack * const r = new IntStack();

	  intersect0(maps[0], maps, vmaps, r);
	  intersect0(vmaps[0], maps, vmaps, r);
	  return r;
	}

	int IntMap::put(int const key, int const value)
	{
	  if (key == FREE_KEY)
	  {
		constexpr int ret = m_freeValue;
		if (!m_hasFreeKey)
		{
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
		if (m_size >= m_threshold)
		{
		  rehash(m_data.size() * 2); //size is set inside
		}
		else
		{
		  ++m_size;
		}
		return NO_VALUE;
	  }
	  else if (k == key) //we check FREE prior to this call
	  {
		constexpr int ret = m_data[ptr + 1];
		m_data[ptr + 1] = value;
		return ret;
	  }

	  while (true)
	  {
		ptr = ptr + 2 & m_mask2; //that's next index calculation
		k = m_data[ptr];
		if (k == FREE_KEY)
		{
		  m_data[ptr] = key;
		  m_data[ptr + 1] = value;
		  if (m_size >= m_threshold)
		  {
			rehash(m_data.size() * 2); //size is set inside
		  }
		  else
		  {
			++m_size;
		  }
		  return NO_VALUE;
		}
		else if (k == key)
		{
		  constexpr int ret = m_data[ptr + 1];
		  m_data[ptr + 1] = value;
		  return ret;
		}
	  }
	}

	int IntMap::remove(int const key)
	{
	  if (key == FREE_KEY)
	  {

//====================================================================================================
//End of the allowed output for the Free Edition of Java to C++ Converter.

//To purchase the Premium Edition, visit our website:
//https://www.tangiblesoftwaresolutions.com/order/order-java-to-cplus.html
//====================================================================================================
