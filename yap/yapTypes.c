#include "yapTypes.h"

#include <stdlib.h>
#include <string.h>

yOperand yap32ArrayPushUnique(yap32Array *p, yU32 v)
{
    int i;
    for(i=0; i<p->count; i++)
    {
        if(p->data[i] == v)
            return i;
    }
    return yap32ArrayPush(p, v);
}

yOperand yap32ArrayPush(yap32Array *p, yU32 v)
{
    if(p->count == p->capacity)
    {
        p->capacity = (p->capacity) ? p->capacity*2 : 2;
        p->data = yapRealloc(p->data, p->capacity * sizeof(yU32));
    }
    p->data[p->count++] = v;
    return (yOperand)(p->count - 1);
}

void yap32ArrayClear(yap32Array *p)
{
    p->count = 0;
    yapFree(p->data);
    p->data = NULL;
    p->capacity = 0;
}

void yapArraySquash(yapArray *p)
{
    int head = 0;
    int tail = 0;
    while(tail < p->count)
    {
        if(p->data[tail] != NULL)
        {
            p->data[head] = p->data[tail];
            head++;
        }
        tail++;
    }
    p->count = head;
}

yOperand yapArrayPushUniqueString(yapArray *p, const char *s)
{
    int i;
    for(i=0; i<p->count; i++)
    {
        const char *v = (const char *)p->data[i];
        if(!strcmp(s, v))
        {
            return (yOperand)i;
        }
    }
    return yapArrayPush(p, yapStrdup(s));
}

yOperand yapArrayPushUniqueStringLen(yapArray *p, const char *s, int len)
{
    char *temp;
    int i;
    for(i=0; i<p->count; i++)
    {
        const char *v = (const char *)p->data[i];
        if((strlen(v) == len) && !strncmp(s, v, len))
        {
            return (yOperand)i;
        }
    }
    temp = yapAlloc(len+1);
    memcpy(temp, s, len);
    temp[len] = 0;
    return yapArrayPush(p, temp);
}

yOperand yapArrayPush(yapArray *p, void *v)
{
    if(p->count == p->capacity)
    {
        p->capacity = (p->capacity) ? p->capacity*2 : 2;
        p->data = yapRealloc(p->data, p->capacity * sizeof(char**));
    }
    p->data[p->count++] = v;
    return (yOperand)(p->count - 1);
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

void yapArrayClear(yapArray *p, yapDestroyCB cb)
{
    if(cb)
    {
        int i;
        for(i=0; i<p->count; i++)
            cb(p->data[i]);
    }
    p->count = 0;
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
