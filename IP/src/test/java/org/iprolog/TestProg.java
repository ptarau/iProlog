package org.iprolog;

/*
The direction to take with this:
  - use reflection to read field values
  - try to see how those fields were annotated
  - initialize them with lambdas accordingly

  https://docs.oracle.com/javase/tutorial/reflect/member/fieldValues.html
 */
import org.junit.jupiter.api.Test;

import java.lang.annotation.Annotation;
import java.lang.annotation.Target;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

import java.util.Arrays;
import static java.lang.System.out;

public class TestProg extends TestTerm {

    public interface TermFn0 {
        public Term f0();
    }

    private class LPvar {
        TermFn0 run;
    }

    LPvar qn_(LPvar... xs) {
        String nm = f_();
        LPvar r = new LPvar();
        Term xts[] = new Term[xs.length];
        int i = 0;
        for (LPvar x : xs) {
            xts[i] = xs[i].run.f0();
            ++i;
        }
        r.run = ()->s_(nm,xts);
        return r;
    }

    Term pred(LPvar t) {
        return s_(m_(),t.run.f0());
    }

    LPvar Nothing;
    LPvar Something;
    LPvar some_struct(LPvar x) { return qn_(x); }
    LPvar other_struct(LPvar x, LPvar y)  { return qn_(x,y); }

    @Test
    public void mainTest() {

        Class tc = this.getClass();
        Field fs[] = tc.getDeclaredFields();
        String tcname = tc.getName();

        Main.println ("tcname = " + tcname);

 try {
        for (Field f : fs) {
            String field_name = f.getName();
            String field_type = f.getType().getName();

            Main.println("   field name: " + field_name);
            Main.println("   field class: " + field_type);

                if (field_type.endsWith("LPvar")) {
                    LPvar x = new LPvar();
                    x.run = ((TermFn0) (() -> v_(field_name)));
                    f.set(this, x);
                }
        }

        assert Something != null;
        assert Something.run != null;
        Main.println ("Something = " + Something.run.f0());

 } catch (IllegalAccessException x) {
        x.printStackTrace();
 } catch (IllegalArgumentException x) {
        x.printStackTrace();
 }
        Method ms[] = tc.getDeclaredMethods();
        for (Method m : ms) {

            String method_type = m.getReturnType().getName();

            if (method_type.endsWith("LPvar")) {
                String method_name = m.getName();
                Main.println ("method " + m.getName());
            }
        }

        Term result = pred(Nothing);
        Main.println("pred(Nothing): " + result);

        LPvar r = some_struct(Nothing);
        Main.println ("some_struct(Nothing) = " + r.run.f0());
        r = some_struct(some_struct(Something));
        Main.println ("some_struct(some_struct(Something)) = " + r.run.f0());
        r = other_struct(Something,Nothing);
        Main.println ("other_struct(Something,Nothing) = " + r.run.f0());
    }
}