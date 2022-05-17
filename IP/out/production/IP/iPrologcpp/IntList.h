#pragma once

#include "IntStack.h"
#include <string>
#include <vector>

namespace iProlog
{
	class IntList
	{

  private:
//JAVA TO C++ CONVERTER NOTE: Fields cannot have the same name as methods of the current type:
	  const int the_head;
//JAVA TO C++ CONVERTER NOTE: Fields cannot have the same name as methods of the current type:
	  IntList *const the_tail;

  public:
	  virtual ~IntList()
	  {
		  delete the_tail;
	  }

  private:
	  IntList(int const head);

	  IntList(int const X, IntList *const Xs);

  public:
	  static bool isEmpty(IntList *const Xs);

	  static int head(IntList *const Xs);

	  static constexpr IntList *empty = nullptr;

	  static IntList *tail(IntList *const Xs);

	  static IntList *cons(int const X, IntList *const Xs);

	  static IntList *app(std::vector<int> &xs, IntList *const Ys);

	  static IntStack *toInts(IntList *Xs);

	  static int len(IntList *const Xs);

	  virtual std::wstring toString();
	};

}
