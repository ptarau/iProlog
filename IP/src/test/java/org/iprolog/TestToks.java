package org.iprolog;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNull;

public class TestToks {

    public static String case1() {

        String in = "";
        in += "live_ i_ .\n";
        in += "goal X\n";
        in += "if\n";
        in += "  live_ X .\n";

        return in;
    }

    @Test
    public void mainTest () {
        System.out.println ("<<<<<<<<<< Test Toks >>>>>>>>>>>");

        Toks T = Toks.makeToks("Hello 123 nothing .", false);
        assertNotNull(T);

        String s = T.getWord();
        assert(s.compareTo("v:Hello") == 0);
        s = T.getWord();
        assert(s.compareTo("n:123") == 0);
        s = T.getWord();
        assert(s.compareTo("c:nothing") == 0);
        s = T.getWord();
        assert(s.compareTo(".") == 0);
        s = T.getWord();
        assertNull(s);

        Main.pp(Toks.toSentences(case1(), false));
    }

  public static void main(final String[] args) {

        // Main.pp(Toks.toSentences("core/resources/t.pl.nl", true));
      Main.pp(Toks.toSentences(case1(), false));
  }
}
