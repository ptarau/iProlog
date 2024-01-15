/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include <iostream>
#include <sstream>
#include <string>
#include "Engine.h"
#include "CellList.h"
#include "prog.h"


using namespace std;

namespace iProlog {
    /**
     * raw display of a term - to be overridden
     */
    /*virtual*/ string Prog::showTermCell(cell x) {
      return showTerm(exportTerm(x));
    }

    /**
     * raw display of an externalized term
     */
    string Prog::showTerm(Object O) {
        if (O.type == Object::e_integer)
            return O.toString();
        if (O.type == Object::e_vector)
            return st0(O.v);
        return O.toString();
    }

Object Prog::exportTerm(cell x) {

    if (x == cell::tag(cell::BAD,0))
	return Object();

    x = deref(x);
    int t = cell::tagOf(x);
    int w = cell::detag(x);

    Object res;
    switch (t) {
        case cell::C_: res = getSym(w);     break;
        case cell::N_: res = Integer(w);            break;
        case cell::V_: res = cstr("V") + w;         break;
            /*case U_:*/ 

        case cell::R_: {
                    cell a = cell_at(w);
                    if (!cell::isArgOffset(a)) {
                        throw logic_error(cstr("*** should be A, found=") + showCell(a));
                    }
                    int n = cell::detag(a);
                    vector<Object> args;
                    int k = w + 1;
                    for (int i = 0; i < n; i++) {
                        int j = k + i;
                        cell c = cell_at(j);
                        Object o = exportTerm(c);
                        args.push_back(o);
                    }
                    res = args;
                }
                break;
        default:
                    throw logic_error(cstr("*BAD TERM*") + showCell(x));
    }
    return res;
}


    string Prog::showCells(int base, int len) {
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

	void Prog::pp(string s) {
	    std::cout << s << endl;
	}

	void Prog::pp(unordered_map<string, Integer*> syms) {
		pp("pp(syms):");
		cout << "syms.size()=" << syms.size() << endl;
		for (auto &kv : syms)
			cout << "   " << kv.first << "," << kv.second->i << endl;
	}

        void Prog::ppGoals(shared_ptr<CellList> bs) {
            for (shared_ptr<CellList> bp = bs; bs != nullptr; bs = CellList::tail(bs)) {
                pp(showTerm(exportTerm(CellList::head(bp))));
            }
        }
        void Prog::ppc(Spine &S) {
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
 * run - execute the logig program. "It also unpacks the actual answer term
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
        pp(cstr("n_matches=") + Engine::Ip->n_matches);
        pp(cstr("n_alloced=") + CellList::alloced());
    }

    void Prog::ppCode() {
        string t;

        for (size_t i = 0; i < slist.size(); i++) {
            if (i > 0) t += ", ";
            t += slist[i] + "=" + i;
        }

        pp("\nSYMS:\n{" + t + "}");

	pp(syms);

        pp("\nCLAUSES:\n");

        for (size_t i = 0; i < clauses.size(); i++) {
            pp(cstr("[") + i + "]:" + showClause(clauses[i]));
        }
        pp("");
    }

    string Prog::showClause(const Clause &s) {
        string buf;

        size_t l = s.goal_refs.size();
        buf += "\n";
        // buf += showTerm(s.goal_refs[0]);
        buf += showCell(s.goal_refs[0]);

        if (l > 1) {
            buf += " :- \n";
            for (int i = 1; i < l; i++) {
                cell e = s.goal_refs[i];
                buf += "   ";
                // buf += showTerm(e);
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
        buf += showCell(s.goal_refs[0]);
        buf += " :- [";
        for (size_t i = 1; i < l; i++) {
            cell e = s.goal_refs[i];
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
