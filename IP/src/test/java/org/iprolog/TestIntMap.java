package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;

public class TestIntMap {
    @Test
    public void mainTest() {
        // System.out.println ("*** testing IntMap ***");
        IntMap im = new IntMap();
        assertNotNull(im);
        assertFalse(im.contains(1));

// ====================== copied and adapted from this:
// https://github.com/mikvor/hashmapTest/blob/master/src/test/java/map/objobj/ObjObjMapUnitTest.java

        final IntMap map = new IntMap();
        for ( int i = 0; i < 100000; ++i )
        {
            map.put(i, i);
            assertEquals(i + 1, map.size());
            assertEquals(Integer.valueOf(i), map.get( i ));
        }
        //now check the final state
        for ( int i = 0; i < 100000; ++i )
            assertEquals(Integer.valueOf(i), map.get( i ));

        // Main.println ("Done with adapted tests");
//=======================

        im.put(1,2);
        assert(im.contains(1));
        int i = im.get(1);
        assertEquals (2, i);
        im.remove(1);
        assert(im.isEmpty());
        assertEquals(0,im.size());
        
        // Not clear yet how to test IntMap's intersect(...)

        // System.out.println ("*** test IntMap DONE ***");
    }

}
