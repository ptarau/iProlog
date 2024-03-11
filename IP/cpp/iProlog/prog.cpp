/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include <iostream>
#include <sstream>
#include "Engine.h"
#include "prog.h"

using namespace std;

namespace iProlog {
    /**
     * raw display of a term - to be overridden
     */
    /*virtual*/ string Prog::showTermCell(cell x) const {
      return showTerm(exportTerm(x));
    }

    /**
     * raw display of an externalized term
     */
    string Prog::showTerm(Object O) const {
        if (O.type == Object::e_integer)
            return O.toString();
        if (O.type == Object::e_vector)
            return st0(O.v);
        return O.toString();
    }

    string Prog::showIMaps() const {
       cout << "INDEX" << endl;
       return IMap::show(Ip->imaps);
    }

Object Prog::exportTerm(cell x) const {

    if (x == cell::tag(cell::BAD,0))
	    return Object();

    x = deref(x);
    int w = x.arg();

    switch (x.s_tag()) {
    case cell::C_: return Object(sym.getSym(w));
    case cell::N_: return Object(Integer(w));
    case cell::V_: return Object(cstr("V") + w);
        /*case U_:*/
    case cell::R_: {
                 cell a = cell_at(w);
                 if (!a.is_offset()) throw logic_error(cstr("*** should be A, found=") + showCell(a));
                 int n = a.arg();
                 vector<Object> args;
                 for (int k = w + 1; n-- > 0; k++)
                     args.push_back(exportTerm(cell_at(k)));
                 return Object(args);
                }
        default:
                    throw logic_error(cstr("*BAD TERM*") + showCell(x));
    }
}


    string Prog::showCells(int base, int len) const {
	string buf;
	for (int k = 0; k < len; k++) {
	    cell instr = cell_at(base + k);
	    buf += cstr("[") + (base + k) + "]" + showCell(instr) + " ";
	}
	return buf;
    }

    void Prog::ppTrail() {
        assert(cell::V_ == 0);
        for (int i = 0; i <= trail.getTop(); i++) {
            cell t = trail.get(size_t(i));
            // pp(cstr("trail_[") + i + "]=" + showCell(t) + ":" + showTermCell(t));
            pp(cstr("trail_[") + i + "]=" + showCell(t) + ":"
					  + "*[showTermCell(cell) stub]*");
        }
    }

	void Prog::pp(const string s) const {
	    std::cout << s << endl;
	}
#if 0
	void Prog::pp(unordered_map<string, Integer*> syms) const {
#else
    void Prog::pp(sym_tab &syms) const {
#endif
		pp("pp(syms):");
		cout << "syms.size()=" << syms.syms.size() << endl;
		for (auto &kv : syms.syms)
			cout << "   " << kv.first << "," << kv.second.as_int() << endl;
	}

        void Prog::ppGoals(const shared_ptr<CellList> bs) const {
            for (shared_ptr<CellList> bp = bs; bp != nullptr; bp = CellList::tail(bs)) {
                pp(showTerm(exportTerm(CellList::head(bp))));
            }
        }
        void Prog::ppc(const Spine &S) const {
            shared_ptr<CellList> bs = S.goals;
            pp(cstr("\nppc: t=") + S.trail_top + ",last_clause_tried=" + S.last_clause_tried + "len=" + bs->size());
            ppGoals(bs);
        }

        inline bool Prog::isListCons(cstr name) { return "." == name || "[|]" == name || "list" == name; }
        inline bool Prog::isOp(cstr name) { return "/" == name || "-" == name || "+" == name || "=" == name; }
 
    string Prog::stats() const {
        ostringstream s;
        s // << heap_.capacity() << ' '
            // << spines_top << " of " << spines.capacity() << ' '
            // << trail.capacity() << ' '
            // << unify_stack.capacity()
            ;
        return s.str();
    }

/**
 * run - execute the logic program. "It also unpacks the actual answer term
 * (by calling the method exportTerm) to a tree representation of a term,
 * consisting of recursively embedded arrays hosting as leaves,
 * an external representation of symbols, numbers and variables." [HHG doc]
 */
    void Prog::run(bool print_ans) {
        int ctr = 0;
        for (;; ctr++) {
            auto A = exportTerm(ask());
            if (A.type == Object::e_nullptr)
                break;
            if (print_ans)
                pp(cstr("[") + ctr + "] " + "*** ANSWER=" + showTerm(A));
        }
        pp(cstr("TOTAL ANSWERS=") + ctr);
if(indexing)
        pp(cstr("n_matches=") + Engine::Ip->n_matches);
        pp(cstr("n_alloced=") + CellList::alloced());
    }

    void Prog::ppCode() const {
        string t;

        for (size_t i = 0; i < sym.slist.size(); i++) {
            if (i > 0) t += ", ";
            t += sym.slist[i] + "=" + i;
        }

        pp("\nSYMS:\n{" + t + "}");

	    pp((sym_tab&) sym);  // compiler was erroring out on trying to convert to string??

        pp("\nCLAUSES:\n");

        for (size_t i = 0; i < clauses.size(); i++) {
            pp(cstr("[") + i + "]:" + showClause(clauses[i]));
        }
        pp("");
    }

    string Prog::showClause(const Clause &s) const {
        string buf;

        size_t l = s.skeleton.size();
        buf += "\n";
        buf += showCell(s.skeleton[0]);

        if (l > 1) {
            buf += " :- \n";
            for (int i = 1; i < l; i++) {
                cell e = s.skeleton[i];
                buf += "   ";
                buf += showCell(e);
                buf += "\n";
            }
        }
        else {
            buf += "\n";
        }

        buf += cstr("---base:[") + s.base + "] neck: " + s.neck + "-----\n";
        buf += showCells(s.base, s.len); // TODO
        buf += "\n";
        buf += showCell(s.skeleton[0]);
        buf += " :- [";
        for (size_t i = 1; i < l; i++) {
            cell e = s.skeleton[i];
            buf += showCell(e);
            if (i < l - 1)
                buf += ", ";
        }
        buf += "]\n";

        return buf;
    }

    string Prog::st0(const vector<Object> &args) {
        string r;
        if (!args.empty()) {
            string name = args[0].toString();
            if (args.size() == 3 && isOp(name)) {
                r += "(";
                r += maybeNull(args[0]);
                r += " " + name + " ";
                r += maybeNull(args[1]);
                r += ")";
            } else if (args.size() == 3 && isListCons(name)) {
                r += '[';
                r += maybeNull(args[1]);
                Object tail = args[2];
                for (;;) {
                    if ("[]" == tail.toString() || "nil" == tail.toString())
                        break;
                    if (tail.type != Object::e_vector) {
                        r += '|';
                        r += maybeNull(tail);
                        break;
                    }
                    const vector<Object>& list = tail.v;
                    if (!(list.size() == 3 && isListCons(list[0].toString()))) {
                        r += '|';
                        r += maybeNull(tail);
                        break;
                    } else {
                        r += ',';
                        r += maybeNull(list[1]);
                        tail = list[2];
                    }
                }
                r += ']';
            } else if (args.size() == 2 && "$VAR" == name) {
                r += "_" + args[1].toString();
            } else {
                string qname = maybeNull(args[0]);
                r += qname;
                r += "(";
                for (size_t i = 1; i < args.size(); i++) {
                    r += maybeNull(args[i]);
                    if (i < args.size() - 1)
                        r += ",";
                }
                r += ")";
            }
        }
        return r;
    }

    string Prog::maybeNull(const Object &O) {
        if (O.type == Object::e_nullptr)
            return "$null";
        if (O.type == Object::e_vector)
            return st0(O.v);
        return O.toString();
    }
}
