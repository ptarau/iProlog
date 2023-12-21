/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */
#pragma once

#include "Engine.h"

using namespace std;

namespace iProlog {
	class Prog : Engine {
	public:
		void run(bool print_ans);
		string stats() const;
		Prog(string s) : Engine(s) {};
		void ppCode();
		void ppc(Spine &S);
		string showClause(const Clause& s);
		string showTerm(Object O);

		void ppGoals(CellList* bs);

	private:
		static string maybeNull(const Object& O);
		static inline bool isListCons(cstr name);
		static inline bool isOp(cstr name);
		static string st0(const vector<Object>& args);
	};
};