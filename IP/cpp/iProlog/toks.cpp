/*
 * hhprolog: Hitchhiker Prolog
 *
 * Version: 1.0.0
 * License: MIT
 *
 * Copyright (c) 2018,2019 Carlo Capelli
 */

#include <regex>
#include <array>
#include <cstring>
#include <vector>
#ifdef DUMP_TOKS
#include <iostream>
#endif
#include "defs.h"
#include "cell.h" // just to get n_tag_bits?
#include "toks.h"

namespace iProlog {
    Toks::Toks(string t, string s, int n) : t(t), s(s), n(n) {}

// tokens as regex specification
const string
    SPACE = "\\s+",
    ATOM  = "[a-z]\\w*",
    VAR   = "[A-Z_]\\w*",
    NUM   = "-?\\d+",
    DOT   = "\\.";

// atom keywords
const string
    IF    = "if",
    AND   = "and",
    HOLDS = "holds",
  //NIL   = "nil",
    LISTS = "lists",
    IS    = "is";  // ?

vector<Toks> Toks::makeToks(string s) {
    static string E = "("+SPACE+")|("+ATOM+")|("+VAR+")|("+NUM+")|("+DOT+")";
    auto e = regex(E);
    auto token = [](smatch r) {
        // cout << "&r=" << (void*)&r << endl;
        if (!r.empty()) {
            auto tkAtom = [](string s) {
                static const array<string, 5> kws = {IF, AND, HOLDS, /*NIL,*/ LISTS, IS};
                auto p = find(kws.cbegin(), kws.cend(), s);
                Toks r = { p == kws.cend() ? ATOM : s, s, 0 };
                return r;
            };
            if (r[1].matched) return Toks{SPACE, r[0], 0};
            if (r[2].matched) return tkAtom(r.str());
            if (r[3].matched) return Toks{VAR, r[0], 0};
            if (r[4].matched) return Toks{NUM, r[0], stoi(r[0])};
            if (r[5].matched) return Toks{DOT, r[0], 0};
        }
        throw runtime_error("no match");
    };
    vector<Toks> tokens;

    sregex_iterator f(s.cbegin(), s.cend(), e), l = sregex_iterator();
    while (f != l) {
        auto r = token(*f++);
        if (r.t != SPACE) {
            tokens.push_back(r);
            /*
            cout << "r.s.c_str()=" << (void*) r.s.c_str()
                 << "tokens.back().s.c_str()" << (void*)(tokens.back().s.c_str()) << endl;
            */
        }
    }

    return tokens;
}

Tsss Toks::toSentences(string s) {
    Tsss Wsss;
    Tss Wss;
    Ts Ws;
    for (auto t : Toks::makeToks(s)) {
        if (t.t == DOT) {
            Wss.push_back(Ws);
            Wsss.push_back(Wss);
            Wss.clear();
            Ws.clear();
            continue;
        }
        if (t.t == IF) {
            Wss.push_back(Ws);
            Ws.clear();
            continue;
        }
        if (t.t == AND) {
            Wss.push_back(Ws);
            Ws.clear();
            continue;
        }
        if (t.t == HOLDS) {
            Ws[0] = "h:" + Ws[0].substr(2);
            continue;
        }
        if (t.t == LISTS) {
            Ws[0] = "l:" + Ws[0].substr(2);
            continue;
        }
        if (t.t == IS) {
            Ws[0] = "f:" + Ws[0].substr(2);
            continue;
        }
        if (t.t == VAR) {
            Ws.push_back("v:" + t.s);
            continue;
        }
        if (t.t == NUM) {
            Ws.push_back((t.n < (1 << (bitwidth- cell::n_tag_bits)) ? "n:" : "c:") + t.s);
            continue;
        }
        if (t.t == ATOM) { // || t.t == NIL) {
            Ws.push_back("c:" + t.s);
            continue;
        }
        throw runtime_error("unknown token:" + t.t);
    }
#ifdef DUMP_TOKS
    cout << "--- Wsss -----------------" << endl;
    for (auto Wss : Wsss)
      for (auto Ws : Wss)
        for (auto s: Ws)
           cout << s << endl;
    cout << "--- returning Wsss -----------------" << endl;
#endif
    return Wsss;
}

Tss Toks::maybeExpand(Ts Ws) {
    auto W = Ws[0];
    if (W.size() < 2 || "l:" != W.substr(0, 2))
        return Tss();
    int l = int(Ws.size());
    Tss Rss;
    auto V = W.substr(2);
    for (int i = 1; i < l; i++) {
        string Vi = 1 == i ? V : V + "__" + (int) (i - 1);
        string Vii = V + "__" + i;
        Ts Rs = {
                "h:" + Vi,
                "c:list",
                Ws[size_t(i)], i == l - 1 ?
                                    "c:nil"
                                :   "v:" + Vii
                };
        Rss.push_back(Rs);
    }
    return Rss;
}

Tss Toks::mapExpand(Tss Wss) {
    Tss Rss;
    for (auto Ws: Wss) {
        auto Hss = maybeExpand(Ws);
        if (Hss.empty())
            Rss.push_back(Ws);
        else
            for (auto X: Hss)
                Rss.push_back(X);
    }
    return Rss;
}

#if 0
Tss Toks::vcreate(size_t l) {
    // return Toks::Tss(l);
}
#endif

} // end namespace
