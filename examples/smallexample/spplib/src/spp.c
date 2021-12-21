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

__attribute__((__used__))
__attribute__((always_inline))
void *
__spp_clean_tag (void * ptr)
{
    return (void*)(((uintptr_t)ptr) & __SPP_MASK_TAG_OUT);
}

__attribute__((__used__))
__attribute__((always_inline))
void * 
__spp_updatetag(void* ptr, int64_t off) {
	void* ptrval = ptr;// (void*)((uint64_t)ptr + (off << PTRSIZEBITS));
	printf("Offset: %lu \n", off);
	return ptrval;
}
///////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

