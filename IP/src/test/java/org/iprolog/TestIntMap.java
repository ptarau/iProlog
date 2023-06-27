package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;

public class TestIntMap {
    @Test
    public void mainTest() {
        System.out.println ("*** testing IntMap ***");
        IntMap im = new IntMap();
        assertNotNull(im);
        assertFalse(im.contains(1));
        im.put(1,2);
        assert(im.contains(1));
        int i = im.get(1);
        assertEquals (2, i);
        im.remove(1);
        assert(im.isEmpty());
        assertEquals(0,im.size());
        
        // Not clear yet how to test IntMap's intersect(...)

        System.out.println ("*** test IntMap DONE ***");
    }

}
