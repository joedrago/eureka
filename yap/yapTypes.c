#include "yapTypes.h"

#include <stdlib.h>
#include <string.h>

void yapArrayPush(yapArray *p, void *v)
{
    if(p->count == p->capacity)
    {
        p->capacity = (p->capacity) ? p->capacity*2 : 2;
        p->data = yapRealloc(p->data, p->capacity * sizeof(char**));
    }
    p->data[p->count++] = v;
}

void * yapArrayPop(yapArray *p)
{
    if(!p->count)
        return NULL;
    return p->data[--p->count];
}

void * yapArrayTop(yapArray *p)
{
    if(!p->count)
        return NULL;
    return p->data[p->count-1];
}

yU32 yapArrayCount(yapArray *p)
{
    return p->count;
}

void yapArrayDestroy(yapArray *p, yapDestroyCB cb)
{
    int i;
    if(cb)
    {
        for(i=0; i<p->count; i++)
            cb(p->data[i]);
    }
    yapArrayClear(p);
    yapFree(p->data);
    p->data = NULL;
    p->capacity = 0;
}

void * yapAlloc(ySize bytes)
{
    return calloc(1, bytes);
}

void * yapRealloc(void *ptr, ySize bytes)
{
    return realloc(ptr, bytes);
}

void yapFree(void *ptr)
{
    free(ptr);
}

char * yapStrdup(const char *s)
{
    return strdup(s);
}
