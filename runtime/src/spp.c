///////////////////////////////////////////////
//////          spp.c           ///////////////
///////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>
#include <execinfo.h>
#include <signal.h>
#include "./spp.h" ///

#if defined(__x86_64__) && defined(__BMI2__) && !defined(HW_OFF) 
#include "x86intrin.h"
#endif

///////////////////////////////
///      Debug Macro        ///
///////////////////////////////

//#define SPP_PRINT_DEBUG
#ifdef SPP_PRINT_DEBUG
#  define dbg(x) x
#else
#  define dbg(x)
#endif

//#define SPP_DEBUG
#ifdef SPP_DEBUG
#  define __SPP_INLINE __attribute__((__optnone__))
#else
#  define __SPP_INLINE __attribute__((always_inline)) 
#endif

#define __SPP_USED __attribute__((__used__))

#define __SPP_ATTR __SPP_INLINE __SPP_USED

//#define ASSERT_DEBUG
#ifdef ASSERT_DEBUG
#  define assert_dbg(x) x
#else
#  define assert_dbg(x) 
#endif

///////////////////////////////
///  End of Debug Macro     ///
///////////////////////////////

/* 
 * https://stackoverflow.com/questions/3899870/print-call-stack-in-c-or-c
 */
void 
print_trace(void) 
{
    printf("\033[0;34m");
    char **strings;
    size_t i, size;
    enum Constexpr { MAX_SIZE = 1024 };
    void *array[MAX_SIZE];
    size = backtrace(array, MAX_SIZE);
    strings = backtrace_symbols(array, size);
    for (i = 0; i < size; i++)
        printf("%s\n", strings[i]);
    puts("");
    free(strings);
    printf("\033[0m");
}

__SPP_ATTR
uintptr_t
__spp_extract_tagval(void *ptr)
{
#if defined(__x86_64__) && defined(__BMI2__) && !defined(HW_OFF)
    return _pext_u64((uintptr_t)ptr, TAG_MASK);
#else
    return ((uintptr_t)ptr & TAG_MASK) >> NUM_PTR_BITS;
#endif
}

__SPP_ATTR
uintptr_t
__spp_is_pm_ptr(void *ptr)
{
#if defined(__x86_64__) && defined(__BMI2__) && !defined(HW_OFF)
    return _pext_u64((uintptr_t)ptr, PM_PTR_MASK);
#else 
    return ((uintptr_t)ptr) & PM_PTR_MASK;
#endif
}

__SPP_ATTR
void*
__spp_cleantag(void *ptr)
{
    dbg(printf(">>%s with %p\n", __func__, ptr);)

    if (!__spp_is_pm_ptr(ptr)) 
    {
        //if ptr is not tagged, return!
        return ptr;  
    }

    //MASK OUT THE TAG
#if defined(__x86_64__) && defined(__BMI2__) && !defined(HW_OFF)
    uintptr_t ptrval = _bzhi_u64((uintptr_t)ptr, NUM_PTR_BITS);
#else
    uintptr_t ptrval = (uintptr_t)ptr & PTR_MASK;
#endif

    dbg(printf(">>%s new_ptr: %p\n", __func__, ptrval);)
    return (void*)ptrval;
}

__SPP_ATTR
void*
__spp_cleantag_external(void *ptr)
{
    dbg(printf(">>%s with %p\n", __func__, ptr);)
    
    if (!__spp_is_pm_ptr(ptr)) 
    {
        //if ptr is not tagged, return!
        return ptr;  
    }
    
    //MASK OUT THE TAG
#if defined(__x86_64__) && defined(__BMI2__) && !defined(HW_OFF)
    uintptr_t ptrval = _bzhi_u64((uintptr_t)ptr, NUM_PTR_BITS);
#else
    uintptr_t ptrval = (uintptr_t)ptr & PTR_MASK;
#endif
    
    dbg(printf(">>%s old_ptr: %p new_ptr: %p\n", __func__, ptr, ptrval);)
    return (void*)ptrval;
}

__SPP_ATTR
void*
__spp_checkbound(void *ptr)
{
    dbg(printf(">>%s with %p\n", __func__, ptr);)
    
    // NOTE: BE CAREFUL with signed/unsigned,
    // when performing bit operation.
    // Especially, shift opreations.
    
    if (!__spp_is_pm_ptr(ptr)) 
    {
        //if ptr is not tagged, return!
        return ptr;
    }    
    
    // careful with function body optimised away at Opt level 2
#if defined(__x86_64__) && defined(__BMI2__) && !defined(HW_OFF)
    if (_pext_u64((uintptr_t)ptr, OVERFLOW_MASK))
#else
    if ((uintptr_t)ptr & OVERFLOW_MASK)
#endif
    {
        //simply print it in red
        printf("\033[0;31m");
        printf("!!!!> OVERFLOW detected at %s for %p\n", __func__, ptr);
        print_trace();
        printf("\033[0m");
        fflush(stdout);
        raise(SIGINT);
        _exit(1);
        // return NULL;      
    }

    return __spp_cleantag(ptr);
}

__SPP_ATTR
void* 
__spp_updatetag(void *ptr, int64_t off) {
    // ptr:  after pointer arithmetic  (i.e. GEP itself)
   
    ///////////////////////////////////////////////
    //  NOTE: BE CAREFUL with signed/unsigned,   //
    //  when performing bit operation.           //
    //  Especially, shift opreations.            //
    ///////////////////////////////////////////////
    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    
    if (!__spp_is_pm_ptr(ptr))
    {
        //if ptr is not tagged, return!
        return ptr; 
    }
    
    int64_t tag = (int64_t)__spp_extract_tagval(ptr);     
    tag = tag + off;  
    
    uintptr_t tempval = ((uintptr_t)tag)<<NUM_PTR_BITS | PM_PTR_SET;
    uintptr_t untagged = (uintptr_t)__spp_cleantag(ptr);
    uintptr_t ptrval = untagged | tempval; 
    
    dbg(printf(">>%s new_ptr: %p\n", __func__, ptrval);)
	
    return (void*)ptrval;
}

__SPP_ATTR
void* 
__spp_memintr_check_and_clean(void *ptr, int64_t off) {
    // ptr: pointer pass to LLVM memory intrinsic
    // off: offset to be added and checked
   
    // calculates the final address of the ptr according to the offset
    // performs bounds check
    // returns the clean ptr for the memory intrinsic function

    ///////////////////////////////////////////////
    //  NOTE: BE CAREFUL with signed/unsigned,   //
    //  when performing bit operation.           //
    //  Especially, shift opreations.            //
    ///////////////////////////////////////////////
    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    
    if (!__spp_is_pm_ptr(ptr))
    {
        //if ptr is not tagged, return!
        return ptr; 
    }
    
    int64_t tag = (int64_t)__spp_extract_tagval(ptr);   
    // -1 is applied due to addressing starting from 0   
    dbg(printf(">>%s with %p and offset %ld and tag %lx\n", __func__, ptr, off, tag);)
    tag = tag + (off - 1); 
    
    uintptr_t tempval = ((uintptr_t)tag)<<NUM_PTR_BITS | PM_PTR_SET;
    uintptr_t untagged = (uintptr_t)__spp_cleantag(ptr);
    uintptr_t ptrval = untagged | tempval; 
    
    dbg(printf(">>%s checked ptr: %p\n", __func__, ptrval);)
    
    if (__spp_checkbound((void*)ptrval) == NULL)
    {
        printf(">>%s with %p and offset %ld and tag %lx\n", __func__, ptr, off, tag);
        printf(">>%s checked ptr: %p\n", __func__, ptrval);
        return NULL;
    }
	
    return (void*)untagged;
}

// it is not desirable that this func is called
// normally, it SHOULDN'T be called
__SPP_ATTR
void* 
__spp_update_pointer(void *ptr, int64_t off) {
    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    assert_dbg(assert((uintptr_t)ptr==(int64_t)ptr);)
    void *newptr = (void*)((int64_t)ptr + off);
    return __spp_updatetag(newptr, off);
}

#ifdef __cplusplus
}
#endif

