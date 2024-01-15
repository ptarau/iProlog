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
		void ppCode();
		void ppc(Spine &S);
		string showClause(const Clause& s);

		void ppGoals(shared_ptr<CellList> bs);
		void pp(string s);
	        void pp(unordered_map<string, Integer*> syms);
		void ppTrail();
		Prog(CellStack &heap,
		     vector<Clause> &clauses,
		     unordered_map<string, Integer*> &syms,
		     vector<string> &slist,
		     index *Ip)
		         : Engine(heap,clauses,syms,slist,Ip) { };

        /*virtual*/ string showTermCell(cell x);
        /*virtual*/ string showTerm(Object O);
        string showIMaps();
        Object exportTerm(cell c);

	private:
		static string maybeNull(const Object& O);
		static inline bool isListCons(cstr name);
		static inline bool isOp(cstr name);
		static string st0(const vector<Object>& args);
		string showCells(int base, int len);
	};
};
