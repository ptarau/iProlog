#pragma once
/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "defs.h"
#include <limits.h>
#include <iostream>

// C++ 20 will have machine instructions generated for rotr and rotl
// which may permit faster tag extract (thus smaller-footprint tag compare)
// for the hi-order tag styles.

namespace iProlog {

    using namespace std;

    class cell {

        int v;  // it make make sense to init at BAD-tagged cell
    public:
        cell() { v = 0; }
        cell(int i) { v = i; }
        static inline cell nonval() { return cell(int(-1)); };
        inline int as_int() const { return v; }

    // hi_order_tag=1 -> a bit slower on 32-bit, probably because gcc
    //      is generating fatter instructions to accommodate
    //      comparisons to constants that require more bits
    //      to express.
    //      Eventually try on 16-bit arch
    static const int use_sign_bit = 1;

    static const int bitwidth = (sizeof(int) * 8);
    static const int n_tag_bits = 3;
    static const int n_ref_tags = 3;
    static const int n_ref_bits = bitwidth - n_tag_bits;

    // TODO: just have a shift & mask inline function?
    //       ....
    static const int nonref_base = use_sign_bit ? 0 : n_ref_tags;
    static const int signed_zeros = use_sign_bit << (bitwidth - 1);
    static const int tag_shift = use_sign_bit ? n_ref_bits : 0;
    static const int ref_shift = use_sign_bit ? 0 : n_tag_bits;
    static const int unshifted_tag_mask = (1 << n_tag_bits) - 1;
    static const int tag_mask = unshifted_tag_mask << tag_shift;
    static const int ref_mask = ~tag_mask;

  public:
    static const int V_ = (0 << tag_shift) | signed_zeros;
    static const int U_ = (1 << tag_shift) | signed_zeros;
    static const int R_ = (2 << tag_shift) | signed_zeros;
    static const int C_ = (0 + nonref_base) << tag_shift;
    static const int N_ = (1 + nonref_base) << tag_shift;
    static const int A_ = (2 + nonref_base) << tag_shift;

    static const int BAD = (3 + nonref_base) << tag_shift;

    // For x64, and maybe 32-bit CPU case:
    // By making BAD == 0, and always making sure there's a zero cell at the
    // end of a copy, there's a low-overhead sentinel. This could outperform
    // memcpy, since there must be set-up overhead for it, and the average
    // size of copies (except for inline single-cell in construction)
    // is somewhere around 5-6 cells. The copy could be
    // 
    // (1) copy one 64-bit (8-byte) long long int, then
    // (2) look at the copy count to see if more is needed.
    // (3) If so, copy another, then fall into
    // (4) unrolled-loop switch. [MT]
    // 
    // If the first cell is zero, which makes no code sense,
    // it could trap out and use following cells as arguments
    // to the trap call. This would still leave one more cell
    // tag available for other purposes that arise.

    static inline cell tag(int t, size_t w) {
        return cell((int)((w << ref_shift) | t));
    }
    static inline int detag(cell w) {
        if (use_sign_bit)
            return (w.as_int() & ref_mask);
        else
            return w.as_int() >> ref_shift;
    }
    static inline int tagOf(cell w) { return w.as_int() & tag_mask; }
    static inline bool isVAR(cell x) {
        int t = tagOf(x);
        return t == U_ || t == V_;
    }
    static inline bool isRef(cell x) { return tagOf(x) == R_; }
    static inline bool isReloc(cell x) {
        if (!use_sign_bit)
            return isVAR(x) || isRef(x);
        else
            return x.as_int() < 0;
    }
    static inline bool isConst(cell x)      { return tagOf(x) == C_;    }
    static inline bool isConstTag(int t)    { return t == C_;           }
    static inline bool isArgOffset(cell x)  { return tagOf(x) == A_;    }
    inline bool operator ==(cell x)         { return v == x.v;          }

    // inline cell operator() (int i) { return (cell) i; }

    static inline cell relocate(cell b, const cell c) {
        if (isReloc(c)) {
            if (use_sign_bit)   return c.as_int() + (b.as_int() & ref_mask);
            else                return c.as_int() + b.as_int();
        }
        return c;
    }

    static string tagSym(int t) {
        if (t == V_) return "V";
        if (t == U_) return "U";
        if (t == R_) return "R";
        if (t == C_) return "C";
        if (t == N_) return "N";
        if (t == A_) return "A";
        return "?";
    }

    static inline cell argOffset(size_t o) { return tag(A_, o);  }
    static inline cell reference(size_t r) { return tag(R_, r);  }
  };

} /* end namespace*/