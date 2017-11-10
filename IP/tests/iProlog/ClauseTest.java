package iProlog;

import static org.junit.Assert.*;

import org.junit.Test;
import static org.hamcrest.CoreMatchers.instanceOf;


public class ClauseTest {

	@Test
	public void test() {
		Clause c = new Clause(0, null, 0, 0, null);
		assertThat(c, instanceOf(Clause.class));
	}

}
