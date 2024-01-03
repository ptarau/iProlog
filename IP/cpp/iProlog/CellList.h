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
        shared_ptr<CellList> tail_;
       
    public:
        static int n_alloced;

        // Singleton list
        CellList() { head_ = 0; tail_ = nullptr; ++n_alloced; }
  
        CellList(cell h) : head_(h), tail_(nullptr) { ++n_alloced; }

        ~CellList() { --n_alloced; }

        static void init();
        static int alloced();

        inline static bool isEmpty(shared_ptr<CellList> Xs) { return nullptr == Xs; }

        static cell head(shared_ptr<CellList> Xs) {
            assert(Xs != nullptr);
            return Xs->head_;
        }
#if 0  // strange error message from this:
        static const CellList* empty = nullptr;
#endif
        static shared_ptr<CellList> tail(shared_ptr<CellList> Xs) {
            return Xs->tail_;
        }

        static shared_ptr<CellList> cons(cell X, shared_ptr<CellList> Xs) {
            shared_ptr<CellList> cl = make_shared<CellList>(X);
            cl->tail_ = Xs;
            return cl;
        }

        // O(n)
        size_t size() const {
            if (this == nullptr) return 0;
            size_t sum = 1;
            for (shared_ptr<CellList> p = tail_; p != nullptr; p = p->tail_)
                ++sum;
            return sum;
        }

        // append CellList Ys to CellList made from int array xs, return result
        static shared_ptr<CellList> concat(vector<cell> xs, shared_ptr<CellList> Ys) {
            shared_ptr<CellList> Zs = Ys;
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
        static vector<cell> toCells(shared_ptr<CellList> Xs) {
            vector<cell> is = vector<cell>();
            while (!isEmpty(Xs)) {
                cell c = head(Xs);
                is.push_back(c);
                Xs = tail(Xs);
            }
            return is;
        }

        string toString() {
            string s = "[";
            string sep = "";
            shared_ptr<CellList> x = shared_ptr<CellList>(this);
            vector<cell> elts = toCells(x);
            for (cell x : elts) {
                s += sep;
                sep = ",";
                s += "<toString cell dummy>"; // showCell(x);
            }
            return s;
        }
    };

} // end namespace