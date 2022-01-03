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
//#include <stdbool.h>
#include <assert.h>
#include <math.h>

#define __SPP_MASK_TAG_OUT 0xFFFFFFFFFFFF
#define USED_NUM_BITS 48

extern void * __spp_cleantag (void * ptr);

extern void* __spp_updatetag(void* ptr, int64_t off);
extern void* __spp_updatetag_DEBUG(void* ptr, int64_t off, void* ptrop);
extern void* __spp_update_pointer(void* ptr, int64_t off);

///////////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif // SPP_HEADER_H_
