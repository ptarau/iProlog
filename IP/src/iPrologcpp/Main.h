#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <any>

namespace iProlog
{

	class Main
	{

  public:
	  static void println(std::any const o);

	  static void pp(std::any const o);

	  static void run(const std::wstring &fname0);

	  static void srun(const std::wstring &fname0);

	  static void main(std::vector<std::wstring> &args);
	};

}
