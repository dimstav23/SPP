#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

extern void * mymalloc (int x);
extern void myfree (void * ptr);
extern void mystrcpy (char * x, char * y);

int main (int argc, char ** argv)
{
    char* str_src= "spp_test";
    char * str_dest= mymalloc(10); 

    mystrcpy (str_dest, str_src);
    
    printf("print_1 str_dest: %s\n", str_dest);
    
    char* temp= str_dest;
    
    temp++;
    printf("temp_tagged_1: 0x%" PRIx64 "\n", (uintptr_t)temp);
    
    temp++;
    printf("temp_tagged_1: 0x%" PRIx64 "\n", (uintptr_t)temp);
    
    myfree(str_dest);

    return 0;
}

