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
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "./spp.h" ///

__attribute__((__used__))
__attribute__((always_inline))
void *
__spp_cleantag(void * ptr)
{
    return (void*)(((uintptr_t)ptr) & PTRVALMASK);
}

__attribute__((__used__))
__attribute__((always_inline))
void * 
__spp_updatetag(void* ptr, int64_t off) {
	int64_t delta = ((uint64_t)ptr & TAGMASK) >> PTRSIZEBITS;
	delta += off;
	void* res = ((uint64_t)ptr & PTRVALMASK) + (delta << PTRSIZEBITS);
	printf("Orig ptr: %lu Updated delta: %lu Offset %lu Updated ptr %lu \n", (int64_t)ptr, (int64_t)delta, (int64_t)off, (int64_t)res);
	//TODO: Add Overflow/Underflow checks
	return res;
}
///////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

