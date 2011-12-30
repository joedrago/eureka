#include "yapTypes.h"

#include "yapLexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *yapAlloc(ySize bytes)
{
    void *ptr = calloc(1, bytes);
    yapTraceMem(("                                     ALLOC %p [%d]\n", ptr, bytes));
    return ptr;
}

void *yapRealloc(void *ptr, ySize bytes)
{
    void *p = realloc(ptr, bytes);
    yapTraceMem(("                                     REALLOC %p -> %p [%d]\n", ptr, p, bytes));
    return p;
}

void yapFree(void *ptr)
{
    yapTraceMem(("                                     FREE %p\n", ptr));
    if(ptr == 0)
        printf("huh\n");
    free(ptr);
}

char *yapStrdup(const char *s)
{
    yapTraceMem(("                                     STRDUP %p\n", s));
    return strdup(s);
}
