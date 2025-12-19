#pragma once
/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "Engine.h"
#include "Object.h"

using namespace std;

namespace iProlog {
	class Prog : Engine {
	public:
		void run(bool print_ans);
		string stats() const;
		void ppCode() const;
		void ppc(const Spine &S);
		string showClause(const Clause& s) const;

		void ppGoals(CL_p bs);
		void pp(const string s) const;
		void pp(sym_tab &sym) const;
		void ppTrail();
		Prog(CellStack& heap_0,
			vector<Clause>& clauses_0,
			sym_tab& sym_0,
			index *Ip_0)
			: Engine(heap_0, clauses_0, sym_0, Ip_0) {

			if (indexing)
				Ip = new index(heap_0, clauses_0);
		}

        string showTermCell(cell x) const;
        string showTerm(Object O) const;
		string show_index() const;

	private:
		static string maybeNull(const Object& O);
		static inline bool isListCons(cstr name);
		static inline bool isOp(cstr name);
		static string st0(const vector<Object>& args);
		string showCells(int base, int len) const;
	};
};
