#pragma once
/*
* iProlog / C++  [derived from Java version]
* License: Apache 2.0
* Copyright(c) 2017 Paul Tarau
*/

#include "defs.h"
#include <assert.h>

#include "cell.h"

namespace iProlog {

    using namespace std;

    class CellList {

    private:
        cell head_;
        CellList* tail_;

        // Singleton list
    public:
        CellList() { head_ = 0; tail_ = nullptr; }
  
        CellList(cell h) : head_(h), tail_(nullptr) { }

        inline static bool isEmpty(CellList* Xs) { return nullptr == Xs; }

        static cell head(CellList* Xs) {
            assert(Xs != nullptr);
            return Xs->head_;
        }
#if 0  // strange error message from this:
        static const CellList* empty = nullptr;
#endif
        static CellList* tail(CellList *Xs) {
            return Xs->tail_;
        }

        static CellList *cons(cell X, CellList *Xs) {
            CellList *cl = new CellList(X);
            cl->tail_ = Xs;
            return cl;
        }

        // O(n)
        size_t size() const {
            if (this == nullptr) return 0;
            size_t sum = 1;
            for (CellList* p = tail_; p != nullptr; p = p->tail_)
                ++sum;
            return sum;
        }

        // append CellList Ys to CellList made from int array xs, return result
        static CellList *concat(vector<cell> xs, CellList*Ys) {
            CellList *Zs = Ys;
            if (xs.size() < 1) abort();
            for (int i = int(xs.size()) - 1; i >= 0; i--) {
#if 0
                cout << "concat: i=" << i << endl;
#endif
                cell c = xs[size_t(i)];
                Zs = cons(c, Zs);
            }
#if 0
            cout << "    *** concat: returning with Zs->size()=" << Zs->size() << endl;
#endif
            return Zs;
        }

        // push Zs CellList onto new stack, return stack (tos = last)
        static vector<cell> toCells(CellList *Xs) {
            vector<cell> is = vector<cell>();
            while (!isEmpty(Xs)) {
                cell c = head(Xs);
                is.push_back(c);
                Xs = tail(Xs);
            }
            return is;
        }

        // CellList len (note O(n) running time)
        static size_t len(CellList *Xs) {
            return toCells(Xs).size();
        }

        string toString() {
            string s = "[";
            string sep = "";
            vector<cell> elts = toCells(this);
            for (cell x : elts) {
                s += sep;
                sep = ",";
                s += "<toString cell dummy>"; // showCell(x);
            }
            return s;
        }
    };

} // end namespace