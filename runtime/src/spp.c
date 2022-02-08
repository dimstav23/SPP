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
#include <unistd.h>
//#include <stdbool.h>
#include <assert.h>
#include <math.h>
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

///////////////////////////////
///  Debug Macro up to here ///
///////////////////////////////

// (check ./spp.h)
// NUM_SPARE_BITS= 25 
// NUM_USED_BITS = (64-NUM_SPARE_BITS) 

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
    assert(NUM_X86_USED_BITS==48);
    return ((uintptr_t)ptr)>>NUM_X86_USED_BITS;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void *
__spp_cleantag (void * ptr)
{
    //return (void*)((((uintptr_t)ptr)<<NUM_SPARE_BITS)>>NUM_SPARE_BITS);
    dbg(printf("    __spp_cleantag\n");)
    dbg(printf("      ptr: 0x%.16" PRIXPTR "\n", (uintptr_t)ptr);)

    if (!__spp_extract_x86_tagval(ptr)) {
        dbg(printf("    :: skip. volptr\n");)
       return ptr;  
    }
    uintptr_t ptrval= ((((uintptr_t)ptr)<<NUM_SPARE_BITS)>>NUM_SPARE_BITS);
    dbg(printf("      newptr: 0x%.16" PRIXPTR "\n", ptrval);)
    return (void*)ptrval;
}

 __attribute__((__used__))
__attribute__((__SPP_ATTR))
void *
__spp_cleantag_external (void * ptr)
{
    // if upper 16 bits are tag-free, returns;
    dbg(printf("  __spp_cleantag_external --------------\n");)
    dbg(printf("    ptr: 0x%.16" PRIXPTR "\n", (uintptr_t)ptr);)
    
    if (!__spp_extract_x86_tagval(ptr)) {
        dbg(printf("    :: Skip. volptr\n");)
        return ptr;  
    }
    
    uintptr_t ptrval= ((((uintptr_t)ptr)<<NUM_SPARE_BITS)>>NUM_SPARE_BITS);
    dbg(printf("    newptr: 0x%.16" PRIXPTR "\n", ptrval);)
    return (void*)ptrval;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void *
__spp_checkbound (void * ptr)
{
    // NOTE: BE CAREFUL with signed/unsigned,
    // when performing bit operation.
    // Especially, shift opreations.
    
    dbg(printf("  __spp_checkbound______________\n");)
    dbg(printf("    ptr: 0x%.16" PRIXPTR "\n", (uintptr_t)ptr);)
    
    /// Returns, if it's tag-free. ///
    if (!__spp_extract_x86_tagval(ptr)) {
        dbg(printf("    :: Skip. volptr \n");)
        return ptr;
    } 
    
    
    uintptr_t tag= __spp_extract_tagval(ptr);
    dbg(printf("    tag: 0x%.16" PRIXPTR "\n", tag);)
    
    /// Now ptr is a tagged pointer.
    
    // just testing. modify this.
    // careful with function body optimised away at Opt level 2
    if (tag & 0x1000000) {
        printf("\n --->__spp_error: at 0x%.16" PRIXPTR " <--------------\n\n", (uint64_t)ptr); 
        //internal__exit(1);
        _exit(1);
        //assert(!(tag & 0x8000));
        //return ptr;        
    }
    return __spp_cleantag(ptr); 
    
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void * 
__spp_updatetag(void* ptr, int64_t off) {
    // ptr:   after pointer arithmetic  (i.e. GEP itself)
   
    ///////////////////////////////////////////////
    //  NOTE: BE CAREFUL with signed/unsigned,   //
    //  when performing bit operation.           //
    //  Especially, shift opreations.            //
    ///////////////////////////////////////////////
    dbg(printf("  __spp_updatetag______________\n");)
    dbg(printf("    ptr:\t0x%.16" PRIXPTR ", off:\t0x%.16" PRIXPTR "\n", (uintptr_t)ptr, off);)
    
    if (!__spp_extract_x86_tagval(ptr))   {
        dbg(printf("    :: Skip. volptr\n"); )
        return ptr; 
    }
    
    
    int64_t tag= (int64_t)__spp_extract_tagval(ptr); 
    
    dbg(printf("    oldtag:\t0x%.16" PRIXPTR ", ", (uint64_t)tag);)
    
    /// Returns, if it's tag-free. ///
    
    tag= tag + off;  
    
    dbg(printf("newtag:\t0x%.16" PRIXPTR ", (intval: %ld)\n", (uint64_t)tag, tag);)
    
    uintptr_t tempval = ((uintptr_t)tag)<<NUM_USED_BITS;
    uintptr_t untagged= (uintptr_t)__spp_cleantag(ptr);
    uintptr_t ptrval= untagged | tempval; 
    
    //assert(tag>0);
    //if (tag <= 0) {
    //    dbg(printf("    ---> __spp_warning: Out_of_bounds! Tag: %ld, ptr: 0x%.16" PRIXPTR "\n", tag, (uintptr_t)ptr);)
    //}
    dbg(printf("    newptr:\t0x%.16" PRIXPTR "\n", ptrval);)
	
    return (void*)ptrval;
}

///
/// This function is only for debugging hook __spp_updatetag
/// spp pass calls this at GEP instead of __spp_updatetag in debug mode
/// 
/*
__attribute__((__used__))
__attribute__((__SPP_ATTR))
void * 
__spp_updatetag_DEBUG (void* ptr, int64_t off, void * ptrop) {
    // ptr:   after pointer arithmetic  (i.e. GEP itself)
    // ptrop: before pointer arithmetic (i.e. GEP's ptrOperand )
    
    uintptr_t tag= __spp_extract_tagval(ptr); 
    
    /// Returns, if it's tag-free. ///
    if (!tag)   return ptr; 
     
    assert((uintptr_t)ptr == (int64_t)ptr);
    
    dbg(printf("  __spp_updatetag_DEBUG\n");)
    dbg(printf("    ptrop:\t0x%.16" PRIXPTR ", off:\t0x%.16" PRIXPTR "\n", (uintptr_t)ptrop, off);)
    
    tag= ((int64_t)tag) - off;  
    
    dbg(printf("    ptr:\t0x%.16" PRIXPTR ", ", (uintptr_t)ptr);)
    dbg(printf("tag:\t0x%.16" PRIXPTR "\n", tag);)
    
    //assert(tag>=0 && "assert_0");
    
    uint64_t tempval = (tag<<NUM_USED_BITS);
    uint64_t untagged= (((uint64_t)ptr)<<NUM_SPARE_BITS)>>NUM_SPARE_BITS;
    uint64_t ptrval= ((uint64_t)untagged) | tempval; 
    
    if ((int64_t)tag <= 0) {
        printf("    ---> __spp_warning: Out_of_bounds! Tag: %ld \n", tag);
        //exit(0); // why lnt skips this???
        ptrval=  ptrval | (1ULL<<63); // just for testing.
    }
    dbg(printf("    newptr:\t0x%.16" PRIXPTR "\n", ptrval);)
    //assert(tag>=0 && "assert_2");
	
    return (void*)ptrval;
}
*/
	
// it is not desirable that this func is called
// ignore this.
__attribute__((__used__))
__attribute__((__SPP_ATTR))
void * 
__spp_update_pointer (void* ptr, int64_t off) {
    assert((uintptr_t)ptr==(int64_t)ptr);
    //dbg(printf("__spp_update_pointer. oldptr: 0x%.16" PRIXPTR ", off: 0x%.16" PRIXPTR "\n", (uintptr_t)ptr, (uintptr_t)off);)

    void *  newptr = (void*)((int64_t)ptr + off);
    //dbg(printf("                      newptr: 0x%.16" PRIXPTR "\n", (uintptr_t)newptr);)
    return __spp_updatetag(newptr, off);

}
///////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

