#pragma once
/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "defs.h"
#include "Inty.h"
#include <limits.h>

#include <iostream>

// C++ 20 will have machine instructions generated for rotr and rotl
// which may permit faster tag extract (thus smaller-footprint tag comparison)
// for the hi-order tag styles.

namespace iProlog {

    using namespace std;

    class cell : public Inty<cell_int> {

    public:
	static const int bitwidth = CHAR_BIT * sizeof(int);
        cell() : Inty<cell_int>(0) { }
        cell(int x) : Inty<cell_int>(x) { }
        static inline cell nonval() { return cell(-1); }; // IFFY

    // hi_order_tag=1 -> a bit slower on 32-bit, probably because gcc
    //      is generating fatter instructions to accommodate
    //      comparisons to constants that require more bits
    //      to express.
    //      Eventually try on 16-bit arch
    static const int use_sign_bit = USE_SIGN_BIT;

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

    // tag(BAD,0) used as "null pointer value" in prog.cpp,
    // and as "null termination" in index vectors
    static inline cell null()    { return tag(BAD,0); }

    // abs_var is "abstract variable" used in index matching
    static inline cell abstr_var() { return tag(V_, 0); }

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

    inline int arg() const {
        if (use_sign_bit)
            return (as_int() & ref_mask);
        else
            return as_int() >> ref_shift;
    }

    inline int s_tag() const { return as_int() & tag_mask; }
 
    inline bool is_var() const { int t = s_tag(); return t == U_ || t == V_;  }
    inline bool is_ref() const { return s_tag() == R_;                        }

    inline bool is_reloc() const {
        if (!use_sign_bit)
            return is_var() || is_ref();
        else
            return as_int() < 0;
    }

    inline bool is_const() const          { return s_tag() == C_; }
    inline bool is_offset() const     { return s_tag() == A_; }

    // The offset b will be shifted according to the tag architecture
    // chosen. When copies are forward in the cell heap, b > 0.
    //
    inline cell relocated_by(cell b) const {
        if (!is_reloc())    return *this;
        if (use_sign_bit)   return as_int() + (b.as_int() & ref_mask);
        else                return as_int() + b.as_int();
    }

    static inline cell argOffset(size_t o) { return tag(A_, o);  }
    static inline cell reference(size_t r) { return tag(R_, r);  }

    // note that this can be in-place and overlapping
    static inline void cp_cells(cell b, const cell *srcp, cell *dstp, int count) {
#       define STEP *dstp++ =  (*srcp++).relocated_by(b)
            while (count >= 4) { STEP; STEP; STEP; STEP; count -= 4; }
            switch (count) {
                case 3: STEP; case 2: STEP; case 1: STEP; case 0: ;
                }
#       undef STEP
    }

    // Need to avoid string lib in small memory

    static string tagSym(int t) {
        if (t == V_) return "V";
        if (t == U_) return "U";
        if (t == R_) return "R";
        if (t == C_) return "C";
        if (t == N_) return "N";
        if (t == A_) return "A";
        if (t == BAD) return "!";
        return "?";
    }

    string show() {
        string s = tagSym(s_tag());
        s += ":";
        s += to_string(arg());
        return s;
    }

  };

} /* end namespace*/
