package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestAdd extends TestTerm {

    LPvar  the_sum_of(LPvar a1, LPvar a2, LPvar Sum)    {  return S_(a1,a2,Sum);  }
    LPvar  the_successor_of(LPvar x)                    {  return S_(x);          }
    LPvar R;
    LPvar X,Y,Z;
    LPvar  goal(LPvar x)                                {  return S_(x);          }

    @Test
    public void mainTest() {
        start_new_test();

        LPvar zero = C_("0");
        LPvar one = the_successor_of(zero);
        LPvar two = the_successor_of(one);

        say_( the_sum_of(zero, X, X));
        say_( the_sum_of(the_successor_of(X), Y, the_successor_of(Z)) )
                .if_( the_sum_of(X,Y,Z) );

        say_( goal(R)).if_(the_sum_of(two, two, R));

        String[] answer = {
                "the_successor_of(the_successor_of(the_successor_of(the_successor_of(0))))"
        };

        try_it(said, answer);
    }
}
