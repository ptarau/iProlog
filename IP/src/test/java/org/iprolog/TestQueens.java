package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestQueens extends TestTerm {

    LPvar QueenColumn;
    LPvar Q,Qs;
    LPvar Columns,Rows,LeftDiags,RightDiags,OtherColumns,OtherRows;
    LPvar _;

    LPvar this_queen_doesnt_fight_in(LPvar a, LPvar b, LPvar c, LPvar d) {
        return S_(a, b, c, d);
    }
    LPvar these_queens_dont_fight_on_these_lines(LPvar a, LPvar b, LPvar c, LPvar d) {
        return S_(a, b, c, d);
    }
    LPvar these_queens_can_be_in_these_places(LPvar a, LPvar b) {
        return S_(a, b);
    }
    LPvar qs(LPvar cols, LPvar rows)   { return S_(cols, rows); }
    LPvar goal(LPvar Rows)            { return S_(Rows); }

    @Test
    public void mainTest() {
        start_new_test();

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
        say_( goal(Rows)).
                if_(    qs(L_(C_("0"),C_("1"),C_("2"),C_("3")),Rows)
                );

        String[] these_answers = {
                "[1,3,0,2]",
                "[2,0,3,1]"
        };
        try_it(said, these_answers);
    }
}
