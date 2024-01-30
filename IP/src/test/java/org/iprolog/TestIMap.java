
package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;

public class TestIMap {
    @Test
    public void mainTest() {
        IMap<Integer> im = new IMap<Integer>();
        assertNotNull (im);
        im.put(1,2);
        IntMap imx = im.get(1);
        assertNotNull(imx);
        // assertEquals(2, imx.get(1));
        // System.out.println("imx size = " + imx.size());
        // System.out.println("imx get(1) = " + imx.get(1));
        // System.out.println("imx get(2) = " + imx.get(2));
    }
}


