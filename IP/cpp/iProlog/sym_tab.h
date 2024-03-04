#pragma once

/* Inty -- for stronger typing in an "ints all the way down" datastructure
 *
 * It may make sense to make it a template class, so that there can
 * be int16, int32, int64 in places where it matters for speed/space,
 * even varying according to use in data structures.
 */
#include "defs.h"
#include "Integer.h"
#include <unordered_map>
#include "Inty.h"

namespace iProlog {

	typedef Inty<sym_idx_int> sym_idx;

	class sym_tab {
	public:
		unordered_map<string, sym_idx> syms;
		vector<string> slist;

		/**
         * "Places an identifier in the symbol table."
         */
		sym_idx addSym(const string sym) {
			try { return syms.at(sym); }
			catch (const std::exception& e) {
				sym_idx I;
				I.set((int)syms.size());
				syms.insert(pair<string, sym_idx>(sym, I));
				slist.push_back(sym);
				return I;
			}
		}

		/**
		 * "Returns the symbol associated to an [Inty] index
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