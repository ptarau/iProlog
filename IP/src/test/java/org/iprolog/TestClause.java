package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;


import java.util.LinkedList;

public class TestClause {

    @Test
    public void mainTest() {
        Clause ft;
        
        ft = Clause.f__("fun");
        assert ft != null;
        Term a = Term.constant("a");
        assert a != null;
        Term b = Term.constant("b");

        Clause ft_a = ft.__(a);
        assert ft_a != null;
        assert ft_a == ft;
        Clause ft_a_if = ft_a.if_();
        Clause ft_a_if_b = ft_a_if.__(b);
        assert ft == ft_a_if_b;

        Main.println ("ft.head = " + ft.head.toString());
        Main.println ("ft.body = " + ft.body.toString());
    }
}
