/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "spine.h"

namespace iProlog {

    using namespace std;

    Spine *Spine::free_list = nullptr;

    /**
     * "Creates a spine - as a snapshot of some runtime elements."
     * 
     * disaggregate hga_0 into reloced_head, reloced_body, and body_len instead
     * of hg_len. Delete unfold.h. Maybe the relocations can even be done
     * in parallel with building the CellList.
     */
    Spine* Spine::new_Spine(
        Clause& C0,
        cell b,
        int base_0,                 // base
        CL_p goals_0,               // was gs[Java]; tail of G->goals in unfold()
        int trail_top_0,
        Unifiables &unifiables_0)
    {
#define TR if(0)
        Spine* sp = Spine::alloc();

        sp->base = base_0;
        sp->trail_top = trail_top_0;
        sp->last_clause_tried = 0;
        sp->unifiables = unifiables_0;

        for (int i = 0; i < MAXIND; ++i)
            sp->index_vector[i] = cell::BAD;

        sp->skel_len = (int)C0.skel_len;

        CL_p acc = goals_0;
        for (int i = sp->skel_len - 1; i > 0; i--)
            acc = CellList::cons(C0.skel[size_t(i)].relocated_by(b), acc);
        sp->the_goals = acc;
        sp->head = C0.skel[0].relocated_by(b);

        return sp;
#undef TR
    }

    /**
     * "Creates a specialized spine returning an answer (with no goals left to solve)." {Spine.java]
     */
    Spine *Spine::new_Spine(cell h, int tt) {
        Spine* sp = Spine::alloc();
        sp->head = h;
        sp->base = 0;
        sp->the_goals = nullptr;
        sp->skel_len = 0;
        sp->trail_top = tt;
        sp->last_clause_tried = -1;
        for (int i = 0; i < MAXIND; ++i)
            sp->index_vector[i] = cell::BAD;
        return sp;
    }

    string Spine::show() const {
        string s = "Spine: {";
        s += "\n  head=" + to_string(head.as_int());
        s += "\n  base=" + to_string(base);
        if (the_goals == nullptr)
            s += "\n  goals=<null>";
        else
            s += "\n  goals=<stub>" /* + goals.show()*/;
        s += "\n  trail_top=" + to_string(trail_top);
        s += "\n  last_clause_tried=" + to_string(last_clause_tried);
        if (unifiables.empty())
            s += "\n  unifiables=<null>";
        else
            s += "\n  unifiables=<stub>" /* + unifiables.show()*/;
        s += "\n}";
        return s;
    }

    Spine::~Spine() {
        abort();
    }

} // end namespace
