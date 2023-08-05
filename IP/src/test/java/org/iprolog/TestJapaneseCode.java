package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestJapaneseCode extends TestTerm {

        Term いきる(Term x)   {  return s_(m_(), x);      }
        Term 人()            {  return v_(m_());         }
        Term いいです(Term x) {  return s_(m_(), x);      }
        Term goal(Term x)   {  return s_(m_(), x);      }

        @Test
        public void mainTest() {
            start_new_test();
            String expected[] = {"私", "あなた"};
            for (String s : expected)
                say_( いきる (c_(s)));
            say_( いいです ( 人 ())).if_( いきる (人 ()));
            say_(goal( 人 ())).if_( いいです ( 人 ()));
            try_it(said, expected);
        }
}
