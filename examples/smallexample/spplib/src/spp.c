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

//#define SPP_PRINT_DEBUG
#ifdef SPP_PRINT_DEBUG
#  define dbg(x) x
#else
#  define dbg(x)
#endif

//typedef enum { false = 0, true = !false } mybool;
// NUM_USED_BITS = 48
// NUM_SPARE_BITS= 16 (check ./spp.h)

__attribute__((__used__))
//__attribute__((always_inline))
int64_t 
__spp_extract_tagval (void *ptr)
{
    return ((int64_t)ptr)>>NUM_USED_BITS;
}

__attribute__((__used__))
//__attribute__((always_inline))
void *
__spp_cleantag (void * ptr)
{
    return (void*)(((uintptr_t)ptr) & __SPP_MASK_TAG_OUT);
}

 __attribute__((__used__))
__attribute__((__optnone__)) /// -> disable later. just to check if it's hooked.
//__attribute__((always_inline))
void *
__spp_checkbound (void * ptr)
{
    int64_t tag= __spp_extract_tagval(ptr);
    
    /// Returns, if it's tag-free. ///
    if (!tag) return ptr;
    
    /////////////////////////////////////////////////// 
    /// Now ptr is a tagged pointer.
    /// TODO: do something with a tagged pointer.   
    /////////////////////////////////////////////////// 

    // Or, return a tag-free pointer, if you generate one above. 
    return __spp_cleantag(ptr); 
}

///
/// This function is only for debugging hook __spp_updatetag
/// spp pass calls this at GEP instead of __spp_updatetag in debug mode
/// 
__attribute__((__used__))
__attribute__((__optnone__)) /// disable later. just to check if it's hooked.
//__attribute__((always_inline))
void * 
__spp_updatetag_DEBUG (void* ptr, int64_t off, void * ptrop) {
    // ptr:   after pointer arithmetic  (i.e. GEP itself)
    // ptrop: before pointer arithmetic (i.e. GEP's ptrOperand )
    
    int64_t tag= (int64_t)__spp_extract_tagval(ptr); 
    
    /// Returns, if it's tag-free. ///
    if (!tag)   return ptr; 
     
    assert((uintptr_t)ptr == (int64_t)ptr);
    
    dbg(printf("__spp_updatetag_DEBUG\n");)
    dbg(printf("ptrop:\t0x%.16" PRIXPTR ", off:\t0x%.16" PRIXPTR "\n", (uintptr_t)ptrop, off);)
    
    tag= tag - off;  
    
    dbg(printf("ptr:\t0x%.16" PRIXPTR ", ", (int64_t)ptr);)
    dbg(printf("tag:\t0x%.16" PRIXPTR "\n", (int64_t)tag);)
    
    //assert(tag>=0 && "assert_0");
    
    if (tag < 0) {
        printf("__spp_error: Out_of_bounds! Tag: %ld .. exiting\n", tag);
        //exit(0); // why lnt skips this???
        assert(tag>=0 && "assert_1");
    }
    //assert(tag>=0 && "assert_2");
    
    int64_t tempval = (tag<<NUM_USED_BITS);
    int64_t untagged= (((int64_t)ptr)<<NUM_SPARE_BITS)>>NUM_SPARE_BITS;
    void * ptrval= (void*)(((int64_t)untagged) | tempval); 
    
    dbg(printf("masked:\t0x%16" PRIXPTR ",newtag:\t0x%.16" PRIXPTR "\n", untagged, (int64_t)tag);)
    dbg(printf("ptr:\t0x%.16" PRIXPTR ", newptr:\t0x%.16" PRIXPTR "\n", (uintptr_t)ptr, (uintptr_t)ptrval);)
	
    /*void* ptrval = ptr;// (void*)((uint64_t)ptr + (off << PTRSIZEBITS));
	printf("SPP_Offset: %lu \n", off);
	*/
    return ptrval;
}

__attribute__((__used__))
__attribute__((__optnone__)) /// disable later. just to check if it's hooked.
//__attribute__((always_inline))
//__attribute__((always_inline))
void * 
__spp_updatetag(void* ptr, int64_t off) {
    
    assert((uintptr_t)ptr==(int64_t)ptr); // just worried about integer overflow
    assert(NUM_USED_BITS==48); 
    
    int64_t tag= (int64_t)__spp_extract_tagval(ptr); 
    
    /// Returns, if it's tag-free. ///
    if (!tag)   return ptr; 

    dbg(printf("__spp_updatetag ------\n");) 
    dbg(printf("ptr:\t0x%.16" PRIXPTR ", ", (uintptr_t)ptr);)
    dbg(printf("off:\t0x%.16" PRIXPTR "\n", off);)
    
    /////////////////////////////////////////////////// 
    /// Now ptr is a tagged pointer.
    /// TODO: do something with a tagged pointer.   
    /// TODO: Update a tag!
    /////////////////////////////////////////////////// 
    
    int64_t tempval = (tag<<NUM_USED_BITS); 
    int64_t untagged= (((int64_t)ptr)<<NUM_SPARE_BITS)>>NUM_SPARE_BITS;
    // You can also do:: int64_t untagged= __spp_cleantag(ptr);
    
    void * ptrval= (void*)(((int64_t)untagged) | tempval); 
    
    dbg(printf("newptr:\t0x%.16" PRIXPTR "\n", (uintptr_t)ptrval);)
	
    /*void* ptrval = ptr;// (void*)((uint64_t)ptr + (off << PTRSIZEBITS));
	printf("SPP_Offset: %lu \n", off);
	*/

    return ptrval;
}

	
// it is not desirable that this func is called
__attribute__((__used__))
//__attribute__((inline))
void * 
__spp_update_pointer (void* ptr, int64_t off) {
    assert((uintptr_t)ptr==(int64_t)ptr);
    //dbg(printf("__spp_update_pointer. oldptr: 0x%.16" PRIXPTR ", off: 0x%.16" PRIXPTR "\n", (uintptr_t)ptr, (uintptr_t)off);)

    void *  newptr = ptr + off;
    //dbg(printf("                      newptr: 0x%.16" PRIXPTR "\n", (uintptr_t)newptr);)
    return __spp_updatetag(newptr, off);

}
///////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

