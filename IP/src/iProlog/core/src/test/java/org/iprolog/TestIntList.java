package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;

import org.iprolog.IntList;

public class TestIntList {

    @Test
    public void mainTest() {
        assertEquals(1,1);
        System.out.println ("<<<<<<< TestIntList >>>>>>>>");
        IntList il = IntList.cons(1,null);
        assertNotNull(il);
        System.out.println ("<<<<<<< TestIntList DONE >>>>>>>>");
    }

    public static void main(final String[] args) {
        System.out.println ("--------- TestIntList something ------------");
    }
}
