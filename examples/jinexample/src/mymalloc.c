#include <stdio.h>
#include <stdlib.h>

void * 
mymalloc (int x)
{
    printf("mymalloc.c - mymalloc called\n"); 
    return malloc (x);
}

