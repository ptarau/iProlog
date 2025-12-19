#include "defs.h"
#include <assert.h>

#include "cell.h"
#include "CellList.h"

namespace iProlog {

    using namespace std;

    int CellList::n_alloced = 0;
    CL_p CellList::free_list = nullptr;

    string CellList::toString() {
        string s = "[";
        string sep = "";
#if 0
        CL_p start = mk_shared(head(), tail());
        for (CellList* cln = this; cln != nullptr; cln = CellList::tail(cln) {
            s += sep;
            sep = ",";
            s += "<toString cell dummy>"; // showCell(x);
        }
#else
        s += " *** CellList::toString() STUB ***";
#endif
        s += "]";
        return s;
    }
}

