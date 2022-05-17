#pragma once

#include <string>
#include <vector>
#include <algorithm>

/**
Dynamic Stack for int data.
 */
namespace iProlog
{

	class IntStack
	{

  private:
	  std::vector<int> stack;

	  int top = 0;

  public:
	  static int SIZE; // power of 2

	  static int MINSIZE; // power of 2

	  IntStack();

	  IntStack(int const size);

	  int getTop();

	  int setTop(int const top);

	  void clear();

	  bool isEmpty();

	  /**
	   * Pushes an element - top is incremented first than the
	   * element is assigned. This means top point to the last assigned
	   * element - which can be returned with peek().
	   */
	  void push(int const i);

	  int pop();

	  int get(int const i);

	  void set(int const i, int const val);

	  int size();

	  /**
	   * dynamic array operation: doubles when full
	   */
  private:
	  void expand();

	  /**
	  * dynamic array operation: shrinks to 1/2 if more than than 3/4 empty
	  */
	  void shrink();

  public:
	  virtual std::vector<int> toArray();

	  void reverse();

	  virtual std::wstring toString();

	};

}
