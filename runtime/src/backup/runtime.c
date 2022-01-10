#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <pthread.h>
#include <sys/mman.h>

#define INLINEATTR __attribute__((always_inline))
#define TAGLENGTH 15 // TODO: change to compile parameter
#define PTRSIZEBITS (64 - TAGLENGTH)
#define PTRVALMASK ((1ULL << PTRSIZEBITS) - 1ULL)

INLINEATTR uint64_t* __spp_cleantag(void* ptr) {
	void* ptrval = (void*)((uint64_t)ptr & PTRVALMASK);
	return ptrval;
}

INLINEATTR void* __spp_updatetag(void* ptr, int64_t off) {
	void* ptrval = ptr;// (void*)((uint64_t)ptr + (off << PTRSIZEBITS));
	printf("Offset: %lu \n", off);
	return ptrval;
}

#ifdef __cplusplus
}
#endif
