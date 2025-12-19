/*
 * iProlog/C++
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include <iostream>
#include <chrono>
#include <string>
#if 0
#include <filesystem>
#endif
#include <fstream>
#include <sstream>

#include "prog.h"
#include "toks.h"

#include "Inty.h"
#include "RelocStack.h"
#include "sym_tab.h"


std::string file2string(std::string path) {
    std::ifstream f(path);
    if (!f.good())
        throw std::invalid_argument(path + " not found");
    std::stringstream s;
    s << f.rdbuf();
    return s.str();
}


using namespace std;
using namespace chrono;

std::string current_working_directory()
{
#if 0
   std::filesystem::__cxx11::path p = std::filesystem::current_path();
   cout << "p = " << p << endl;
#endif

#ifdef CPP17
    std::filesystem::path cwd = std::filesystem::current_path();
    return cwd.string();
#else
    return "C:/Users/Michael Turner/Documents/Github/iProlog/IP/";
#endif
}

namespace iProlog {


void show(cstr h, int i) {
    cout << h << i << " (oct)" << std::oct << i << endl;
}

    CellStack heap;
    sym_tab sym;

string showCell(cell w) {
    int t = w.s_tag();
    int val = w.arg();
    string s = "";

    switch (t) {
        case cell::V_:    s = cstr("v:") + val;        break;
        case cell::U_:    s = cstr("u:") + val;        break;
        case cell::N_:    s = cstr("n:") + val;        break;
        case cell::C_:    s = cstr("c:") + val; break;      //had getSym(...)
        case cell::R_:    s = cstr("r:") + val;        break;
        case cell::A_:    s = cstr("a:") + val;        break;
        default:    s = cstr("*BAD*=") + w.as_int();
    }
    return s;
}

/*static*/ vector<int> &
put_ref(        const string arg,
                unordered_map<string, vector<int>> &refs,
                int clause_pos) {
    vector<int>& Is = refs[arg];
    if (Is.empty()) {
        Is = vector<int>();
        refs[arg] = Is;
    }
    Is.push_back(clause_pos);
    return Is;
}

/*
 * "Encodes string constants into symbols while leaving
 * other data types untouched." [Engine.java]
 */
cell encode(int t, const string s) {
    size_t w;
    try {
        w = stoi(s);
    }
    catch (const std::invalid_argument& e) {
        if (t == cell::C_)
            w = int(sym.addSym(s));
        else {
            cstr err = string("bad number form in encode=") + t + ":" + s + ", [" + e.what() + "]";
            throw logic_error(err);
        }
    }
    // cout << " encode: w=" << w << " returning " << cell::tag(t, w).as_int() << endl;
    return cell::tag(t, w);
}

/**
  * "Places a clause built by the Toks reader on the heap." [Engine.java]
  */
Clause putClause(vector<cell> cells, vector<cell> &vskel, int neck) {
    int base = heap.getTop()+1;
    cell b = cell::tag(cell::V_, base);
        // ... because b is used later in '+' ops that would otherwise mangle tags.
    int len = int(cells.size());
    CellStack::pushCells(heap, b, 0, len, cells);

    cell* src = vskel.data();
#ifdef RAW_SKEL
    cell* dst = (cell*)malloc(vskel.size() * sizeof(cell));
#else
    cell* dst = vskel.data();
#endif

    // for goals list being C++ vector, in-place reloc
    if (has_raw_cell_heap)
        cell::cp_cells(b, src, dst, (int)vskel.size());
    else
        for (size_t i = 0; i < vskel.size(); i++)
            dst[i] = dst[i].relocated_by(b);

#ifdef RAW_SKEL
    Clause rc = Clause(len, dst, vskel.size(), base, neck);
#else
    Clause rc = Clause(len, vskel, vskel.size(), base, neck);
#endif
    return rc;
}

    void linker(unordered_map<string,vector<int>> refs,
                        vector<cell> &cells,
                        vector<cell> &goal_refs,  // R_ (ref) cells only
                        vector<Clause> &compiled_clauses) {
        // Java:
        // final Iterator<IntStack> K = refs.values().iterator();
        // while (K.hasNext())
        
        for (auto kIs = refs.begin(); kIs != refs.end(); ++kIs) {
            vector<int> Is = kIs->second;
            if (Is.size() == 0)
                continue;
            assert(goal_refs.size() > 0);

            // "finding the A among refs" [Engine.java]
            bool found = false;
            size_t leader = -1;
            for (size_t j = 0; j < Is.size(); ++j)
                if (/*cell::isArgOffset(cells[j])*/
                    cells[Is[j]].s_tag() == cell::A_) {
                    leader = Is[j];
                    found = true;
                    break;
                }

            if (!found) {
                // "for vars, first V others U" [Engine.java]
                leader = Is[0];
                for (size_t i = 0; i < Is.size(); ++i)
                    if (Is[i] == leader)
                        cells[Is[i]] = cell::tag(cell::V_, Is[i]);
                    else
                        cells[Is[i]] = cell::tag(cell::U_, leader);
            }
            else {
                for (size_t i = 0; i < Is.size(); ++i) {
                    if (Is[i] == leader)
                        continue;
                    cells[Is[i]] = cell::tag(cell::R_, leader);
                }
            }
        }

        int neck;
        if (1 == goal_refs.size())
            neck = int(cells.size());
        else
            neck = goal_refs[1L].arg();

        Clause C = putClause(cells, goal_refs, neck); // safe to pass all?

        compiled_clauses.push_back(C);
    }


vector<Clause> dload(const cstr s) {
    vector<vector<vector<string>>> clause_asm_list = Toks::toSentences(s);
    vector<Clause> compiled_clauses;

    for (vector<vector<string>> unexpanded_clause : clause_asm_list) {
        // map<string, IntStack> refs;
        unordered_map<string, vector<int>> refs = unordered_map<string,vector<int>>();
        vector<cell> cells;
        vector<cell> goal_refs;
        int k = 0;
        for (vector<string> clause_asm : Toks::mapExpand(unexpanded_clause)) {

            size_t line_len = clause_asm.size();

            goal_refs.push_back(cell::reference(k++));
            cells.push_back(cell::argOffset(line_len));
            for (string cell_asm_code : clause_asm) {
                if (1 == cell_asm_code.length())
                    cell_asm_code = "c:" + cell_asm_code;
                string arg = cell_asm_code.substr(2);

                switch (cell_asm_code[0]) {
                case 'c':   cells.push_back(encode(cell::C_, arg));     k++; break;
                case 'n':   cells.push_back(encode(cell::N_, arg));     k++; break;
                case 'v':   put_ref(arg, refs, k);
                            cells.push_back(cell::tag(cell::BAD, k));   k++; break;
                case 'h':   refs[arg].push_back(k-1);
                            assert(k > 0);
                            cells[size_t(k-1)] = cell::argOffset(line_len-1);
                            goal_refs.pop_back();                               break;
                default:    throw logic_error(cstr("FORGOTTEN=") + cell_asm_code);
                }
            }
        }
        linker(refs, cells, goal_refs, compiled_clauses);
    }

    size_t clause_count = compiled_clauses.size();
    vector<Clause> all_clauses = vector<Clause>(clause_count);

    for (int i = 0; i < clause_count; i++) {
        all_clauses[i] = compiled_clauses[i];
    }

    return all_clauses;
 }

    void test_tagging() {
        int n = -1;
        assert(n >> 1 == -1);
        // Need to be sure that V_ and U_ < R_,
        // and the largest possible value of R_ < lowest possible value
        // of the lowest type of cell.
        //
        cout << "n_ref_bits=" << cell::n_ref_bits << endl;

        show("unshifted_tag_mask=", cell::unshifted_tag_mask);
        assert(cell::V_ < cell::C_ && cell::V_ < cell::N_ && cell::V_ < cell::A_);
        assert(cell::U_ < cell::C_ && cell::U_ < cell::N_ && cell::U_ < cell::A_);
        assert(cell::R_ < cell::C_ && cell::R_ < cell::N_ && cell::R_ < cell::A_);

        cout << "ref_mask=" << cell::ref_mask << " (oct)" << std::oct << cell::ref_mask << endl;
        cout << "tag_mask=" << cell::tag_mask << " (oct)" << std::oct << cell::tag_mask << endl;

        cell r = cell::tag(cell::R_, 1);
        cout << "r=" << r.as_int() << " (oct)" << std::oct << r.as_int() << endl;
        size_t dtr = r.arg();
        int tor = r.s_tag();
        cout << "arg of r)=" << dtr << " (oct)" << std::oct << dtr << endl;
        cout << "tag of r)=" << tor << " (oct)" << std::oct << tor << endl;

        assert(!r.is_var());
        assert(!r.is_const());
        assert(r.is_reloc());

        cell rx = cell::tag(cell::R_, 0);
        cell ry = cell::tag(cell::R_, 1);
        // Int rz = tag(R_,-1);
        assert(ry.as_int() > rx.as_int());
        // assert(cell::hi_order_tag ? rx.as_int() < 0 : rx.as_int() > 0);
        // assert (use_sign_bit ? ry < rz : ry > rz);

        int max_unsigned_ref = (1 << cell::n_ref_bits) - 1;

        cout << "max_unsigned_ref=" << max_unsigned_ref << endl;

        assert(cell::C_ > cell::R_);
        assert(cell::C_ < cell::A_ && cell::C_ < cell::N_);

        cell rmax = cell::tag(cell::R_, max_unsigned_ref);
        cout << "rmax=" << rmax.as_int() << " (oct)" << std::oct << rmax.as_int() << endl;

        cell cmax = cell::tag(cell::C_, max_unsigned_ref);
        // assert (cmax > rmax);
        cout << "rx=" << rx.as_int() << " (oct)" << std::oct << rx.as_int() << endl;
        cout << "cmax=" << cmax.as_int() << " (oct)" << std::oct << cmax.as_int() << endl;
        // assert (cmax > rx); // fails with use_sign_bit because ...

        int val = 7;
        const int the_tag = cell::A_;
        cout << "test_tagging: tag_mask = " << std::oct << cell::tag_mask << endl;
        cell i = cell::tag(the_tag, val);
        size_t w = i.arg();
        int t = i.s_tag();
        cout << "In test_tagging, w=" << std::oct << w << endl;
        assert(t == the_tag);
        assert(w == val);
        cout << std::dec << endl;
        cell bad = cell::tag(cell::BAD, val);
        assert(!bad.is_ref());
        assert(!bad.is_var());
        assert(!bad.is_const());
        assert(!bad.is_reloc());

        cell a = cell::tag(cell::A_, 0);
        if (a.is_var()) abort();
        if (!a.is_offset()) abort();
    }

    void testSharedCellList() {
        CL_p p;
        cell h;
        p = CellList::mk_shared();
        CL_p q;
        q = CellList::mk_shared(h);
    }

    // int * about 25%-30% faster than vector<int>
    void vec_bench()
    {
        using namespace chrono;

        const int OC = 200000000;
        const int IC = 10;

        cout << "vectors benchmark" << endl;
        {
            int x = 0;
            auto b = steady_clock::now();

            vector<int> is(IC);
            for (int i = 0; i < OC; ++i) {
                for (int j = 0; j < IC; ++j)
                    is[j] = j + x++;
                for (int j = 0; j < IC; ++j)
                    is[j] = -is[j] + x--;
            }

            cout << "after run, x = " << to_string(x) << endl;
            auto e = steady_clock::now();
            long long d = duration_cast<milliseconds>(e - b).count();
            cout << "done in " << std::dec << duration_cast<milliseconds>(e - b).count() << endl;
            cout << "or " << (double)d / 1000 << endl;
        }

        cout << "int * benchmark" << endl;
        {
            int x = 0;
            auto b = steady_clock::now();
            int* is = (int*)malloc(sizeof(int) * IC);
            for (int i = 0; i < OC; ++i) {
                for (int j = 0; j < IC; ++j)
                    is[j] = j + x++;
                for (int j = 0; j < IC; ++j)
                    is[j] = -is[j]+ x--;
            }

            cout << "after run, x = " << to_string(x) << endl;
            auto e = steady_clock::now();
            long long d = duration_cast<milliseconds>(e - b).count();
            cout << "done in " << std::dec << duration_cast<milliseconds>(e - b).count() << endl;
            cout << "or " << (double)d / 1000 << endl;
        }
    }

    void int_bench()
    {
        using namespace chrono;
        auto b = steady_clock::now();
        cout << "before run" << endl;
        //////////////
        cout << "after run" << endl;
        auto e = steady_clock::now();
        long long d = duration_cast<milliseconds>(e - b).count();
        cout << "done in " << std::dec << duration_cast<milliseconds>(e - b).count() << endl;
        cout << "or " << (double)d / 1000 << endl;
    }

    void test_IntSet() {
        IntSet im;
        cout << endl << "************** test_IntSet() ***********************************" << endl;

        for (int i = 1; i < 9; ++i) {
            // cout << "about to call add_key";
            im.add_key(i);
            // cout << "about to call put";
            im.put(i, i + 1);
            // cout << " about to call get";
            int v = im.get(i);
            // cout << "test_IntSet i = " << to_string(i) << endl;
            if (!(v == i + 1)) abort();
        }

        cout << "************** END test_IntSet() ***********************************" << endl;
    }

    void test_IMap() {
#define TR if(1)
        cout << "************** test_IMap() ***********************************" << endl;
        IMap x;
        cell c = cell::tag(cell::V_, 3);
        ClauseNumber cls_no = 7;

        TR cout << "===== calling x.put(cellbox->i=" << c.as_int() << ", cls_no=" << cls_no.as_int() << endl;

        x.put(cls_no, c);

        cls_no_set* the_intset = x.get_cls_no_set(c);
        TR cout << " got intset, length=" << the_intset->length() << endl;
        TR cout << "the_intset: " << the_intset->show() << endl;

        TR cout << "Imap x is now " << x.show() << endl;

        assert(the_intset->contains(cls_no.as_int()));

        cout << endl << "************** END test_IMap() ***********************************" << endl;
#undef TR
    }

    int do_with(int argc, char* argv[])
    {
#if 0
        vec_bench();
        exit(0);
#endif
        iProlog::test_IntSet();
        // exit(0);

        cout << "...starting execution of " << argv[0] << endl;
        test_IMap();
#if 0
        test_tagging();
        testSharedCellList();
#endif
        string where_i_am = current_working_directory();
        string test_directory = where_i_am + "/progs/";
        cout << "... in " << where_i_am << endl;

        if (argc == 1) {
            cerr << "Must supply name of a program in directory " << test_directory << endl;
            exit(-1);
        }
        try {
            string fname;
            bool print_ans;

            fname = argv[1];
            print_ans = argc == 3 ? string(argv[2]) == "true" : false;

            string pl_nl = test_directory + fname + ".nl";

            cout << "==============================================================" << endl;

	    string source = file2string(pl_nl);
	    vector<Clause> clauses = dload(source);

        index* Ip = nullptr;

        if (indexing) {
            Ip = new index(heap, clauses);
            cout << Ip->show() << endl;
        }

	    Prog *p = new Prog(heap,clauses,sym,Ip); // any index-building done there

        p->ppCode();

        { using namespace chrono;
        auto b = steady_clock::now();
        cout << "before run" << endl;
        p->run(print_ans);
        cout << "after run" << endl;
        auto e = steady_clock::now();
        long long d = duration_cast<milliseconds>(e - b).count();
        cout << "done in " << std::dec << duration_cast<milliseconds>(e - b).count() << endl;
        cout << "or " << (double)d / 1000 << endl;
        }

            cout << p->stats() << endl;

cout << "---------------------------------------" << endl;

            delete p;
        }
        catch (exception& e) {
            cout << e.what() << endl;
        }

        return 0;
    }

    Engine* ep = nullptr;
    void set_engine(void* e) { ep = (Engine *) e;  }

    void checkit_() {
#define TR if(0)
        if (!integrity_check) return;
        cout << "checkit..." << endl;
        if (indexing) {
            assert(ep->Ip != nullptr);
            for (int i = 0; i < ep->clauses.size(); ++i) {
                TRY{
                    cell hd = ep->clauses.at(i).skel[0];
                    assert(ep->clauses[i].base >= 0);
                    assert(ep->clauses[i].base < ep->heap.size());
                } CATCH("blowing up in check it")
            }
        }
#undef TR
    }

} // end iProlog

int main(int argc, char* argv[]) {
	return iProlog::do_with(argc, argv);
}

