// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapArray.h"

#include "yapLexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

yOperand yap32ArrayPushUnique(yap32Array *p, yU32 *v)
{
    int i;
    for(i = 0; i < p->count; i++)
    {
        if(p->data[i] == *v)
            return i;
    }
    return yap32ArrayPush(p, *v);
}

yOperand yap32ArrayPush(yap32Array *p, yU32 v)
{
    if(p->count == p->capacity)
    {
        int newSize = (p->capacity) ? p->capacity * 2 : 2;
        yapTraceMem(("                                     "
                     "ARRAYPUSH %p resize %p [%d] -> [%d]\n", p, p->data, p->capacity, newSize));
        p->capacity = newSize;
        p->data = yapRealloc(p->data, p->capacity * sizeof(yU32));
    }
    p->data[p->count++] = v;
    return (yOperand)(p->count - 1);
}

yU32 yap32ArrayPop(yap32Array *p)
{
    if(p->count == 0)
        return -1;

    return p->data[--p->count];
}

void yap32ArrayClear(yap32Array *p)
{
    p->count = 0;
    if(p->data)
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

void yapArrayInject(yapArray *p, void *v, int n)
{
    int index = yapArrayPush(p, v); // start with a push to cause the array to grow, if need be
    int injectIndex = index - n;    // calculate the proper home for the entry that is now at the endIndex
    if(injectIndex < 0)
    {
        printf("yapArrayInject(): injectIndex is %d! Terrible things are happening!\n", injectIndex);
        injectIndex = 0;
    }
    while(index > injectIndex)
    {
        p->data[index] = p->data[index - 1];
        index--;
    }
    p->data[index] = v;
}

void yapArrayUnshift(yapArray *p, void *v)
{
    yapArrayPush(p, NULL); // make some room!
    memmove(&p->data[1], &p->data[0], sizeof(void*) * (p->count - 1)); // this might be a no-op if the array was empty. This is okay.
    p->data[0] = v;
}

void yapArrayShrink(yapArray *p, int n, yapDestroyCB cb)
{
    while(p->count > n)
    {
        if(cb)
            cb(p->data[p->count-1]);
        p->count--;
    }
}

yOperand yapArrayPushUniqueString(yapArray *p, char *s)
{
    int i;
    for(i = 0; i < p->count; i++)
    {
        const char *v = (const char *)p->data[i];
        if(!strcmp(s, v))
        {
            yapFree(s);
            return (yOperand)i;
        }
    }
    return yapArrayPush(p, s);
}

yOperand yapArrayPushUniqueToken(yapArray *p, struct yapToken *token)
{
    return yapArrayPushUniqueString(p, yapTokenToString(token));
}

yOperand yapArrayPush(yapArray *p, void *v)
{
    if(p->count == p->capacity)
    {
        int newSize = (p->capacity) ? p->capacity * 2 : 2;
        yapTraceMem(("                                     "
                     "ARRAYPUSH %p resize %p [%d] -> [%d]\n", p, p->data, p->capacity, newSize));
        p->capacity = newSize;
        p->data = yapRealloc(p->data, p->capacity * sizeof(char **));
    }
    p->data[p->count++] = v;
    return (yOperand)(p->count - 1);
}

void *yapArrayPop(yapArray *p)
{
    if(!p->count)
        return NULL;
    return p->data[--p->count];
}

void *yapArrayTop(yapArray *p)
{
    if(!p->count)
        return NULL;
    return p->data[p->count - 1];
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
        for(i = 0; i < p->count; i++)
            cb(p->data[i]);
    }
    p->count = 0;
    if(p->data)
        yapFree(p->data);
    p->data = NULL;
    p->capacity = 0;
}

void yapArrayClearP1(yapArray *p, yapDestroyCB1 cb, void *arg1)
{
    if(cb)
    {
        int i;
        for(i = 0; i < p->count; i++)
            cb(arg1, p->data[i]);
    }
    yapArrayClear(p, NULL);
}

void yapArrayDestroy(yapArray *p, yapDestroyCB cb)
{
    yapArrayClear(p, cb);
    yapFree(p);
}
