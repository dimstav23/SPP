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
#define PTRSIZEBITS 32 // TODO: change to compile parameter
#define PTRVALMASK ((1ULL << PTRSIZEBITS) - 1ULL)

INLINEATTR uint64_t* __spp_cleantag(const uint64_t* ptr) {
	void* ptrval = (void*)((uint64_t)ptr & PTRVALMASK);
	return ptrval;
}
/*
int main() {
	printf("%lu\n", (uint64_t) __spp_cleantag((void*)0xFFFFFFFFFFFF));
}*/
#ifdef __cplusplus
}
#endif