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

#ifdef RAW_CELL_LIST
#define CL_p CellList *
#else
#define CL_p shared_ptr<CellList>
#endif

    class CellList {
    private:
        static CL_p free_list;  // Free list of CellList records.
                                // Note that in a multi-engine context this
                                //  will be global to all engines.
                                // It could be a good thing, but in a
                                // single-shot context, it may make sense to
                                // to have an Engine destructor that frees
                                // up this CellList reserve. 
        cell head_;
        CL_p tail_;
        static int n_alloced;
#ifdef MEASURE_CELL_LIST
# define _add_(c) { n_alloced += (c); }
#else
# define _add_(c) 
#endif
        inline static void inc_n_alloced() { _add_(1);   }
        inline static void dec_n_alloced() { _add_(-1);  }
#undef _add_

    public:
        // Singleton list
        CellList() { head_ = 0; tail_ = nullptr; inc_n_alloced(); }
  
        CellList(cell h) : head_(h), tail_(nullptr) { inc_n_alloced(); }

        ~CellList() { dec_n_alloced(); }

        static int alloced() { return n_alloced; }

        inline static bool isEmpty(const CL_p Xs) { return nullptr == Xs; }

        inline static cell head(const CL_p Xs) {
            assert(Xs != nullptr);
            return Xs->head_;
        }

        inline static CL_p tail(const CL_p Xs) {
            return Xs->tail_;
        }

        inline static CL_p mk_shared(cell X) {
            if (free_list == nullptr) {
#ifdef RAW_CELL_LIST
                return new CellList(X);
#else
                inc_n_alloced();
                return make_shared<CellList>(X);
#endif
            }
            CL_p r = free_list;
            free_list = tail(r);
            r->tail_ = nullptr;
            r->head_ = X;
            return r;
        }

        inline static CL_p mk_shared() {
            return mk_shared(cell::BAD);
        }

        inline static void collect_n(CL_p clp, int k)
        {
            CL_p start = clp;
            
            while (k-- > 0)
                clp = tail(clp);
            if (clp != nullptr)
                clp->tail_ = free_list;
            free_list = start;
        }

        inline static CL_p cons(cell X, /*const*/CL_p Xs) {
            CL_p cl = mk_shared(X);
            cl->tail_ = Xs;
            return cl;
        }

        // O(n)
        size_t size() const {
            if (this == nullptr) return 0;
            size_t sum = 1;
            for (CL_p p = tail_; p != nullptr; p = p->tail_)
                ++sum;
            return sum;
        }

        string toString();
    };

} // end namespace
