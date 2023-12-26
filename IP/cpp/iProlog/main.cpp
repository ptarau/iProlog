/*
 * iProlog/C++
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include <iostream>
#include <chrono>
#include <string>
#ifdef CPP17
// #include <filesystem>
#endif

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
#ifdef CPP17
    std::filesystem::path cwd = std::filesystem::current_path();
    return cwd.string();
#else
     // return "C:/Users/Michael Turner/projects/helloworld";
    return "C:/Users/Michael Turner/Documents/Github/hhprolog/cpp/";
#endif
}

namespace iProlog {

template<class P> class moo_pointer {
    private: static int a[100000];
    public:  P i;
    int size() { return sizeof(a); }
};

const int HL = 500;
const int NJ = 5000000;
int p_count = 0;
const int count_limit = HL/2;

inline long int random_bit() {
    auto p1 = std::chrono::system_clock::now();
    auto f = std::chrono::duration_cast<std::chrono::hours>(
                    p1.time_since_epoch()).count();
    return f & 1;
}

void p_do_it (cell *p) {
    p_count = count_limit;
    if (random_bit())
        --p;
}

inline void p_fudge (cell *p) {
    if (!--p_count)
        p_do_it(p);
}

void i_do_it(int &i) {
    p_count = count_limit;                   
    if (random_bit())
        --i;
}

inline void fudge (int &i) {
    if (!--p_count)
        i_do_it(i);
}


// with -O2 flag and -funroll-loops, all about the same except that
//    "vector moo_pointer indexing on C-style array"
// is about 25% faster.
// "-funroll-loops" slowed the engine down about 10% however.
//
void moo_bench() {

    static cell h[HL];
    int i;

    cout << "direct indexing on C-style array" << endl;

            { using namespace chrono;
            auto b = steady_clock::now();
   
    for (int j = 0; j < NJ; ++ j)
        for (i = HL-2; i > 0; --i) {
            fudge(i);
            h[i] = cell(j+1);
        }
            auto e = steady_clock::now();
            cout << "done in " << duration_cast<milliseconds>(e - b).count() << " ms" << endl;
        }

    moo_pointer<int> mp;
    vector<cell> H = vector<cell>(HL);

    cout << "moo_pointer indexing on C-style array" << endl;
            { using namespace chrono;
            auto b = steady_clock::now();

    for (int j = 0; j < NJ; ++ j)
        for (mp.i = HL-2; mp.i > 0; --mp.i) {
            fudge(mp.i);
            h[mp.i] = cell(j+1);
        }
            auto e = steady_clock::now();
            cout << "done in " << duration_cast<milliseconds>(e - b).count() << endl;
        }

    cout << "vector moo_pointer with dir indexing on vector" << endl;
            { using namespace chrono;
            auto b = steady_clock::now();


    for (int j = 0; j < NJ; ++ j)
        for (i = HL-2; i > 0; --i) {
            fudge(i);
            H[i] = cell(j+1);
        }

            auto e = steady_clock::now();
            cout << "done in " << duration_cast<milliseconds>(e - b).count() << endl;
        }

        cout << "vector moo_pointer with moo_pointer indexing" << endl;
            { using namespace chrono;
            auto b = steady_clock::now();
        
    for (int j = 0; j < NJ; ++ j)
        for (mp.i = HL-2; mp.i > 0; --mp.i) {
            fudge(mp.i);
            H[mp.i] = cell(j+1);
        }
            auto e = steady_clock::now();
            cout << "done in " << duration_cast<milliseconds>(e - b).count() << endl;
        }

    cout << "machine pointer indexing on C-style array" << endl;

            { using namespace chrono;
            auto b = steady_clock::now();
     
    for (int j = 0; j < NJ; ++ j) {
        cell *p = &h[1];
        for (int i = HL-1; i > 0; i--) {
            p_fudge(p);
            *p = cell(j+1);
        }
    }
            auto e = steady_clock::now();
            cout << "done in " << duration_cast<milliseconds>(e - b).count() << endl;
        }

    cout << "moo_bench done, with h[5] = " << h[5].as_int() << endl;
        cout << "moo_bench done, with H[5] = " << H[5].as_int() << endl;
}

void show (cstr h, int i) {
        cout << h << i << " (oct)" << std::oct << i<< endl;
}

void test_tagging() {
    int n = -1;
    assert (n >> 1 == -1);
// Need to be sure that V_ and U_ < R_,
// and the largest possible value of R_ < lowest possible value
// of the lowest type of cell.
//
    cout << "n_ref_bits=" << cell::n_ref_bits << endl;

    show ("unshifted_tag_mask=", cell::unshifted_tag_mask);
    assert (V_ < C_ && V_ < N_ && V_ < A_);
    assert (U_ < C_ && U_ < N_ && U_ < A_);
    assert (R_ < C_ && R_ < N_ && R_ < A_);

    cout << "ref_mask=" << cell::ref_mask << " (oct)" << std::oct << cell::ref_mask<< endl;
    cout << "tag_mask=" << cell::tag_mask << " (oct)" << std::oct << cell::tag_mask << endl;

    cell r = cell::tag(cell::R_,1);
    cout << "r=" << r.as_int() << " (oct)" << std::oct << r.as_int() << endl;
    size_t dtr = cell::detag(r);
    int tor = cell::tagOf(r);
    cout << "detag(r)=" << dtr << " (oct)" << std::oct << dtr << endl;
    cout << "tagOf(r)=" << tor << " (oct)" << std::oct << tor << endl;

    assert (!isVAR(r));
    assert (!isConst(r));
    assert (isReloc(r));

    cell rx = cell::tag(cell::R_,0);
    cell ry = cell::tag(cell::R_,1);
    // Int rz = tag(R_,-1);
    assert (ry > rx);
    assert (hi_order_tag ? rx < 0 : rx > 0);
    // assert (use_sign_bit ? ry < rz : ry > rz);

    int max_unsigned_ref = (1 << cell::n_ref_bits)-1;

    cout << "max_unsigned_ref=" << max_unsigned_ref << endl;

assert (C_ > R_);
assert (C_ < A_ && C_ < N_);

    cell rmax = cell::tag(cell::R_,max_unsigned_ref);
    cout << "rmax=" << rmax.as_int() << " (oct)" << std::oct  << rmax.as_int() << endl;

    cell cmax = cell::tag(cell::C_,max_unsigned_ref);
    // assert (cmax > rmax);
    cout << "rx=" << rx.as_int() << " (oct)" << std::oct << rx.as_int() << endl;
    cout << "cmax=" << cmax.as_int() << " (oct)" << std::oct << cmax.as_int() << endl;
    // assert (cmax > rx); // fails with use_sign_bit because ...

    int val =7;
    const int the_tag = cell::A_;
    cout << "test_tagging: tag_mask = " << std::oct << cell::tag_mask << endl;
    cell i = cell::tag(the_tag,val);
    size_t w = cell::detag (i);
    int t = cell::tagOf(i);
    cout << "In test_tagging, w=" << std::oct << w << endl;
    assert (t == the_tag);
    assert (w == val);
    cout << std::dec << endl;
    cell bad = cell::tag(cell::BAD,val);
    assert (!isRef(bad));
    assert (!isVAR(bad));
    assert (!isConst(bad));
    assert (!isReloc(bad));

    cell a = cell::tag(cell::A_,0);
    assert (!isVAR(a));
    assert (isArgOffset(a));
}
}

int main(int argc, char *argv[])
{
	cout << "...starting execution of " << argv[0] << endl;
    // Tag tests:
#if 0
    iProlog::test_tagging();
#endif
    // Indexing benchmarks:
    // iProlog::moo_bench();

    string where_i_am = current_working_directory();
    string test_directory = where_i_am + "/../test/";
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

        // assume SWI-Prolog already takes care of .pl => .pl.nl


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

        cout << "sizeof(char *)=" << sizeof(char*) << endl;
        // cout << "sizeof(short)=" << sizeof(short) << endl;
        // cout << "1 << 31 = " << ((int)1 << 31) << endl;
        // cout << "sizeof(moo_pointer<int>)=" << sizeof(hhprolog::moo_pointer<int>) << endl;
        // moo_bench();

        cout << "sizeof(vector<int>)=" << sizeof(vector<int>) << endl;

        delete p;
    }
    catch(exception &e) {
        cout << e.what() << endl;
    }
    return 0;
}

