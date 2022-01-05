#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__attribute__((__used__))
__attribute__((__optnone__))
void
mymemcpy (void * dest, void * src, size_t sz, int loopcount)
{
    printf("mymemcpy called\n");
    int * ptr_d= (int*)dest;
    int * ptr_s= (int*)src;

    for (int i=0; i< loopcount; i++) {
        memcpy (ptr_d, ptr_s, sz);
        ptr_d++;    
        ptr_s++;
    }
}

