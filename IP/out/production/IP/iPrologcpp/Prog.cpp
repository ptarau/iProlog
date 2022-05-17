//====================================================================================================
//The Free Edition of Java to C++ Converter limits conversion output to 100 lines per file.

//To purchase the Premium Edition, visit our website:
//https://www.tangiblesoftwaresolutions.com/order/order-java-to-cplus.html
//====================================================================================================

#include "Prog.h"
#include "Main.h"

namespace iProlog
{
	using Stream = java::util::stream::Stream;
	using Spliterator = java::util::Spliterator;
	using Consumer = java::util::function::Consumer;
	using StreamSupport = java::util::stream::StreamSupport;

	Prog::Prog(const std::wstring &fname) : Engine(fname)
	{
	}

	void Prog::pp(std::any const o)
	{
	  Main::pp(o);
	}

	void Prog::println(std::any const o)
	{
	  Main::println(o);
	}

	std::wstring Prog::showTerm(std::any const O)
	{
	  if (O.type() == typeid(std::any[]))
	  {
		return st0(std::any_cast<std::vector<std::any>>(O));
	  }
//JAVA TO C++ CONVERTER TODO TASK: There is no C++ equivalent to 'toString':
	  return O.toString();
	}

	std::wstring Prog::maybeNull(std::any const O)
	{
	  if (nullptr == O)
	  {
		return L"$null";
	  }
	  if (O.type() == typeid(std::any[]))
	  {
		return st0(std::any_cast<std::vector<std::any>>(O));
	  }
//JAVA TO C++ CONVERTER TODO TASK: There is no C++ equivalent to 'toString':
	  return O.toString();
	}

	bool Prog::isListCons(std::any const name)
	{
	  return L"." == name || L"[|]" == name || L"list" == name;
	}

	bool Prog::isOp(std::any const name)
	{
	  return L"/" == name || L"-" == name || L"+" == name || L"=" == name;
	}

	std::wstring Prog::st0(std::vector<std::any> &args)
	{
	  StringBuilder * const buf = new StringBuilder();
//JAVA TO C++ CONVERTER TODO TASK: There is no C++ equivalent to 'toString':
	  const std::wstring name = args[0].toString();
	  if (args.size() == 3 && isOp(name))
	  {
		buf->append(L"(");
		buf->append(maybeNull(args[0]));
		buf->append(L" " + name + L" ");
		buf->append(maybeNull(args[1]));
		buf->append(L")");
	  }
	  else if (args.size() == 3 && isListCons(name))
	  {
		buf->append(L'[');
		{
		  buf->append(maybeNull(args[1]));
		  std::any tail = args[2];
		  for (;;)
		  {

			if (L"[]" == tail || L"nil" == tail)
			{
			  break;
			}
			if (!(tail.type() == typeid(std::any[])))
			{
			  buf->append(L'|');
			  buf->append(maybeNull(tail));
			  break;
			}
			const std::vector<std::any> list = std::any_cast<std::vector<std::any>>(tail);
			if (!(list.size() == 3 && isListCons(list[0])))
			{
			  buf->append(L'|');
			  buf->append(maybeNull(tail));
			  break;
			}
			else
			{
			  //if (i > 1)
			  buf->append(L',');
			  buf->append(maybeNull(list[1]));
			  tail = list[2];
			}
		  }
		}
		buf->append(L']');
	  }
	  else if (args.size() == 2 && L"$VAR" == name)
	  {
		buf->append(L"_" + args[1]);
	  }
	  else
	  {
		const std::wstring qname = maybeNull(args[0]);
		buf->append(qname);
		buf->append(L"(");
		for (int i = 1; i < args.size(); i++)
		{
		  constexpr std::any O = args[i];
		  buf->append(maybeNull(O));
		  if (i < args.size() - 1)
		  {
			buf->append(L",");
		  }
		}
		buf->append(L")");
	  }
	  return buf->toString();
	}

	void Prog::ppCode()
	{
	  pp(L"\nSYMS:");
	  pp(syms);
	  pp(L"\nCLAUSES:\n");

	  for (int i = 0; i < clauses.size(); i++)
	  {

		Clause * const C = clauses[i];
		pp(L"[" + std::to_wstring(i) + L"]:" + showClause(C));
	  }
	  pp(L"");

	}

	std::wstring Prog::showClause(Clause *const s)
	{
	  StringBuilder * const buf = new StringBuilder();
	  constexpr int l = s->hgs.size();
	  buf->append(L"---base:[" + std::to_wstring(s->base) + L"] neck: " + std::to_wstring(s->neck) + L"-----\n");
	  buf->append(showCells(s->base, s->len)); // TODO
	  buf->append(L"\n");
	  buf->append(showCell(s->hgs[0]));

	  buf->append(L" :- [");
	  for (int i = 1; i < l; i++)
	  {

		constexpr int e = s->hgs[i];
		buf->append(showCell(e));
		if (i < l - 1)
		{
		  buf->append(L", ");
		}
	  }

	  buf->append(L"]\n");

	  buf->append(showTerm(s->hgs[0]));
	  if (l > 1)
	  {
		buf->append(L" :- \n");
		for (int i = 1; i < l; i++)
		{
		  constexpr int e = s->hgs[i];
		  buf->append(L"  ");
		  buf->append(showTerm(e));
		  buf->append(L"\n");
		}
	  }
	  else
	  {
		buf->append(L"\n");
	  }
	  return buf->toString();
	}


//====================================================================================================
//End of the allowed output for the Free Edition of Java to C++ Converter.

//To purchase the Premium Edition, visit our website:
//https://www.tangiblesoftwaresolutions.com/order/order-java-to-cplus.html
//====================================================================================================
