///////////////////////////////////////////////
//////          spp.h           ///////////////
///////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SPP_HEADER_H_
#define SPP_HEADER_H_

///////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#define TAGLENGTH 15 // TODO: change to compile parameter
#define PTRSIZEBITS (64 - TAGLENGTH)
#define PTRVALMASK ((1ULL << PTRSIZEBITS) - 1ULL)
#define TAGMASK (~PTRVALMASK)

extern void * __spp_cleantag(void * ptr);

extern void* __spp_updatetag(void* ptr, int64_t off);

///////////////////////////////////////////////

#endif // SPP_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

