package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestProg extends TestTerm {

    Term pred(LP_ t) {
        return s_(m_(),t.run.fn());
    }

    LP_ Nothing;
    LP_ Something;
    LP_ some_struct(LP_ x)            { return S_(x);     }
    LP_ other_struct(LP_ x, LP_ y)    { return S_(x,y);   }
    LP_ pair(LP_ x, LP_ y)            { return P_(x,y);   }
    LP_ is_zero(LP_ x)                { return S_(x); }
    LP_ X;

    void try_matching (String output, LP_ f) {
        assert output != null;
        assert f != null;
        assert output.compareTo(f.run.fn().toString()) == 0;
    }

    @Test
    public void mainTest() {

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

        LP_ zero = C_("0");
        say_(is_zero(zero));
        say_(good_(X)).if_(is_zero(X));

        String expected[] = { "0" };

        try_it (said, expected, true);
    }
}