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


#define __SPP_MASK_TAG_OUT 0xFFFFFFFFFFFF

__attribute__((__used__))
__attribute__((always_inline))
void *
__SPP_clean_tag (void * ptr)
{
    return (void*)(((uintptr_t)ptr) & __SPP_MASK_TAG_OUT);
}

///////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

