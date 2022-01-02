///////////////////////////////////////////////
//////          spp.c           ///////////////
///////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
//#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "./spp.h" ///

//#define USED_NUM_BITS 48

#define SPP_PRINT_DEBUG
#ifdef SPP_PRINT_DEBUG
#  define dbg(x) x
#else
#  define dbg(x)
#endif

__attribute__((__used__))
//__attribute__((always_inline))
int64_t 
__spp_extract_tagval (void *ptr)
{
    return ((int64_t)ptr)>>USED_NUM_BITS;
}

__attribute__((__used__))
__attribute__((always_inline))
void *
__spp_cleantag (void * ptr)
{
    return (void*)(((uintptr_t)ptr) & __SPP_MASK_TAG_OUT);
}


__attribute__((__used__))
//__attribute__((always_inline))
void * 
__spp_updatetag_DEBUG (void* ptr, void * ptrop, int64_t off) {
    // ptr: after pointer arithmetic
    // ptrop: before pointer arithmetic (i.e. GEP's ptrOperand )
    assert((uintptr_t)ptr == (int64_t)ptr);
    
    dbg(printf("__spp_updatetag. ptrop: 0x%.16" PRIXPTR ",  off  : 0x%.16" PRIXPTR "\n", (uintptr_t)ptrop, off);)
    
    int64_t tempval = (__spp_extract_tagval(ptr) + off)<<USED_NUM_BITS;
    void * ptrval= (void*)(((int64_t)ptr) | tempval); 
    
    dbg(printf("                   ptr: 0x%.16" PRIXPTR ", newptr: 0x%.16" PRIXPTR "\n", (uintptr_t)ptr, (uintptr_t)ptrval);)
	
    /*void* ptrval = ptr;// (void*)((uint64_t)ptr + (off << PTRSIZEBITS));
	printf("SPP_Offset: %lu \n", off);
	*/
    return ptrval;
}

__attribute__((__used__))
//__attribute__((always_inline))
void * 
__spp_updatetag(void* ptr, int64_t off) {
    
    assert((uintptr_t)ptr==(int64_t)ptr);
    
    dbg(printf("__spp_updatetag. oldptr: 0x%.16" PRIXPTR ", off: 0x%.16" PRIXPTR "\n", (uintptr_t)ptr, off);)
    
    int64_t tempval = (__spp_extract_tagval(ptr) + off)<<USED_NUM_BITS;
    void * ptrval= (void*)(((int64_t)ptr) | tempval); 
    
    dbg(printf("                 newptr: 0x%.16" PRIXPTR "\n", (uintptr_t)ptrval);)
	
    /*void* ptrval = ptr;// (void*)((uint64_t)ptr + (off << PTRSIZEBITS));
	printf("SPP_Offset: %lu \n", off);
	*/
    return ptrval;
}

	
// it is desirable that this func is not called
__attribute__((__used__))
//__attribute__((inline))
void * 
__spp_update_pointer (void* ptr, int64_t off) {
    assert((uintptr_t)ptr==(int64_t)ptr);
    dbg(printf("__spp_update_pointer. oldptr: 0x%.16" PRIXPTR ", off: 0x%.16" PRIXPTR "\n", (uintptr_t)ptr, (uintptr_t)off);)

    void *  newptr = ptr + off;
    dbg(printf("                      newptr: 0x%.16" PRIXPTR "\n", (uintptr_t)newptr);)
    return __spp_updatetag(newptr, off);

}
///////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

