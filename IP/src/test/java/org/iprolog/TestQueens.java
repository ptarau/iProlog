package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestQueens extends TestTerm {

    LP_ QueenColumn;
    LP_ Q,Qs;
    LP_ Columns,Rows,LeftDiags,RightDiags,OtherColumns,OtherRows;

    TestQueens() {
        say_( this_queen_doesnt_fight_in(
                QueenColumn,
                P_(QueenColumn, _),
                P_(QueenColumn, _),
                P_(QueenColumn, _)  )
        );
        say_( this_queen_doesnt_fight_in(
                Q, P_(_,Rows), P_(_,LeftDiags), P_(_,RightDiags) )
        ).if_(this_queen_doesnt_fight_in(
                Q, Rows, LeftDiags, RightDiags )
        );
        say_( these_queens_dont_fight_on_these_lines(
                L_(), _, _, _ )
        );
        say_( these_queens_dont_fight_on_these_lines(
                P_(QueenColumn, Qs),
                Rows,
                LeftDiags,
                P_(_, RightDiags) )
        ).
                if_(    these_queens_dont_fight_on_these_lines(
                        Qs, Rows, P_(_, LeftDiags), RightDiags ),
                        this_queen_doesnt_fight_in(
                                QueenColumn, Rows, LeftDiags, RightDiags )
                );
        say_( these_queens_can_be_in_these_places(L_(), L_()) );
        say_( these_queens_can_be_in_these_places(
                P_(_,OtherColumns),
                P_(_,OtherRows) )
        ).if_(  these_queens_can_be_in_these_places(
                OtherColumns,
                OtherRows)
        );
        say_( qs(Columns, Rows)).
                if_(    these_queens_can_be_in_these_places(
                        Columns,
                        Rows ),
                        these_queens_dont_fight_on_these_lines(
                                Columns,Rows,_,_ )
                );
        say_( good_(Rows)).
                if_(    qs(L_(C_("0"),C_("1"),C_("2"),C_("3")),Rows)
                );
    }

    LP_ this_queen_doesnt_fight_in(LP_ a, LP_ b, LP_ c, LP_ d) {
        return S_(a, b, c, d);
    }
    LP_ these_queens_dont_fight_on_these_lines(LP_ a, LP_ b, LP_ c, LP_ d) {
        return S_(a, b, c, d);
    }
    LP_ these_queens_can_be_in_these_places(LP_ a, LP_ b) {
        return S_(a, b);
    }
    LP_ qs(LP_ cols, LP_ rows) {
        return S_(cols, rows);
    }

    @Test
    public void mainTest() {
        String[] these_answers = {
                "[1,3,0,2]",
                "[2,0,3,1]"
        };
        try_it(said, these_answers);
    }
}
