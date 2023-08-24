package org.iprolog;

import org.junit.jupiter.api.Test;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class TestProg extends TestTerm {

    Term pred(LPvar t) {
        return s_(m_(),t.run.fn());
    }

    LPvar Nothing;
    LPvar Something;
    LPvar some_struct(LPvar x)              { return S_(x);     }
    LPvar other_struct(LPvar x, LPvar y)    { return S_(x,y);   }
    LPvar goal(LPvar x)                     { return S_(x);     }
    LPvar pair(LPvar x, LPvar y)            { return P_(x,y);   }
    LPvar is_zero(LPvar x) { return S_(x); }
    LPvar X;

    void try_matching (String output, LPvar f) {
        assert output != null;
        assert f != null;
        assert output.compareTo(f.run.fn().toString()) == 0;
    }

    @Test
    public void mainTest() {

        start_new_test();

        // show_LPvar_fields();
        // show_LPvar_methods();

        Term.set_Prolog();

        assert Something != null;
        assert Something.run != null;
        try_matching ("Something",Something);
        assert Nothing != null;
        assert Nothing.run != null;
        try_matching ("Nothing",Nothing);
        Term result = pred(Nothing);
        assert "pred(Nothing)".compareTo(result.toString()) == 0;
        try_matching ("some_struct(Nothing)",some_struct(Nothing));
        try_matching ("some_struct(some_struct(Something))",some_struct(some_struct(Something)));
        try_matching ("other_struct(Something,Nothing)",other_struct(Something,Nothing));
        try_matching ("nil",L_());
        try_matching ("[Something,Nothing]",L_(Something,Nothing));
        try_matching ("[Something|Nothing]",pair(Something,Nothing));
        try_matching ("[0|[Something|Nothing]]",P_(C_("0"),Something,Nothing));

        LPvar zero = C_("0");
        say_(is_zero(zero));
        say_(goal(X)).if_(is_zero(X));

        String expected[] = { "0" };

        try_it (said, expected, true);
    }
}