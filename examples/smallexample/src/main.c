#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

extern void * mymalloc (size_t n);
extern void myfree (void * ptr);
extern void mymemcpy (void * dest, void * src, size_t sz, int loopcount);

int main (int argc, char ** argv)
{
    unsigned elemnum= 5;
    int * alloc= mymalloc(sizeof(int)); 
    int arr[elemnum];
    
    for (unsigned i=0; i < elemnum; i++) {
        if (i < 3) {
           arr[i]= i*11; 
        }
        else { 
            arr[i]= i*111;
        }
    }
    
    mymemcpy (alloc, arr, sizeof(int), elemnum);
    int * temp= alloc + 3;
    printf("> main -- arr[3]: %d\n", *temp);
    
    temp= temp + 1; 
    printf("> main -- arr[4]: %d\n", *temp);
    
    // out of bounds.
    temp= temp + 1; 
    printf("> main-- arr[5](overflow): %d\n", *temp);
     
    myfree(alloc);

    return 0;
}

