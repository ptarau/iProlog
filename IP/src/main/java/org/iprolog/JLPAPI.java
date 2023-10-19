package org.iprolog;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.LinkedList;

public class JLPAPI {
    public static Term v_(String s) {    return Term.variable(s); }
    public String m_() {
        return Thread.currentThread().getStackTrace()[2].getMethodName();
    }
    public Term v_()                {    return v_(m_());         }

    static Term _()          {    return v_("_");        }
    static Term c_(String s) {    return Term.constant(s); }
    static Term s_(String s) {    return Term.compound(s); }
    static Term s_(String s, Term... ts) {
        Term xt = Term.compound(s);
        for (Term t : ts)
            xt = xt.takes_this(t);
        return xt;
    }
    public static Term e_(Term lhs, Term rhs) { return Term.equation (lhs,rhs); }
    public static Term l_(Term... ts) {
        if (ts.length == 0)
            return c_("nil");
        return Term.termlist(ts);
    }
    static Term pal_(Term[] tal, int i) {
        if (i == tal.length-2)
            return Term.termpair(tal[i], tal[i+1]);
        return Term.termpair (tal[i], pal_(tal, i+1));
    }
    public static Term p_(Term... Ts) { return pal_(Ts, 0);  }

    public Clause yes_ (Term hd) {     return Clause.f__(hd); }

    public String f_() {
        return Thread.currentThread().getStackTrace()[3].getMethodName();
    }

    public Term call(Term f, Term... ts) { return s_(f.v(),ts); }

    LPvar call(LPvar f, LPvar... ls)       {
        LPvar r = new LPvar();
        r.run = ()->s_(f.run.fn().toString(), make_xts(ls));
        return r;
        // return S_(ls);
    }

    protected Term[] make_xts(LPvar[] xs) {
        Term[] xts = new Term[xs.length];
        int i = 0;
        for (LPvar x : xs) {
            xts[i] = xs[i].run.fn();
            ++i;
        }
        return xts;
    }

    Term true_()                  { return null;         }  // ????

    Term c0() { return c_("0"); }
    Term c1() { return c_("1"); }
    Term c2() { return c_("2"); }
    Term c3() { return c_("3"); }
    Term c4() { return c_("4"); }

    Term V()  { return v_(m_()); }
    Term X()  { return v_(m_()); }
    Term Y()  { return v_(m_()); }
    Term Z()  { return v_(m_()); }

    Term Xs() { return v_(m_()); }
    Term Ys() { return v_(m_()); }
    Term Zs() { return v_(m_()); }

    Term goal(Term x)  { return s_(m_(),x); }

    LPvar S_(LPvar... xs) {
        String nm = f_();  // misses the right stack frame if called as arg to s_()
        LPvar r = new LPvar();
        r.run = ()->s_(nm,make_xts(xs));
        return r;
    }

    LPvar C_(String c) {
        LPvar r = new LPvar();
        r.run = ()->c_(c);
        return r;
    }

    LPvar L_(LPvar... xs) {
        LPvar r = new LPvar();
        r.run = ()->l_(make_xts(xs));
        return r;
    }

    LPvar P_(LPvar x, LPvar y) {
        LPvar r = new LPvar();
        r.run = ()->p_(x.run.fn(),y.run.fn());
        return r;
    }
    LPvar paf_ (LPvar[] taf, int i) {
        LPvar r = new LPvar();
        if (i == taf.length-2) {
            r.run = () -> (Term.termpair(taf[i].run.fn(), taf[i + 1].run.fn()));
            return r;
        }
        r.run = ()->(Term.termpair (taf[i].run.fn(), paf_(taf, i+1).run.fn()));
        return r;
    }
    LPvar P_(LPvar... Fs) {  return paf_ (Fs, 0);  }

    public void init_LPvars (Class<?> tc) {
        // Main.println ("Entering init_LPvars, class: " + tc.getName());
        Field[] fs = tc.getDeclaredFields();

        try {
            for (Field f : fs)
                if (f.getType().getName().endsWith("LPvar")) {
                    LPvar x = new LPvar();
                    x.run = () -> v_(f.getName());
                    f.set(this, x);
                    // Main.println (f.getName());
                }
        } catch (IllegalAccessException x) {
            x.printStackTrace();
        }
    }

    protected void init_LPvars() {
        Class tc = this.getClass();
        init_LPvars(tc);
        tc = tc.getSuperclass();
        init_LPvars(tc);
    }

    protected void show_LPvar_methods() {
        Method[] ms = this.getClass().getDeclaredMethods();
        StringBuilder s = new StringBuilder();
        String sep = "";
        for (Method m : ms) {
            String method_type = m.getReturnType().getName();
            if (method_type.endsWith("LPvar")) {
                s.append(sep).append(m.getName());
                sep = ", ";
            }
        }
        Main.println ("LPvar methods are: " + s);
    }

    protected void show_LPvar_fields() {
        Field[] fs = this.getClass().getDeclaredFields();
        for (Field f : fs)
            if (f.getType().getName().endsWith("LPvar"))
                Main.println("   field name: " + f.getName());
    }

    public LinkedList<Clause> said;

    private Clause say_(Clause cl) {
        assert said != null;
        said.add (cl);
        return cl;
    }

    protected Clause say_(LPvar hd) {
        assert said != null;
        Term t = hd.run.fn();
        assert t != null;
        Clause cl = yes_(hd.run.fn());
        assert cl != null;
        said.add(cl);
        return cl;
    }
    protected Clause say_(Term hd)   {
        assert said != null;
        Clause cl = yes_(hd);
        said.add (cl);
        return cl;
    }

    public Prog compile() {
        StringBuilder asm_txt;
        Term.reset_gensym();
        Term.set_TarauLog();

        // Main.println ("   ===== try_it(): flattening transform =======");
        asm_txt = new StringBuilder();
        for (Clause cl : said) {
            cl.flatten();
            asm_txt.append(cl.toString()).append(System.lineSeparator());
        }
        // Main.println ("   ===== try_it(): after flattening =======");
        // Main.println ("asm_txt = \n" + asm_txt);

        // Main.println ("   ===== try_it(): Calling new Prog: ===============");
        return new Prog(asm_txt.toString(), false);
    }
}
