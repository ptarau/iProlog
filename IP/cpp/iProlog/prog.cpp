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

        void Prog::ppGoals(shared_ptr<CellList> bs) {
#if 0
            cout << "ppGoals(IntList) <stub>" << endl;
#endif
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

    void Prog::run(bool print_ans) {
        int ctr = 0;
        for (;; ctr++) {
            // cout << "About to call ask..." << endl;
            auto A = ask();
            if (A.type == Object::e_nullptr)
                break;
            if (print_ans)
                pp(cstr("[") + ctr + "] " + "*** ANSWER=" + showTerm(A));
        }
        pp(cstr("TOTAL ANSWERS=") + ctr);
        pp(cstr("n_matches=") + Engine::n_matches);
        pp(cstr("n_alloced=") + CellList::alloced());
    }

    void Prog::ppCode() {
        string t;

        for (size_t i = 0; i < slist.size(); i++) {
            if (i > 0) t += ", ";
            t += slist[i] + "=" + i;
        }

        pp("\nSYMS:\n{" + t + "}");

        pp("\nCLAUSES:\n");

        for (size_t i = 0; i < clauses.size(); i++) {
#if 0
            cout << "     CLAUSES show loop, i = " << i << endl;
            cout << "clauses[0].base = " << clauses[0].base << endl;
#endif
            pp(cstr("[") + i + "]:" + showClause(clauses[i]));
        }
        pp("");
    }

    string Prog::showTerm(Object O) {
        if (O.type == Object::e_integer)
            return Engine::showTerm(O);
        if (O.type == Object::e_vector)
            return st0(O.v);
        return O.toString();
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
        buf += Engine::showCells2(s.base, s.len); // TODO
        buf += "\n";
        buf += showCell(s.goal_refs[0]);
#if 0
        cout << "showClause: buf now = " << buf << endl;
#endif
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