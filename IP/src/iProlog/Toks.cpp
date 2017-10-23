#include "a.h"

namespace iProlog
{

std::wstring Toks::IF = L"if";
std::wstring Toks::AND = L"and";
std::wstring Toks::DOT = L".";
std::wstring Toks::HOLDS = L"holds";
std::wstring Toks::LISTS = L"lists";
std::wstring Toks::IS = L"is";

	Toks *Toks::makeToks(const std::wstring &s, bool const fromFile)
	{
	  try
	  {
		Reader *R;
		if (fromFile)
		{
		  R = new FileReader(s);
		}
		else
		{
		  R = new StringReader(s);
		}
		Toks * const T = new Toks(R);
		return T;

	  }
//JAVA TO C++ CONVERTER WARNING: 'final' catch parameters are not available in C++:
//ORIGINAL LINE: catch (final IOException e)
	  catch (const IOException &e)
	  {
		e->printStackTrace();
		return nullptr;
	  }
	}

	Toks::Toks(Reader *const reader) : StreamTokenizer(reader)
	{
	  resetSyntax();
	  eolIsSignificant(false);
	  ordinaryChar(L'.');
	  ordinaryChars(L'!', L'/'); // 33-47
	  ordinaryChars(L':', L'@'); // 55-64
	  ordinaryChars(L'[', L'`'); // 91-96
	  ordinaryChars(L'{', L'~'); // 123-126
	  wordChars(L'_', L'_');
	  wordChars(L'a', L'z');
	  wordChars(L'A', L'Z');
	  wordChars(L'0', L'9');
	  slashStarComments(true);
	  slashSlashComments(true);
	  ordinaryChar(L'%');
	}

	std::wstring Toks::getWord()
	{
	  std::wstring t = L"";

	  int c = TT_EOF;
	  try
	  {
		c = nextToken();
		while (std::isspace(c) && c != TT_EOF)
		{
		  c = nextToken();
		}
	  }
//JAVA TO C++ CONVERTER WARNING: 'final' catch parameters are not available in C++:
//ORIGINAL LINE: catch (final IOException e)
	  catch (const IOException &e)
	  {
		return L"*** tokenizer error:" + t;
	  }

	  switch (c)
	  {
		case TT_WORD:
		{
		  constexpr wchar_t first = sval->charAt(0);
		  if (std::isupper(first) || L'_' == first)
		  {
			t = L"v:" + sval;
		  }
		  else
		  {
			try
			{
			  constexpr int n = static_cast<Integer>(sval);
			  if (std::abs(n) < 1 << 28)
			  {
				t = L"n:" + sval;
			  }
			  else
			  {
				t = L"c:" + sval;
			  }
			}
//JAVA TO C++ CONVERTER WARNING: 'final' catch parameters are not available in C++:
//ORIGINAL LINE: catch (final Exception e)
			catch (const std::exception &e)
			{
			  t = L"c:" + sval;
			}
		  }
		}
		break;

		case StreamTokenizer::TT_EOF:
		{
		  t = L"";
		}
		break;

		default:
		{
		  t = L"" + StringHelper::toString(static_cast<wchar_t>(c));
		}

	  }
	  return t;
	}

	std::vector<std::vector<std::vector<std::wstring>>> Toks::toSentences(const std::wstring &s, bool const fromFile)
	{
	  const std::vector<std::vector<std::vector<std::wstring>>> Wsss = std::vector<std::vector<std::vector<std::wstring>>>();
	  std::vector<std::vector<std::wstring>> Wss;
	  std::vector<std::wstring> Ws;
	  Toks * const toks = makeToks(s, fromFile);
	  std::wstring t = L"";
	  while (L"" != (t = toks->getWord()))
	  {

		if (DOT == t)
		{
		  Wss.push_back(Ws);
		  Wsss.push_back(Wss);
		  Wss = std::vector<std::vector<std::wstring>>();
		  Ws = std::vector<std::wstring>();
		}
		else if ((L"c:" + IF)->equals(t))
		{

		  Wss.push_back(Ws);

		  Ws = std::vector<std::wstring>();
		}
		else if ((L"c:" + AND)->equals(t))
		{
		  Wss.push_back(Ws);

		  Ws = std::vector<std::wstring>();
		}
		else if ((L"c:" + HOLDS)->equals(t))
		{
		  const std::wstring w = Ws[0];
		  Ws[0] = L"h:" + w.substr(2);
		}
		else if ((L"c:" + LISTS)->equals(t))
		{
		  const std::wstring w = Ws[0];
		  Ws[0] = L"l:" + w.substr(2);
		}
		else if ((L"c:" + IS)->equals(t))
		{
		  const std::wstring w = Ws[0];
		  Ws[0] = L"f:" + w.substr(2);
		}
		else
		{
		  Ws.push_back(t);
		}
	  }
	  return Wsss;
	}

	std::wstring Toks::toString(std::vector<void*> &Wsss)
	{
	  return Arrays::deepToString(Wsss);
	}

	void Toks::main(std::vector<std::wstring> &args)
	{
	  Main::pp(toSentences(L"prog.nl", true));
	}
}
