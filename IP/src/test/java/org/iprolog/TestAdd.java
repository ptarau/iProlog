package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestAdd extends TestTerm {
    LP_ the_sum_of_(LP_ a1, LP_ a2, LP_ Sum) {  return S_(a1,a2,Sum); } // args: triple
    LP_ X,Y,Z,R;
    TestAdd() {
        say_( the_sum_of_(zero_.and_,X.is_,X));
        say_( the_sum_of_(one_plus_(X).and_,Y.is_,one_plus_(Z)) )
                .if_( the_sum_of_(X.and_,Y.is_,Z) );
        say_( good_(R)).if_(the_sum_of_(two_.and_,two_.is_, R));
    }

    LP_ one_plus_(LP_ x) {  return S_(x); } // args: singleton structure
    LP_ zero_ = L_();   // empty list, will show as "nil"
    LP_ one_  = one_plus_(zero_);
    LP_ two_  = one_plus_(one_);

    @Test
    public void mainTest() {
        String[] answer = {
                "one_plus_(one_plus_(one_plus_(one_plus_(nil))))"
        };
        try_it(said, answer);
    }
}
