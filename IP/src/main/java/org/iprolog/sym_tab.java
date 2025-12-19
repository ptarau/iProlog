package org.iprolog;

import java.util.ArrayList;
import java.util.LinkedHashMap;

public class sym_tab {
    // Symbol table - made of map (syms) + reverse map from ints to syms (slist) */

    private final ArrayList<String> slist;     // resizeable ints->sym
/*
    //  linear search only to test whether just "int" is enough
    // for the logic of iProlog execution.
    // Sadly, "LinkedHashMap<String, int> syms" is disallowed.
    // And yes, it turns out iProlog can do without "new Integer" here.
    sym_tab() {
        slist = new ArrayList<String>();
    }

    public final int addSym(final String sym) {
        int last_size = slist.size();
        for (int i = 0; i < last_size; ++i)
            if (sym.equals(slist.get(i)))
                return i;

        slist.add(sym);
        return last_size;
    }
*/

    private final LinkedHashMap<String, Integer> syms; // syms->ints

    sym_tab() {
        syms = new LinkedHashMap<>();
        slist = new ArrayList<String>();
    }

    //
    //Places an identifier in the symbol table.
    //
    public final int addSym(final String sym) {
        Integer I = syms.get(sym);
        if (null == I) {
            final int i = syms.size();
            I = new Integer(i);
            syms.put(sym, I);
            slist.add(sym);
        }
        return I.intValue();
    }

    //
    // Returns the symbol associated to an integer index
    //in the symbol table.
    //
    public final String getSym(final int w) {
        if (w < 0 || w >= slist.size())
            return "BADSYMREF=" + w;
        return slist.get(w);
    }
}
