#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__attribute__((__used__))
__attribute__((__optnone__))
void
mystrcpy (char * x, char * y)
{
    strcpy (x, y);
}

