package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestAdd extends TestTerm {
    Term  the_sum_of(Term a1, Term a2, Term Sum)    {  return s_(m_(), a1,a2,Sum);  }
    Term  the_successor_of(Term x)                  {  return s_(m_(), x);          }

    @Test
    public void mainTest() {
        start_new_test();

        Term succ_X = the_successor_of(X());
        Term succ_Z = the_successor_of(Z());
        Term one    = the_successor_of(c0());
        Term vR = v_("R");

        say_( the_sum_of(c0(), X(), X()));
        say_( the_sum_of(succ_X, Y(), succ_Z)).if_(the_sum_of(X(),Y(),Z()));

        Term two = the_successor_of(one);
        say_( goal(vR)).if_(the_sum_of(two, two, vR));

        String[] these_answers = {
                "the_successor_of(the_successor_of(the_successor_of(the_successor_of(0))))"
        };

        try_it(said, these_answers);
    }
}
