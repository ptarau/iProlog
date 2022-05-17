package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.iprolog.Toks;

public class TestToks {
    @Test
    public void mainTest () {
        assertEquals(1,1);

        System.out.println ("{<<<<<<<<<<< Test Toks >>>>>>>>>>>");
        Main.pp(Toks.toSentences("resources/t.pl.nl", true));

        String in = "";
        in += "live_ i_ .\n";
        in += "goal X\n";
        in += "if\n";
        in += "  live_ X .\n";
        
        Main.pp(Toks.toSentences(in, false));
    }

  public static void main(final String[] args) {
    Main.pp(Toks.toSentences("core/resources/t.pl.nl", true));
  }
}
