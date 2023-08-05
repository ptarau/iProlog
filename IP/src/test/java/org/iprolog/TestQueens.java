package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestQueens extends TestTerm {

    Term QueenColumn()  {  return v_("QueenColumn");    }
    Term Q()            {  return v_(m_());                }
    Term Qs()           {  return v_(m_());                }
    Term Columns()      {  return v_(m_());                }
    Term Rows()         {  return v_(m_());                }
    Term LeftDiags()    {  return v_(m_());                }
    Term RightDiags()   {  return v_(m_());                }
    Term OtherColumns() {  return v_(m_());                }
    Term OtherRows()    {  return v_(m_());                }

    Term this_queen_doesnt_fight_in(Term a, Term b, Term c, Term d) {
        return s_(m_(), a, b, c, d);
    }
    Term these_queens_dont_fight_on_these_lines(Term a, Term b, Term c, Term d) {
        return s_(m_(), a, b, c, d);
    }
    Term these_queens_can_be_in_these_places(Term a, Term b) {
        return s_(m_(), a, b);
    }
    Term qs(Term cols, Term rows)   { return s_(m_(), cols, rows); }
    Term goal(Term Rows)            { return s_(m_(), Rows); }

    @Test
    public void mainTest() {
        start_new_test();

        say_( this_queen_doesnt_fight_in(
                QueenColumn(),
                p_(QueenColumn(), _()),
                p_(QueenColumn(), _()),
                p_(QueenColumn(), _())  )
        );
        say_( this_queen_doesnt_fight_in(
                Q(), p_(_(),Rows()), p_(_(),LeftDiags()), p_(_(),RightDiags()) )
        ).if_(this_queen_doesnt_fight_in(
                Q(), Rows(), LeftDiags(), RightDiags() )
        );
        say_( these_queens_dont_fight_on_these_lines(
                l_(), _(), _(), _() )
        );
        say_( these_queens_dont_fight_on_these_lines(
                p_(QueenColumn(), Qs()),
                Rows(),
                LeftDiags(),
                p_(_(), RightDiags()) )
        ).
                if_(    these_queens_dont_fight_on_these_lines(
                        Qs(), Rows(), p_(_(), LeftDiags()), RightDiags() ),
                        this_queen_doesnt_fight_in(
                                QueenColumn(), Rows(), LeftDiags(), RightDiags() )
                );
        say_( these_queens_can_be_in_these_places(l_(), l_()) );
        say_( these_queens_can_be_in_these_places(
                p_(_(),OtherColumns()),
                p_(_(),OtherRows()) )
        ).if_(  these_queens_can_be_in_these_places(
                OtherColumns(),
                OtherRows())
        );
        say_( qs(Columns(), Rows())).
                if_(    these_queens_can_be_in_these_places(
                        Columns(),
                        Rows() ),
                        these_queens_dont_fight_on_these_lines(
                                Columns(),Rows(),_(),_() )
                );
        say_( goal(Rows())).
                if_(    qs(l_(c0(),c1(),c2(),c3()),Rows())
                );

        String[] these_answers = {
                "[1,3,0,2]",
                "[2,0,3,1]"
        };
        try_it(said, these_answers);

    }
}
