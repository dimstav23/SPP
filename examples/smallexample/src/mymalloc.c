/// mymalloc.c

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

void *
__spp_tag_alloc (void * ptr)
{
    return (void*)((((uintptr_t)1)<<63) | (uintptr_t)ptr);                                   
}

void * 
mymalloc (size_t tysz)
{
    void * ptr=  malloc (tysz*5);
    printf("mymalloc. tagged ptr:\t\t 0x%" PRIx64 "\n", (uintptr_t)ptr);
    return ptr;
}

