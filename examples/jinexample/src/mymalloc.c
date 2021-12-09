/// mymalloc.c

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

void *
__SPP_tag_alloc (void * ptr)
{
    return (void*)((((uintptr_t)1)<<63) | (uintptr_t)ptr);                                   
}

void * 
mymalloc (int x)
{
    void * ptr=  malloc (x);
    printf("mymalloc.tag-free ptr:\t\t %p\n", ptr);
//    ptr= __SPP_tag_alloc (ptr); 
    printf("mymalloc.tagged ptr:\t\t %p\n", ptr);
    return ptr;
}

