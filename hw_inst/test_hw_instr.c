#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>

#define UNUSED __attribute__((unused))
#define ALWAYS_INLINE __attribute__((always_inline))

#define LOOP_COUNT 1000000000

#define ADDRSPACE_BITS (36)
#define POINTER_BITS    (sizeof (void*) * 8)
#define ADDRSPACE_MASK ((1ULL << ADDRSPACE_BITS) - 1)
#define OVERFLOW_MASK  (1ULL << (POINTER_BITS - 1))
#define PRESERVE_MASK (ADDRSPACE_MASK | OVERFLOW_MASK)

#define TAG_MASK (ADDRSPACE_MASK << 4)

#if defined(__x86_64__) && defined(__BMI2__)

#include "x86intrin.h"

ALWAYS_INLINE uintptr_t mask_pointer_pext_reg(uintptr_t ptr) {
    return _pext_u64(ptr, PRESERVE_MASK);
}

ALWAYS_INLINE uintptr_t mask_pointer_pext_glob(uintptr_t ptr) {
    return _pext_u64(ptr, PRESERVE_MASK);
}

ALWAYS_INLINE uintptr_t mask_pointer_bzhi(uintptr_t ptr) {
    return _bzhi_u64(ptr, ADDRSPACE_BITS);
}

ALWAYS_INLINE uintptr_t mask_pointer_upper(uintptr_t ptr) {
    return _bzhi_u64(ptr, OVERFLOW_MASK);
}

#endif /* __x86_64__ */

ALWAYS_INLINE uintptr_t mask_pointer_normally(uintptr_t ptr) {
    return ptr & ADDRSPACE_MASK;
}
ALWAYS_INLINE uintptr_t mask_pointer_upper_normally(uintptr_t ptr) {
    return ptr & OVERFLOW_MASK;
}

int main() {
    uintptr_t p1 = 0x1234567890ABCDEF;
    uintptr_t p2 = 0xFEDCBA0987654321;
    uintptr_t p3 = 0xABCDEF0123456789;
    uintptr_t p4 = 0xABCDEF0123456789;
    
    uintptr_t p1upd = mask_pointer_pext_reg(p1); //keeps 1st bit + 32 last
    uintptr_t p2upd = mask_pointer_pext_glob(p2); //keeps 1st bit + 32 last
    uintptr_t p3upd = mask_pointer_bzhi(p3); // zeroes first 32 bits
    uintptr_t p4upd = mask_pointer_normally(p4); // zeroes first 32 bits
    
    printf("p1: %lx %lx\n", p1, p1upd); //0x0....90ABCDEF
    printf("p2: %lx %lx\n", p2, p2upd); //0x1....87654321
    printf("p3: %lx %lx\n", p3, p3upd); //0x123456789 expected
    printf("p4: %lx %lx\n", p4, p4upd); //0x23456789 expected
    printf("p5: %lx %llx\n", p4, _bextr_u64(p4, 32, 16)); // ef01
    
    volatile uintptr_t randupd;

    srandom(time(0));
    volatile uintptr_t* rand = (uintptr_t*)malloc(LOOP_COUNT * sizeof(uintptr_t));

    for (int i=0; i < LOOP_COUNT; i++) {
        rand[i] = (uintptr_t)random();
    }

    volatile clock_t start1, end1;
    start1 = clock();
    for (int i=0; i < LOOP_COUNT; i++) {
        randupd = mask_pointer_bzhi(rand[i]);
    }
    end1 = clock();
    printf("hw used: %ld cpu_time_used\n", (end1 - start1));

    volatile clock_t start2, end2;
    start2 = clock();
    for (int i=0; i < LOOP_COUNT; i++) {
        randupd = mask_pointer_normally(rand[i]);
    }
    end2 = clock();
    printf("normal mask: %ld cpu_time_used\n", (end2 - start2));

    volatile clock_t start3, end3;
    start3 = clock();
    for (int i=0; i < LOOP_COUNT; i++) {
        randupd = mask_pointer_upper(rand[i]);
    }
    end3 = clock();
    printf("single bit hw used: %ld cpu_time_used\n", (end3 - start3));

    volatile clock_t start4, end4;
    start4 = clock();
    for (int i=0; i < LOOP_COUNT; i++) {
        randupd = mask_pointer_upper_normally(rand[i]);
    }
    end4 = clock();
    printf("single bit normal and: %ld cpu_time_used\n", (end4 - start4));

    volatile clock_t start5, end5;
    start5 = clock();
    for (int i=0; i < LOOP_COUNT; i++) {
        randupd = _pext_u64(rand[i], TAG_MASK);
    }
    end5 = clock();
    printf("_pext tag extract used: %ld cpu_time_used\n", (end5 - start5));

    volatile clock_t start6, end6;
    start6 = clock();
    for (int i=0; i < LOOP_COUNT; i++) {
        randupd = _bextr_u64(rand[i], 4, 32);
    }
    end6 = clock();
    printf("_bextr tag extract faster? : %ld cpu_time_used\n", (end6 - start6));
    free((void*)rand);
    return 0;
}

/* Alternative implementations of masking to test their performance. */
#if 0
ALWAYS_INLINE uintptr_t NOINSTRUMENT(mask_pointer)(uintptr_t ptr, bool preserve) {
    return ptr & (preserve ? PRESERVE_MASK : ADDRSPACE_MASK);
}

uintptr_t NOINSTRUMENT(mask_pointer)(uintptr_t ptr, bool preserve) {
    if (preserve) {
        asm ("rol %0" : "+a"(ptr) ::);
        ptr &= ADDRSPACE_MASK;
        asm ("ror %0" : "+a"(ptr) ::);
        return ptr;
    }
    return ptr & ADDRSPACE_MASK;
}

static inline uintptr_t rot(uintptr_t input, unsigned bits) {
     return (input >> bits) | (input << ((sizeof (input) << 3) - bits));
}

uintptr_t NOINSTRUMENT(mask_pointer)(uintptr_t ptr, bool preserve) {
    if (preserve)
        return rot(rot(ptr, 63) & ADDRSPACE_MASK, 1);
    return ptr & ADDRSPACE_MASK;
}

uintptr_t NOINSTRUMENT(mask_pointer)(uintptr_t ptr, bool preserve) {
    if (preserve) {
        asm ("test %%rax, %%rax\n\t"
             "cmovs %[zero], %%eax"
            : "+a"(ptr)
            : [zero] "r"(0)
            : "cc");
        return ptr;
    }
    return ptr & ADDRSPACE_MASK;
}

uintptr_t NOINSTRUMENT(mask_pointer)(uintptr_t ptr, bool preserve) {
    if (preserve) {
        register uintptr_t tmp;
        asm ("movabs $0x80000000ffffffff, %[tmp]\n\t"
             "and %[tmp], %[ptr]"
            : [ptr] "+r"(ptr), [tmp] "=r"(tmp)
            :
            : "cc");
        return ptr;
    }
    return ptr & ADDRSPACE_MASK;
}

uintptr_t NOINSTRUMENT(mask_pointer)(uintptr_t ptr, bool preserve) {
    if (preserve) {
        asm ("test %[ptr], %[ptr]\n\t"
             "js ."
            :
            : [ptr] "r"(ptr)
            : "cc");
        return ptr;
    }
    return ptr & ADDRSPACE_MASK;
}
#endif
