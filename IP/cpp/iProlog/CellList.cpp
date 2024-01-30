#include "defs.h"
#include <assert.h>

#include "cell.h"
#include "CellList.h"

namespace iProlog {

    using namespace std;

    int CellList::n_alloced = 0;

    void CellList::init() {
        n_alloced = 0;
    }

    int CellList::alloced() {
        return n_alloced;
    }
}

