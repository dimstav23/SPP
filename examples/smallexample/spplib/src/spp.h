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

extern void * __spp_clean_tag (void * ptr);

extern void* __spp_updatetag(void* ptr, int64_t off);

///////////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif // SPP_HEADER_H_
