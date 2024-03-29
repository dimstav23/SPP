///////////////////////////////////////////////
//////          spp.h           ///////////////
///////////////////////////////////////////////

#ifndef SPP_HEADER_H_
#define SPP_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
#include <assert.h>
#include <math.h>

#define POINTER_BITS (sizeof (void*) * 8)
#define OVERFLOW_BIT 1
#define PM_PTR_BIT 1

#ifndef TAG_BITS
#define TAG_BITS 26
#endif

#define NUM_RESERVED_BITS (PM_PTR_BIT + OVERFLOW_BIT + TAG_BITS) 
#define NUM_PTR_BITS (POINTER_BITS - NUM_RESERVED_BITS)

#define PM_PTR_SET (1ULL << (POINTER_BITS - 1))

#define PM_PTR_MASK (1ULL << (POINTER_BITS - 1))
#define OVERFLOW_MASK  (1ULL << (POINTER_BITS - PM_PTR_BIT - OVERFLOW_BIT))
#define PTR_MASK (((1ULL << NUM_PTR_BITS) - 1) | OVERFLOW_MASK) 
#define TAG_MASK (~PTR_MASK | OVERFLOW_MASK)

extern void* __spp_cleantag(void *ptr);
extern void* __spp_cleantag_direct(void *ptr);

extern void* __spp_cleantag_external(void *ptr);
extern void* __spp_cleantag_external_direct(void *ptr);

extern void* __spp_checkbound(void *ptr, int64_t deref_size);
extern void* __spp_checkbound_direct(void *ptr, int64_t deref_size);

extern void* __spp_updatetag(void *ptr, int64_t off);
extern void* __spp_updatetag_direct(void *ptr, int64_t off);

extern void* __spp_update_pointer(void *ptr, int64_t off);

extern void* __spp_memintr_check_and_clean(void *ptr, int64_t off);
extern void* __spp_memintr_check_and_clean_direct(void *ptr, int64_t off);

extern void __spp_manual_checkbound(void* ptr);

extern uintptr_t __spp_extract_tagval(void *ptr);

extern uintptr_t __spp_is_pm_ptr(void *ptr);

extern void* __spp_update_check_clean(void *ptr, int64_t off);
extern void* __spp_update_check_clean_direct(void *ptr, int64_t off);

extern void __spp_runtime_stats();
extern void __spp_stats_reset();
extern void __spp_gather_stats();
extern void __spp_print_mean_stats(int repeats);
extern void __spp_update_checkcnt(char *fname);

#ifdef __cplusplus
}
#endif

#endif // SPP_HEADER_H_
