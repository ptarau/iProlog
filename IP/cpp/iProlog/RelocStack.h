#pragma once

// WORK IN PROGRESS

/* Defines a type-parameterized, addressable, relocatable stack with elements of type T,
 * with optional range-checks. A replacement for STL vector.
 * 
 * Purposes:
 * - save space -- the STL vector header is pretty "fat", e.g. 24 bytes
 *				on my 64-bit machine. This one is 8 bytes on the same machine.
 *  			iProlog uses a lot of small arrays, so this matters.
 * 
 * - save time -- getting the C++ compiler to just fetch and use a pointer,
 *				rather than passing a whole struct around during parameter passing and
 *				assignment. The array can be bounds-checked in debug configs, but
 *				that overhead can be dispensed with, in release binaries.
 *				STL vectors also have one level of indirection, so a fetch
 *			    is saved. Space can also be time when caching is considered.
 * 
 * - flexibility -- C++ STL vector is very general. Here, however, we can
 *				allow that array element sizes can be expressed in 8 bits,
 *				and the number of elements in 16 bits. Other configurations
 *				could be specified with typedefs for header member types.
 *				Speed-for-space trades become possible. For example,
 *				if the elements are near enough to a power of two in size,
 *				the array index operator could do a single left shift
 *				on the index. Elements could be aligned on cache block
 *				addresses.
 * 
 * If the class could consist of only a pointer, the
 * optimizer has a better chance of treating an object (not a pointer to it)
 * as a pointer. But there are other data: the storage capacity, and the index
 * to the top of the stack. Relocatability imposes another constraint:
 * sometimes you want the data locked in place prevent relocation, other
 * times you don't care.
 *
 * The STL vector template type uses an indirect,
 * and has a fairly long header with the pointer to the actual relocatable vector data.
 * This might be solved in C style by using malloc, etc. but messily.
 * 
 */

#include "defs.h"
#include <vector>

namespace iProlog {
	using namespace std;

	template <class Ty>
	class RelocStack
#ifndef RAW_CELL_HEAP
		: vector<Ty>
#endif    
	{ /*start class body*/

#ifdef RAW_CELL_HEAP
		typedef struct {
			public:
				Ty* top;				     // fast access
				uint_fast16_t cap;		     // checked often
				uint_least16_t elt_size;     // less often
				uint_least8_t max_hdr_size;  // for testing empty
				bool lock;					 // reloc safety catch
		} header;

		Ty* data_;

	public:
		RelocStack<Ty>(unsigned short min_no) {
			header hd;

			hd.elt_size = (uint_least16_t)sizeof(Ty);
			hd.lock = false;
			hd.cap = min_no;
			//
			// Worst case size:
			// "Passing a size which is not an integral multiple of alignment
			//  or an alignment which is not valid or not supported by the implementation
			//  causes the function to fail and return a null pointer ..."
			// https://en.cppreference.com/w/c/memory/aligned_alloc
			// 

			size_t hdr_align = alignment_of(header);
			size_t hdr_size = sizeof(header);
			size_t Ty_align = alignment_of(Ty);
			size_t requested = sizeof(Ty) * min_no;
			//
			// sizeof(Ty) could be less or greater than sizeof(header)
			// if less, be sure to round up to an integral multiple
			// of largest alignment
			//
			hd.max_hdr_size = hdr_size + (hdr_size & (Ty_align - 1));
			// "-1" for lo-order bit mask; alignments are 2^n, n=1,2,3,4.
		size_t real_size = requested + hd.max_hdr_size;

		char* alloced = aligned_alloc(alignment_of(hdr_align), real_size);
		//
		// hd.top - like "top = -1" init in Java version.
		// DANGER -- what if alloc can start at addr 0 and sizeof(Ty) > min_space?
		// Testing for empty needs to be careful, or check aligned_alloc
		// result to see if it's < sizeof(Ty)
		//
		assert((intptr_t)alloced < hd.max_hdr_size + sizeof(Ty));
		// intptr_t "optional"
		// https://en.cppreference.com/w/cpp/types/integer

	hd.top = (Ty*)((char*)this + hd.max_hdr_size) - 1;
	//
	// set header and finally initialize pointer
	//
	*((header*)data_) = hd;
	data_ = (Ty*)((char*)alloced + hd.max_hdr_size);
}

Ty* s_data_with_lock() {
			header* bp = (header*)this;
			bp->lock = true;
			return data_;
}

void unlock_data() {
	((header*)this)->lock = false;
}
#else
		int nothing;
		void stub() {}
#endif
	};
}