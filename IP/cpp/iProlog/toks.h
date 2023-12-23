#pragma once
/*
 * hhprolog: Hitchhiker Prolog
 *
 * Version: 1.0.0
 * License: MIT
 *
 * Copyright (c) 2018,2019 Carlo Capelli
 */
#include <vector>
#include <string>

namespace iProlog {

	using namespace std;

	typedef vector<string> Ts;
	typedef vector<Ts> Tss;
	typedef vector<Tss> Tsss;

	class Toks {
	private:     const string t, s;
				 int n;
	public:
		Toks(string t, string s, int n); // t(t) : s(s) : n(n) {}
		static Tsss toSentences(string s);
		static Tss  maybeExpand(Ts Ws);
		static Tss  mapExpand(Tss Wss);
		static vector<Toks> makeToks(string s);
		static Tss vcreate(size_t l);
	};


}