package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;

public class TestProg {

    public interface TwoArgIntOperator {
        public int op(int a, int b);
    }

    //elsewhere:
    static int method(TwoArgIntOperator operator) {
        return operator.op(5, 10);
    }
    // Then call the method with a lambda as parameter:
    @Test
    public static void mainTest(String[] args) {
        TwoArgIntOperator addTwoInts = (a, b) -> a + b;
        int result = method(addTwoInts);
        System.out.println("Result: " + result);
    }
}