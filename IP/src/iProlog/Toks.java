package iProlog;
import java.io.*;
import java.util.*;

/**
 * Reads chars from char streams using the current default encoding
 */
public class Toks extends StreamTokenizer {

  // reserved words - with syntactic function

  public static String IF = "if";
  public static String AND = "and";
  public static String DOT = ".";
  public static String HOLDS = "holds";
  public static String LISTS = "lists"; // todo
  public static String IS = "is"; // todo

  public static Toks makeToks(final String s, final boolean fromFile) {
    try {
      Reader R;
      if (fromFile) {
        R = new FileReader(s);
      } else {
        R = new StringReader(s);
      }
      final Toks T = new Toks(R);
      return T;

    } catch (final IOException e) {
      e.printStackTrace();
      return null;
    }
  }

  public Toks(final Reader reader) {
    super(reader);
    resetSyntax();
    eolIsSignificant(false);
    ordinaryChar('.');
    ordinaryChars('!', '/'); // 33-47
    ordinaryChars(':', '@'); // 55-64
    ordinaryChars('[', '`'); // 91-96
    ordinaryChars('{', '~'); // 123-126
    wordChars('_', '_');
    wordChars('a', 'z');
    wordChars('A', 'Z');
    wordChars('0', '9');
    slashStarComments(true);
    slashSlashComments(true);
    ordinaryChar('%');
  }

  public String getWord() {
    String t = null;

    int c = TT_EOF;
    try {
      c = nextToken();
      while (Character.isWhitespace(c) && c != TT_EOF) {
        c = nextToken();
      }
    } catch (final IOException e) {
      return "*** tokenizer error:" + t;
    }

    switch (c) {
      case TT_WORD: {
        final char first = sval.charAt(0);
        if (Character.isUpperCase(first) || '_' == first) {
          t = "v:" + sval;
        } else {
          try {
            final int n = Integer.parseInt(sval);
            if (Math.abs(n) < 1 << 28) {
              t = "n:" + sval;
            } else {
              t = "c:" + sval;
            }
          } catch (final Exception e) {
            t = "c:" + sval;
          }
        }
      }
      break;

      case StreamTokenizer.TT_EOF: {
        t = null;
      }
      break;

      default: {
        t = "" + (char) c;
      }

    }
    return t;
  }

  public static ArrayList<ArrayList<ArrayList<String>>> toSentences(final String s, final boolean fromFile) {
    final ArrayList<ArrayList<ArrayList<String>>> Wsss = new ArrayList<ArrayList<ArrayList<String>>>();
    ArrayList<ArrayList<String>> Wss = new ArrayList<ArrayList<String>>();
    ArrayList<String> Ws = new ArrayList<String>();
    final Toks toks = makeToks(s, fromFile);
    String t = null;
    while (null != (t = toks.getWord())) {

      if (DOT.equals(t)) {
        Wss.add(Ws);
        Wsss.add(Wss);
        Wss = new ArrayList<ArrayList<String>>();
        Ws = new ArrayList<String>();
      } else if (("c:" + IF).equals(t)) {

        Wss.add(Ws);

        Ws = new ArrayList<String>();
      } else if (("c:" + AND).equals(t)) {
        Wss.add(Ws);

        Ws = new ArrayList<String>();
      } else if (("c:" + HOLDS).equals(t)) {
        final String w = Ws.get(0);
        Ws.set(0, "h:" + w.substring(2));
      } else if (("c:" + LISTS).equals(t)) {
        final String w = Ws.get(0);
        Ws.set(0, "l:" + w.substring(2));
      } else if (("c:" + IS).equals(t)) {
        final String w = Ws.get(0);
        Ws.set(0, "f:" + w.substring(2));
      } else {
        Ws.add(t);
      }
    }
    return Wsss;
  }

  static String toString(final Object[] Wsss) {
    return Arrays.deepToString(Wsss);
  }

  public static void main(final String[] args) {
    Main.pp(toSentences("prog.nl", true));
  }
}