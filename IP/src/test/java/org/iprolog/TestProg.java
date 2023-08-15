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

@Target(ElementType.FIELD)
@Retention(RetentionPolicy.RUNTIME)
@interface LPTv_ {
}

@interface LPTs_ {
}

@Target(ElementType.METHOD)
@Retention(RetentionPolicy.SOURCE)
@interface LPTvar_ {
}

public class TestProg extends TestTerm {
    public interface TermFunction0 {
        public Term f0();
    }

    public interface TermFunction1 {
        public Term f1(LPvar x);
    }

    public interface TermFunctionN extends TermFunction0 {
        public Term fn(Term... args);
    }

    private class LPvar {
        TermFunction0 run;
    }

    Term pred(LPvar t) {
        return s_(m_(),t.run.f0());
    }

    @LPTv_ LPvar Nothing;
    @LPTv_ LPvar Something;

    LPvar q1_(LPvar x) {
        String nm = f_();   // don't call as arg to a fn
        LPvar r = new LPvar();
        r.run = ()->s_(nm, x.run.f0());
        return r;
    };

    @LPTs_ LPvar some_struct(LPvar x) { return q1_(x); }

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
                    x.run = ((TermFunction0) (() -> v_(field_name)));
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

            if (method_type.endsWith("LPstr")) {
                String method_name = m.getName();
                Main.println ("method " + m.getName());
            }
        }

        Term result = pred(Nothing);
        Main.println("pred(Nothing): " + result);

        LPvar r = some_struct(Nothing);
        Main.println ("some_struct(Nothing) = " + r.run.f0());
    }
}