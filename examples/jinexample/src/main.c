#include <stdio.h>
#include <stdlib.h>

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

    printf("print_2 temp: %s\n", temp);
    
    temp++;
    
    printf("print_3 temp: %s\n", temp);
    
    myfree(str_dest);

    return 0;
}

