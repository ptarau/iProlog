/*
 * iProlog/C++
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include <iostream>
#include <chrono>
#include <string>
#include <filesystem>

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string file2string(std::string path) {
    std::ifstream f(path);
    if (!f.good())
        throw std::invalid_argument(path + " not found");
    std::stringstream s;
    s << f.rdbuf();
    return s.str();
}

#include "Engine.h"
#include "prog.h"

using namespace std;
using namespace chrono;

std::string current_working_directory()
{
   std::filesystem::__cxx11::path p = std::filesystem::current_path();
   cout << "p = " << p << endl;

// #ifdef CPP17
    std::filesystem::path cwd = std::filesystem::current_path();
    return cwd.string();
// #else
      // return "C:/Users/Michael Turner/projects/helloworld";
//    return "C:/Users/Michael Turner/Documents/Github/iProlog/IP/";
// #endif
}

namespace iProlog {


    void show(cstr h, int i) {
        cout << h << i << " (oct)" << std::oct << i << endl;
    }

#if 0
CellStack heap;
unordered_map<string, Integer*> syms;
vector<string> slist;


t_index_vector getIndexables(cell ref) {
    int p = 1 + cell::detag(ref);
    int n = cell::detag(CellStack::getRef(heap, ref));
    t_index_vector index_vector = { -1,-1,-1 };
    for (int i = 0; i < MAXIND && i < n; i++) {
        cell c = CellStack::deref(heap, CellStack::cell_at(heap, p + i));
        index_vector[i] = CellStack::cell2index(heap,c).as_int();
    }
    return index_vector;
}

/**
 * "Places an identifier in the symbol table."
 */
Integer *addSym(string sym) {
    try { return syms.at(sym); }
    catch (const std::exception& e) {
        Integer* I = new Integer(syms.size());
        syms.insert(pair<string, Integer*>(sym, I));
        slist.push_back(sym);
        return I;
    }
}

/*static*/ vector<int> &
put_ref(string arg,
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
 * Encodes string constants into symbols while leaving
 * other data types untouched.
 */
cell encode(int t, string s) {
    size_t w;
    try {
        w = stoi(s);
    }
    catch (const std::invalid_argument& e) {
        if (t == cell::C_)
            w = int(addSym(s)->i);
        else {
            cstr err = string("bad number form in encode=") + t + ":" + s + ", [" + e.what() + "]";
            throw logic_error(err);
        }
    }
    return cell::tag(t, w);
}

/**
  * Places a clause built by the Toks reader on the heap.
  */
Clause putClause(vector<cell> cells, vector<cell> &hgs, int neck) {

    int base = heap.getTop()+1;

    cell b = cell::tag(cell::V_, base);
    // ... because b is used later in '+' ops that would otherwise mangle tags.
    int len = int(cells.size());
    CellStack::pushCells(heap, b, 0, len, cells);

    bool unroll = true;
    if (unroll)
        cell::cp_cells(b, hgs.data(), hgs.data(), (int) hgs.size());
    else
        for (size_t i = 0; i < hgs.size(); i++)
            hgs[i] = cell::relocate(b, hgs[i]);

    t_index_vector index_vector = getIndexables(hgs[0]);

    Clause rc = Clause(len, hgs, base, neck, index_vector);

    return rc;
}


    void linker(unordered_map<string,vector<int>> refs,
                        vector<cell> &cells,
                        vector<cell> &goals,
                        vector<Clause> &compiled_clauses) {

        // final Iterator<IntStack> K = refs.values().iterator();
        // while (K.hasNext())
        
        for (auto kIs = refs.begin(); kIs != refs.end(); ++kIs) {
            vector<int> Is = kIs->second;
            if (Is.size() == 0)
                continue;
            assert(goals.size() > 0);

            // "finding the A among refs" [Engine.java]
            bool found = false;
            size_t leader = -1;
            for (size_t j = 0; j < Is.size(); ++j)
                if (/*cell::isArgOffset(cells[j])*/
                    cell::tagOf(cells[Is[j]]) == cell::A_) {
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
        if (1 == goals.size())
            neck = int(cells.size());
        else
            neck = cell::detag(goals[1L]);

        Clause C = putClause(cells, goals, neck); // safe to pass all?

        int len = int(cells.size());

        compiled_clauses.push_back(C);
        }


vector<Clause> dload(cstr s) {
    vector<vector<vector<string>>> clause_asm_list = Toks::toSentences(s);
    vector<Clause> compiled_clauses;

    for (vector<vector<string>> unexpanded_clause : clause_asm_list) {
        // map<string, IntStack> refs;
        unordered_map<string, vector<int>> refs = unordered_map<string,vector<int>>();
        vector<cell> cells;
        vector<cell> goals;
        int k = 0;
        for (vector<string> clause_asm : Toks::mapExpand(unexpanded_clause)) {

            size_t line_len = clause_asm.size();

            goals.push_back(cell::reference(k++));
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
                            goals.pop_back();                               break;
                default:    throw logic_error(cstr("FORGOTTEN=") + cell_asm_code);
                }
            }
        }
        linker(refs, cells, goals, compiled_clauses);
    }

    size_t clause_count = compiled_clauses.size();
    vector<Clause> all_clauses = vector<Clause>(clause_count);

    for (int i = 0; i < clause_count; i++) {
        all_clauses[i] = compiled_clauses[i];
    }

    return all_clauses;
 }
#endif
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
        size_t dtr = cell::detag(r);
        int tor = cell::tagOf(r);
        cout << "detag(r)=" << dtr << " (oct)" << std::oct << dtr << endl;
        cout << "tagOf(r)=" << tor << " (oct)" << std::oct << tor << endl;

        assert(!cell::isVAR(r));
        assert(!cell::isConst(r));
        assert(cell::isReloc(r));

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
        size_t w = cell::detag(i);
        int t = cell::tagOf(i);
        cout << "In test_tagging, w=" << std::oct << w << endl;
        assert(t == the_tag);
        assert(w == val);
        cout << std::dec << endl;
        cell bad = cell::tag(cell::BAD, val);
        assert(!cell::isRef(bad));
        assert(!cell::isVAR(bad));
        assert(!cell::isConst(bad));
        assert(!cell::isReloc(bad));

        cell a = cell::tag(cell::A_, 0);
        assert(!cell::isVAR(a));
        assert(cell::isArgOffset(a));
    }

    void testSharedCellList() {
        shared_ptr<CellList> p;
        cell h;
        p = make_shared<CellList>();
        shared_ptr<CellList> q;
        q = make_shared<CellList>(h);
    }
} // end nameSpace

    int main(int argc, char* argv[])
    {
        cout << "...starting execution of " << argv[0] << endl;
        // Tag tests:
#if 0
        iProlog::test_tagging();
#endif
        iProlog::testSharedCellList();


        // Indexing benchmarks:
        // iProlog::moo_bench();

        string where_i_am = current_working_directory();
        string test_directory = where_i_am + "/../../progs/";
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

            // just add ".nl" on the command line
            string pl_nl = test_directory + fname + ".nl";

            cout << "==============================================================" << endl;

#if 1
            auto p = new iProlog::Prog(file2string(pl_nl));
#else
            string s = "";
            s += "f 0 .\n";
            s += "\n";
            s += "goal X\n";
            s += "if\n";
            s += " f X .\n";
            auto p = new iProlog::Prog(s);
#endif
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

            delete p;
        }
        catch (exception& e) {
            cout << e.what() << endl;
        }
        return 0;
    }


