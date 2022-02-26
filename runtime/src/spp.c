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
#include "./spp.h" ///

///////////////////////////////
///      Debug Macro        ///
///////////////////////////////

//#define SPP_PRINT_DEBUG
#ifdef SPP_PRINT_DEBUG
#  define dbg(x) x
#else
#  define dbg(x)
#endif

#define SPP_DEBUG
#ifdef SPP_DEBUG
#  define __SPP_ATTR __optnone__ 
#else
#  define __SPP_ATTR always_inline 
#endif

#define ASSERT_DEBUG
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

__attribute__((__used__))
__attribute__((__SPP_ATTR))
uintptr_t
__spp_extract_tagval (void *ptr)
{
    return ((uintptr_t)ptr)>>NUM_USED_BITS;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
uintptr_t
__spp_extract_x86_tagval (void *ptr)
{
    assert_dbg(assert(NUM_X86_USED_BITS==48);)
    return ((uintptr_t)ptr)>>NUM_X86_USED_BITS;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void*
__spp_cleantag (void *ptr)
{
    dbg(printf(">>%s with %p\n", __func__, ptr);)

    if (!__spp_extract_x86_tagval(ptr)) 
    {
        //if ptr is not tagged, return!
        return ptr;  
    }
    uintptr_t ptrval = ((((uintptr_t)ptr)<<NUM_SPARE_BITS)>>NUM_SPARE_BITS);
    dbg(printf(">>%s new_ptr: %p\n", __func__, ptrval);)
    return (void*)ptrval;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void*
__spp_cleantag_external(void *ptr)
{
    dbg(printf(">>%s with %p\n", __func__, ptr);)
    
    if (!__spp_extract_x86_tagval(ptr)) 
    {
        //if ptr is not tagged, return!
        return ptr;  
    }
    
    uintptr_t ptrval = ((((uintptr_t)ptr)<<NUM_SPARE_BITS)>>NUM_SPARE_BITS);
    dbg(printf(">>%s new_ptr: %p\n", __func__, ptrval);)
    return (void*)ptrval;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void*
__spp_checkbound(void *ptr)
{
    dbg(printf(">>%s with %p\n", __func__, ptr);)
    
    // NOTE: BE CAREFUL with signed/unsigned,
    // when performing bit operation.
    // Especially, shift opreations.
    
    if (!__spp_extract_x86_tagval(ptr)) 
    {
        //if ptr is not tagged, return!
        return ptr;
    }    
    
    uintptr_t tag = __spp_extract_tagval(ptr);
    
    // careful with function body optimised away at Opt level 2
    if (tag & 0x1000000) 
    {
        //simply print it in red
        printf("\033[0;31m");
        printf("!!!!> OVERFLOW detected at %s for %p\n", __func__, ptr);
        print_trace();
        printf("\033[0m");
        fflush(stdout);
        _exit(1);      
    }

    return __spp_cleantag(ptr);
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void* 
__spp_updatetag(void *ptr, int64_t off) {
    // ptr:  after pointer arithmetic  (i.e. GEP itself)
   
    ///////////////////////////////////////////////
    //  NOTE: BE CAREFUL with signed/unsigned,   //
    //  when performing bit operation.           //
    //  Especially, shift opreations.            //
    ///////////////////////////////////////////////
    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    
    if (!__spp_extract_x86_tagval(ptr))
    {
        //if ptr is not tagged, return!
        return ptr; 
    }
    
    int64_t tag = (int64_t)__spp_extract_tagval(ptr);     
    tag = tag + off;  
    
    uintptr_t tempval = ((uintptr_t)tag)<<NUM_USED_BITS;
    uintptr_t untagged = (uintptr_t)__spp_cleantag(ptr);
    uintptr_t ptrval = untagged | tempval; 
    
    dbg(printf(">>%s new_ptr: %p\n", __func__, ptrval);)
	
    return (void*)ptrval;
}

// it is not desirable that this func is called
// normally, it SHOULDN'T be called
__attribute__((__used__))
__attribute__((__SPP_ATTR))
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

