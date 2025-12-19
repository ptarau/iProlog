package org.iprolog;
import java.io.*;
import java.util.*;

/**
 * Reads chars from char streams using the current default encoding
 * 
 * Most of the tokenization is for standard(?) Prolog
 */
public class Toks extends StreamTokenizer {

  // reserved words - with syntactic function
  // Would need guaranteed Term.set_Taraulog at class
  // initialization time to get the right reserved words.
  public static String IF = /* Term.if_sym.trim(); */ "if";
  public static String AND = /* Term.and_op.trim(); */ "and";
  public static String DOT = /* Term.clause_end.trim(); */ ".";
  public static String HOLDS = /* Term.holds_op.trim(); */ "holds";

  public static String LISTS = "lists"; // todo
  public static String IS = "is"; // "todo"

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

  // To support deviation from Prolog conventions for
  // variables and constants, I have calls here to Term's
  // remove_any_Var_prefix and remove_any_Const_prefix.

  public String getWord() {
    String t = null;

    // discard any leading whitespace:
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
          t = "v:" + Term.remove_any_Var_prefix(sval);
        } else {
          try {
            final int n = Integer.parseInt(sval);
            if (Math.abs(n) < Engine.MAX_N) {
              t = "n:" + sval;              // N: int constant
            } else {
              t = "c:" + sval;              // C: constant (see Engine)
            }
          } catch (final Exception e) {
            t = "c:" + Term.remove_any_Const_prefix(sval);
          }
        }
      }
      break;

      case StreamTokenizer.TT_EOF: {
        t = null;
      }
      break;

      default: {
        t = "" + (char) c;  // to convert int c to string?
      }

    }
    return t;
  }

  // Clauses: list of clauses each of which is ...
  // Structures: list of structures, each of which is ...
  // Tokens: list of (token) strings
  
  public static ArrayList<ArrayList<ArrayList<String>>>
  toSentences(final String s, final boolean fromFile) {
    final ArrayList<ArrayList<ArrayList<String>>> Clauses = new ArrayList<ArrayList<ArrayList<String>>>();
    ArrayList<ArrayList<String>> Structures = new ArrayList<ArrayList<String>>();
    ArrayList<String> Tokens = new ArrayList<String>();
    final Toks toks = makeToks(s, fromFile);
    String t = null;
/*
    // preface with a "not(...)" that must fail
    // Note that negation in unfold() depends on not being based at heap[0]
    Tokens.add("c:not");
    Tokens.add("v:_0");
    Structures.add(Tokens);
    Tokens = new ArrayList<String>();

 //   Tokens.add("u:_0");
    Tokens.add("n:0");
    Structures.add(Tokens);      // add this finished (?) structure
    Clauses.add(Structures);    // add it to this finished clause

    // prepare for (possible) new clause and structure
    Structures = new ArrayList<ArrayList<String>>();
    Tokens = new ArrayList<String>();
*/
    while (null != (t = toks.getWord())) {
      if (DOT.equals(t)) {
          Structures.add(Tokens);      // add this finished (?) structure
          Clauses.add(Structures);    // add it to this finished clause
                            // prepare for (possible) new clause and structure
          Structures = new ArrayList<ArrayList<String>>();
          Tokens = new ArrayList<String>();

      } else if (("c:" + IF).equals(t)
              || ("c:" + AND).equals(t)) {

          Structures.add(Tokens);  // finished with this structure
          Tokens = new ArrayList<String>();

      } else if (("c:" + HOLDS).equals(t)) {

          final String w = Tokens.get(0);
          // Prog.println(">>>>>>> Toks: w = " + w);
          Tokens.set(0, "h:" + w.substring(2));

      } else 
      
    // to do?
      
      if (("c:" + LISTS).equals(t)) {

          final String w = Tokens.get(0);
          Tokens.set(0, "l:" + w.substring(2));

      } else if (("c:" + IS).equals(t)) {

          final String w = Tokens.get(0);
          Tokens.set(0, "f:" + w.substring(2));  // why "f"???

      } else {
          Tokens.add(t);
      }
    }
    /*
    int i = 0;
    for (ArrayList<ArrayList<String>> cl : Clauses)
	    for (ArrayList<String> s1 : cl)
		    for (String s0 : s1)
			    Main.println ("[" + i++ + "] " + s0);
     */
    return Clauses;
  }


  static String toString(final Object[] Clauses) {
    return Arrays.deepToString(Clauses);
  }

  public static void main(final String[] args) {
    Main.pp(toSentences("core/resources/t.pl.nl", true));
  }
}
