//====================================================================================================
//The Free Edition of Java to C++ Converter limits conversion output to 100 lines per file.

//To purchase the Premium Edition, visit our website:
//https://www.tangiblesoftwaresolutions.com/order/order-java-to-cplus.html
//====================================================================================================

#include "Engine.h"
#include "Toks.h"
#include "Main.h"
#include "Prog.h"

namespace iProlog {


	Engine::Engine(const std::wstring &fname) : clauses(dload(fname)), cls(toNums(clauses)), syms(new unordered_map<std::wstring, int>()), slist(std::vector<std::wstring>()), trail(new IntStack()), ustack(new IntStack()), imaps(index(clauses, vmaps)), vmaps(vcreate(MAXIND))
	{

	  makeHeap();




	  query = init();

	}

int Engine::MINSIZE = 1 << 15;

	int Engine::tag(int const t, int const w)
	{
	  return -((w << 3) + t);
	}

	int Engine::detag(int const w)
	{
	  return -w >> 3;
	}

	int Engine::tagOf(int const w)
	{
	  return -w & 7;
	}

	int Engine::addSym(const std::wstring &sym)
	{
	  std::optional<int> I = syms->get(sym);
	  if (nullptr == I)
	  {
		constexpr int i = syms->size();
		I = std::optional<int>(i);
		syms->put(sym, I);
		slist.push_back(sym);
	  }
	  return I.value();
	}

	std::wstring Engine::getSym(int const w)
	{
	  if (w < 0 || w >= slist.size())
	  {
		return L"BADSYMREF=" + std::to_wstring(w);
	  }
	  return slist[w];
	}

	void Engine::makeHeap()
	{
	  makeHeap(MINSIZE);
	}

	void Engine::makeHeap(int const size)
	{
	  heap = std::vector<int>(size);
	  clear();
	}

	int Engine::getTop()
	{
	  return top;
	}

	int Engine::setTop(int const top)
	{
	  return this->top = top;
	}

	void Engine::clear()
	{
	  //for (int i = 0; i <= top; i++)
	  //heap[i] = 0;
	  top = -1;
	}

	void Engine::push(int const i)
	{
	  heap[++top] = i;
	}

	int Engine::size()
	{
	  return top + 1;
	}

	void Engine::expand()
	{
	  constexpr int l = heap.size();
	  const std::vector<int> newstack = std::vector<int>(l << 1);

	  std::copy_n(heap.begin(), l, newstack.begin());
	  heap = newstack;
	}

	void Engine::ensureSize(int const more)
	{
	  if (1 + top + more >= heap.size())
	  {
		expand();
	  }
	}

	std::vector<std::vector<std::wstring>> Engine::maybeExpand(std::vector<std::wstring> &Ws)
	{
	  const std::wstring W = Ws[0];
	  if (W.length() < 2 || L"l:" != W.substr(0, 2))
	  {
		return std::vector<std::vector<std::wstring>>();
	  }

	  constexpr int l = Ws.size();
//JAVA TO C++ CONVERTER WARNING: C++ has no equivalent to a 'final' collection which allows modification of internal state:
//ORIGINAL LINE: final ArrayList<String[]> Rss = new ArrayList<String[]>();
	  std::vector<std::vector<std::wstring>> Rss;
	  const std::wstring V = W.substr(2);
	  for (int i = 1; i < l; i++)
	  {
		const std::vector<std::wstring> Rs = std::vector<std::wstring>(4);
		const std::wstring Vi = 1 == i ? V : V + L"__" + std::to_wstring(i - 1);
		const std::wstring Vii = V + L"__" + std::to_wstring(i);
		Rs[0] = L"h:" + Vi;
		Rs[1] = L"c:list";
		Rs[2] = Ws[i];
		Rs[3] = i == l - 1 ? L"c:nil" : L"v:" + Vii;
		Rss.push_back(Rs);
	  }
	  return Rss;

	}

	std::vector<std::vector<std::wstring>> Engine::mapExpand(std::vector<std::vector<std::wstring>> &Wss)
	{
//JAVA TO C++ CONVERTER WARNING: C++ has no equivalent to a 'final' collection which allows modification of internal state:
//ORIGINAL LINE: final ArrayList<String[]> Rss = new ArrayList<String[]>();
	  std::vector<std::vector<std::wstring>> Rss;
	  for (auto Ws : Wss)
	  {

//JAVA TO C++ CONVERTER WARNING: C++ has no equivalent to a 'final' collection which allows modification of internal state:
//ORIGINAL LINE: final ArrayList<String[]> Hss = maybeExpand(Ws);
		std::vector<std::vector<std::wstring>> Hss = maybeExpand(Ws);

		if (nullptr == Hss)
		{
		  const std::vector<std::wstring> ws = std::vector<std::wstring>(Ws.size());
		  for (int i = 0; i < ws.size(); i++)
		  {
			ws[i] = Ws[i];
		  }
		  Rss.push_back(ws);
		}
		else
		{
		  for (auto X : Hss)
		  {
			Rss.push_back(X);
		  }
		}
	  }
	  return Rss;
	}

	std::vector<Clause*> Engine::dload(const std::wstring &s)
	{
	  constexpr bool fromFile = true;
//JAVA TO C++ CONVERTER WARNING: C++ has no equivalent to a 'final' collection which allows modification of internal state:
//ORIGINAL LINE: final ArrayList<ArrayList<ArrayList<String>>> Wsss = Toks.toSentences(s, fromFile);
	  std::vector<std::vector<std::vector<std::wstring>>> Wsss = Toks::toSentences(s, fromFile);

//JAVA TO C++ CONVERTER WARNING: C++ has no equivalent to a 'final' collection which allows modification of internal state:
//ORIGINAL LINE: final ArrayList<Clause> Cs = new ArrayList<Clause>();
	  std::vector<Clause*> Cs;

	  for (auto Wss : Wsss)
	  {
		// clause starts here

		unordered_map<std::wstring, IntStack*> * const refs = new LinkedHashMap<std::wstring, IntStack*>();
		IntStack * const cs = new IntStack();
		IntStack * const gs = new IntStack();

//JAVA TO C++ CONVERTER WARNING: C++ has no equivalent to a 'final' collection which allows modification of internal state:
//ORIGINAL LINE: final ArrayList<String[]> Rss = mapExpand(Wss);
		std::vector<std::vector<std::wstring>> Rss = mapExpand(Wss);
		int k = 0;
		for (auto ws : Rss)
		{

		  // head or body element starts here

		  constexpr int l = ws.size();
		  gs->push(tag(R, k++));
		  cs->push(tag(A, l));

		  for (auto w : ws)
		  {

			// head or body subterm starts here

			if (1 == w.length())
			{
			  w = L"c:" + w;
			}

			const std::wstring L = w.substr(2);

			switch (w.charAt(0))
			{
			  case L'c':
				cs->push(encode(C, L));
				k++;
			  break;
			  case L'n':

//====================================================================================================
//End of the allowed output for the Free Edition of Java to C++ Converter.

//To purchase the Premium Edition, visit our website:
//https://www.tangiblesoftwaresolutions.com/order/order-java-to-cplus.html
//====================================================================================================
