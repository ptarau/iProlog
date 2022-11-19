package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;

import org.iprolog.IntList;

public class TestIntList {

    @Test
    public void mainTest() {
        System.out.println ("<<<<<<< TestIntList >>>>>>>>");


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
        IntList ilx = IntList.app(xs,il2);
        assertEquals(4,IntList.len(ilx));

        System.out.println ("ilx = " + ilx.toString());

        IntStack is = IntList.toInts(ilx);
        System.out.println ("is = " + is.toString());
        System.out.println ("<<<<<<< TestIntList DONE >>>>>>>>");
    }

    public static void main(final String[] args) {
        System.out.println ("--------- TestIntList something ------------");
    }
}
