///////////////////////////////////////////////
//////          spp.h           ///////////////
///////////////////////////////////////////////

#ifndef SPP_HEADER_H_
#define SPP_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
#include <assert.h>
#include <math.h>
//#include <stdbool.h>

#define __SPP_MASK_TAG_OUT 0xFFFFFFFFFFFF

//#define TAGLENGTH 15 // TODO: change to compile parameter
//#define PTRSIZEBITS (64 - TAGLENGTH)
//#define PTRVALMASK ((1ULL << PTRSIZEBITS) - 1ULL)
//#define TAGMASK (~PTRVALMASK)

#define NUM_SPARE_BITS 25 
#define NUM_USED_BITS (64-NUM_SPARE_BITS) 

extern void * __spp_cleantag (void * ptr);
extern void * __spp_checkbound (void * ptr);

extern void* __spp_updatetag(void* ptr, int64_t off);
extern void* __spp_updatetag_DEBUG(void* ptr, int64_t off, void* ptrop);
extern void* __spp_update_pointer(void* ptr, int64_t off);

///////////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif // SPP_HEADER_H_
