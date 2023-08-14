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
import java.util.Arrays;
import static java.lang.System.out;

@Target(ElementType.FIELD)
@Retention(RetentionPolicy.RUNTIME)
@interface LPTv {
}

public class TestProg extends TestTerm {

    public interface TermFunction {
        public Term f();
    }
    Term pred(LPvar t) {
        return s_(m_(),t.run.f());
    }

    private class LPvar {
        TermFunction run;
    }

    @LPTv LPvar Nothing;
    @LPTv LPvar Something;

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
                    x.run = (() -> v_(field_name));
                    f.set(this, x);
                }
        }

        assert Something != null;
        assert Something.run != null;
        Main.println ("Something = " + Something.run.f());

 } catch (IllegalAccessException x) {
        x.printStackTrace();
 } catch (IllegalArgumentException x) {
        x.printStackTrace();
 }

        Term result = pred(Nothing);
        Main.println("pred(Nothing): " + result);
    }
}