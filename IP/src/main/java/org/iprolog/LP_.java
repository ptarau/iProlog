package org.iprolog;

public class LP_ {
    TermFn run;

    // stupid sugar:
    LP_ is_ = this;
    LP_ and_ = this;

    LP_() { run = null; }
    LP_(TermFn r) { run = r; }
}
