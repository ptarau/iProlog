package iProlog;

import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * Reads chars from char streams using the current default encoding
 */
public class Toks extends StreamTokenizer {

    // reserved words - with syntactic function

    private static final String IF = "if";
    private static final String AND = "and";
    private static final String DOT = ".";
    private static final String HOLDS = "holds";
    private static final String LISTS = "lists"; // todo
    private static final String IS = "is"; // todo

    private Toks(Reader reader) {
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

    private static Toks toks(String s, boolean fromFile) {
        try {
            Reader R;
            R = fromFile ? new FileReader(s) : new StringReader(s);
            Toks T = new Toks(R);
            return T;

        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }

    public static ArrayList<ArrayList<ArrayList<String>>> sentences(String s, boolean fromFile) {
        ArrayList<ArrayList<ArrayList<String>>> Wsss = new ArrayList<>();
        ArrayList<ArrayList<String>> Wss = new ArrayList<>();
        ArrayList<String> Ws = new ArrayList<>();
        Toks toks = toks(s, fromFile);
        String t = null;
        while (null != (t = toks.getWord())) {

            if (DOT.equals(t)) {
                Wss.add(Ws);
                Wsss.add(Wss);
                Wss = new ArrayList<>();
                Ws = new ArrayList<>();
            } else if (("c:" + IF).equals(t)) {

                Wss.add(Ws);

                Ws = new ArrayList<>();
            } else if (("c:" + AND).equals(t)) {
                Wss.add(Ws);

                Ws = new ArrayList<>();
            } else if (("c:" + HOLDS).equals(t)) {
                String w = Ws.get(0);
                Ws.set(0, "h:" + w.substring(2));
            } else if (("c:" + LISTS).equals(t)) {
                String w = Ws.get(0);
                Ws.set(0, "l:" + w.substring(2));
            } else if (("c:" + IS).equals(t)) {
                String w = Ws.get(0);
                Ws.set(0, "f:" + w.substring(2));
            } else {
                Ws.add(t);
            }
        }
        return Wsss;
    }

    static String toString(Object[] Wsss) {
        return Arrays.deepToString(Wsss);
    }

    public static void main(String[] args) {

        Main.pp(sentences("prog.nl", true));
    }

    private String getWord() {
        String t = null;

        int c = StreamTokenizer.TT_EOF;
        try {
            c = nextToken();
            while (Character.isWhitespace(c) && c != StreamTokenizer.TT_EOF) {
                c = nextToken();
            }
        } catch (IOException ignored) {
            return "*** tokenizer error:" + ignored;
        }

        switch (c) {
            case StreamTokenizer.TT_WORD: {
                char first = sval.charAt(0);
                if (Character.isUpperCase(first) || '_' == first) {
                    t = "v:" + sval;
                } else {
                    try {
                        int n = Integer.parseInt(sval);
                        t = Math.abs(n) < 1 << 28 ? "n:" + sval : "c:" + sval;
                    } catch (Exception ignored) {
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
}