package org.iprolog;

import org.junit.jupiter.api.Test;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class TestProg extends TestTerm {

    public interface TermFn0 {
        public Term f0();
    }

    private class LPvar {
        TermFn0 run;
    }

    private Term[] make_xts(LPvar[] xs) {
        Term xts[] = new Term[xs.length];
        int i = 0;
        for (LPvar x : xs) {
            xts[i] = xs[i].run.f0();
            ++i;
        }
        return xts;
    }

    LPvar S_(LPvar... xs) {
        String nm = f_();  // misses the right stack frame if called as arg to s_()
        LPvar r = new LPvar();
        r.run = ()->s_(nm,make_xts(xs));
        return r;
    }

    LPvar L_(LPvar... xs) {
        LPvar r = new LPvar();
        r.run = ()->l_(make_xts(xs));
        return r;
    }

    // -----------------------------------------

    private void show_LPvar_methods() {
        Method ms[] = this.getClass().getDeclaredMethods();
        String s = "";
        String sep = "";
        for (Method m : ms) {
            String method_type = m.getReturnType().getName();
            if (method_type.endsWith("LPvar")) {
                s = s + sep + m.getName();
                sep = ", ";
            }
        }
        Main.println ("LPvar methods are: " + s);
    }

    private void show_LPvar_fields() {
        Field fs[] = this.getClass().getDeclaredFields();
        for (Field f : fs)
            if (f.getType().getName().endsWith("LPvar"))
                Main.println("   field name: " + f.getName());
    }

    Term pred(LPvar t) {
        return s_(m_(),t.run.f0());
    }
    LPvar Nothing;
    LPvar Something;
    LPvar some_struct(LPvar x) { return S_(x); }
    LPvar other_struct(LPvar x, LPvar y)  { return S_(x,y); }

    @Test
    public void mainTest() {

        Class tc = this.getClass();
        Field fs[] = tc.getDeclaredFields();

        // show_LPvar_fields();
        // show_LPvar_methods();

        try {
            for (Field f : fs)
                if (f.getType().getName().endsWith("LPvar")) {
                    LPvar x = new LPvar();
                    x.run = ()->v_(f.getName());
                    f.set(this, x);
                }
         } catch (IllegalAccessException x) {
                x.printStackTrace();
         }

        assert Something != null;
        assert Something.run != null;
        assert "Something".compareTo(Something.run.f0().toString()) == 0;
        assert Nothing != null;
        assert Nothing.run != null;
        assert "Nothing".compareTo(Nothing.run.f0().toString()) == 0;
        Term result = pred(Nothing);
        assert "pred(Nothing)".compareTo(result.toString()) == 0;
        LPvar r = some_struct(Nothing);
        assert "some_struct(Nothing)".compareTo(r.run.f0().toString()) == 0;
        r = some_struct(some_struct(Something));
        assert "some_struct(some_struct(Something))".compareTo(r.run.f0().toString()) == 0;
        r = other_struct(Something,Nothing);
        assert "other_struct(Something,Nothing)".compareTo(r.run.f0().toString()) == 0;
        r = L_();
        assert "nil".compareTo(r.run.f0().toString()) == 0;
        r = L_(Something,Nothing);
        assert "[Something,Nothing]".compareTo(r.run.f0().toString()) == 0;
    }
}