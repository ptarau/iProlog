package org.iprolog;

import java.util.ArrayList;
import java.util.LinkedHashMap;

public class sym_tab {
    /* Symbol table - made of map (syms) + reverse map from ints to syms (slist) */
    private final LinkedHashMap<String, Integer> syms; // syms->ints
    private final ArrayList<String> slist;     // resizeable ints->syms

    sym_tab() {
        syms = new LinkedHashMap<String, Integer>();
        slist = new ArrayList<String>();
    }

    /**
     * Places an identifier in the symbol table.
     */
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

    /**
     * Returns the symbol associated to an integer index
     * in the symbol table.
     */
    public final String getSym(final int w) {
        if (w < 0 || w >= slist.size())
            return "BADSYMREF=" + w;
        return slist.get(w);
    }

}
