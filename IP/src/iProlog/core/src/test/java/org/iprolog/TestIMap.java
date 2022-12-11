
package org.iprolog;

import org.junit.jupiter.api.Test;

// import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
// import static org.junit.jupiter.api.Assertions.assertFalse;

public class TestIMap {
    @Test
    public void mainTest() {
        System.out.println("TestIMap entered");
        IMap<Integer> im = new IMap<>();
        assertNotNull (im);
        im.put(1,2);
        System.out.println("TestIMap done");
    }
}


