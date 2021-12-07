#include <stdio.h>
#include <stdlib.h>

extern void * mymalloc (int x);
extern void myfree (void * ptr);

void mystrcpy (char * x, char * y)
{
    strcpy(x, y);
}

int main (int argc, char ** argv)
{
    char* str_src= "spp_test";
    void * str_dest= mymalloc(10); 

    mystrcpy (str_dest, str_src);
    
    printf("print str_dest: %s\n", str_dest);

    myfree(str_dest);

    return 0;
}

