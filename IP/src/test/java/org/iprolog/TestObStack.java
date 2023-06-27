package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;

public class TestObStack {
    @Test
    public void mainTest() {
        System.out.println("TestObStack entered");
        ObStack<Integer> osi = new ObStack<Integer>();
        assertNotNull(osi);
        osi.push(1);
        assertEquals(1,osi.peek());
        System.out.println("TestObStack exiting...");
    }
}