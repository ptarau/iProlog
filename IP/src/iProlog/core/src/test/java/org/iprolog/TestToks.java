package org.iprolog;

import org.junit.jupiter.api.Test;

import org.iprolog.Toks;

public class TestToks {

    public String case1() {

        String in = "";
        in += "live_ i_ .\n";
        in += "goal X\n";
        in += "if\n";
        in += "  live_ X .\n";

        return in;
    }

    @Test
    public void mainTest () {
        System.out.println ("{<<<<<<<<<<< Test Toks >>>>>>>>>>>");

        Main.pp(Toks.toSentences(case1(), false));
    }

  public static void main(final String[] args) {
    Main.pp(Toks.toSentences("core/resources/t.pl.nl", true));
  }
}
