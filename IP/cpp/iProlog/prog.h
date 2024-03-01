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
		void ppc(const Spine &S) const;
		string showClause(const Clause& s) const;

		void ppGoals(const shared_ptr<CellList> bs) const;
		void pp(const string s) const;
		void pp(sym_tab &sym) const;
		void ppTrail();
		Prog(	CellStack&		 heap,
				vector<Clause>&  clauses,
				sym_tab&		 sym,
				index *			 Ip)
									: Engine(heap, clauses, sym, Ip) { };

        string showTermCell(cell x) const;
        string showTerm(Object O) const;
        string showIMaps() const;
        Object exportTerm(cell c) const;

	private:
		static string maybeNull(const Object& O);
		static inline bool isListCons(cstr name);
		static inline bool isOp(cstr name);
		static string st0(const vector<Object>& args);
		string showCells(int base, int len) const;
	};
};
