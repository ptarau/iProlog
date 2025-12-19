package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;

public class TestObStack {
    @Test
    public void mainTest() {
        ObStack<Integer> osi = new ObStack<Integer>();
        assertNotNull(osi);
        osi.push(1);
        assertEquals(1,osi.peek());
    }
}