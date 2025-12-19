package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;

public class TestIntList {

    @Test
    public void mainTest() {
        IntList il = IntList.cons(1,null);
        assertNotNull(il);
        assertFalse(IntList.isEmpty(il));
        assertEquals(1,IntList.head(il));
        assertEquals(1,IntList.len(il));

        IntList il2 = IntList.cons(2,il);
        assertNotNull(il2);
        assertFalse(IntList.isEmpty(il2));
        assertEquals(2,IntList.head(il2));

        final int [] xs = { 3, 4 };
        assertEquals(2,xs.length);
        assertEquals(2,IntList.len(il2));
        IntList ilx = IntList.concat(xs,il2);
        assertEquals(4,IntList.len(ilx));
        int iii = IntList.head(IntList.tail(ilx));
        assertEquals(iii,xs[1]);

        // System.out.println ("ilx = " + ilx.toString());

        IntStack is = IntList.toInts(ilx);
        
        // System.out.println ("is = " + is.toString());
    }
}
