
package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
// import static org.junit.jupiter.api.Assertions.assertFalse;

public class TestIMap {
    @Test
    public void mainTest() {
        System.out.println("TestIMap entered");
        IMap<Integer> im = new IMap<Integer>();
        assertNotNull (im);
        im.put(1,2);
        IntMap imx = im.get(1);
        assertEquals(2, imx.get(1));
        System.out.println("TestIMap done");
    }
}

