package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;

public class TestIntStack {
    @Test
    public void mainTest() {
        System.out.println ("*** test IntStack ***");
        IntStack is = new IntStack();
        assertNotNull(is);
        assert(is.isEmpty());
        is.push(1);
        assertFalse(is.isEmpty());
        System.out.println ("is="+is.toString());
        int [] aa = is.toArray();
        assertEquals(1,aa.length);
        int i = is.get(0);
        assertEquals(1,i);
        is.set(0,2);
        i = is.pop();
        assertEquals(2,i);
        assert(is.isEmpty());
        System.out.println ("*** test IntStack DONE ***");
    }
    
}
