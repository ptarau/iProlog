#pragma once

#include "defs.h"
#include <unordered_map>

namespace iProlog {

	class sym_tab {
	public:
		unordered_map<string, int> syms;
		vector<string> slist;

		/**
         * "Places an identifier in the symbol table."
         */
		int addSym(const string sym) {
			try { return syms.at(sym); }
			catch (const std::exception& e) {
				std::ignore = e;
				int I= (int) syms.size();
				syms.insert(pair<string, int>(sym, I));
				slist.push_back(sym);
				return I;
			}
		}

		/**
		 * "Returns the symbol associated to an index
         * in the symbol table."
         */
		string getSym(int w) const {
			if (w < 0 || w >= slist.size()) {
				cout << (cstr("BADSYMREF=") + w) << endl;
				abort();
			}
			return slist[w];
		}
	};

} // namespace