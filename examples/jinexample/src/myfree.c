#include <stdio.h>
#include <stdlib.h>

void
myfree (void * ptr)
{
    printf("myfree.c - myfree is called\n");
    free (ptr);
}
