// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapContext.h"

#include "yapLexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

yOperand yap32ArrayPushUnique(struct yapContext *Y, yap32Array *p, yU32 *v)
{
    int i;
    for(i = 0; i < p->count; i++)
    {
        if(p->data[i] == *v)
        {
            return i;
        }
    }
    return yap32ArrayPush(Y, p, *v);
}

yOperand yap32ArrayPush(struct yapContext *Y, yap32Array *p, yU32 v)
{
    if(p->count == p->capacity)
    {
        int newSize = (p->capacity) ? p->capacity * 2 : 2;
#ifndef YAP_TRACE_MEMORY_STATS_ONLY
        yapTraceMem(("                                     "
                     "ARRAYPUSH %p resize %p [%d] -> [%d]\n", p, p->data, p->capacity, newSize));
#endif
        p->capacity = newSize;
        p->data = yapRealloc(p->data, p->capacity * sizeof(yU32));
    }
    p->data[p->count++] = v;
    return (yOperand)(p->count - 1);
}

yU32 yap32ArrayPop(struct yapContext *Y, yap32Array *p)
{
    if(p->count == 0)
    {
        return -1;
    }

    return p->data[--p->count];
}

void yap32ArrayClear(struct yapContext *Y, yap32Array *p)
{
    p->count = 0;
    if(p->data)
    {
        yapFree(p->data);
    }
    p->data = NULL;
    p->capacity = 0;
}

void yapArraySquash(struct yapContext *Y, yapArray *p)
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

void yapArrayInject(struct yapContext *Y, yapArray *p, void *v, int n)
{
    int index = yapArrayPush(Y, p, v); // start with a push to cause the array to grow, if need be
    int injectIndex = index - n;    // calculate the proper home for the entry that is now at the endIndex
    if(injectIndex < 0)
    {
        yapTraceExecution(("yapArrayInject(): injectIndex is %d! Terrible things are happening!\n", injectIndex));
        injectIndex = 0;
    }
    while(index > injectIndex)
    {
        p->data[index] = p->data[index - 1];
        index--;
    }
    p->data[index] = v;
}

void yapArrayUnshift(struct yapContext *Y, yapArray *p, void *v)
{
    yapArrayPush(Y, p, NULL); // make some room!
    memmove(&p->data[1], &p->data[0], sizeof(void *) * (p->count - 1)); // this might be a no-op if the array was empty. This is okay.
    p->data[0] = v;
}

void yapArrayShrink(struct yapContext *Y, yapArray *p, int n, yapDestroyCB cb)
{
    while(p->count > n)
    {
        if(cb)
        {
            cb(Y, p->data[p->count-1]);
        }
        p->count--;
    }
}

yOperand yapArrayPushUniqueString(struct yapContext *Y, yapArray *p, char *s)
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
    return yapArrayPush(Y, p, s);
}

yOperand yapArrayPushUniqueToken(struct yapContext *Y, yapArray *p, struct yapToken *token)
{
    return yapArrayPushUniqueString(Y, p, yapTokenToString(Y, token));
}

yOperand yapArrayPush(struct yapContext *Y, yapArray *p, void *v)
{
    if(p->count == p->capacity)
    {
        int newSize = (p->capacity) ? p->capacity * 2 : 2;
#ifndef YAP_TRACE_MEMORY_STATS_ONLY
        yapTraceMem(("                                     "
                     "ARRAYPUSH %p resize %p [%d] -> [%d]\n", p, p->data, p->capacity, newSize));
#endif
        p->capacity = newSize;
        p->data = yapRealloc(p->data, p->capacity * sizeof(char **));
    }
    p->data[p->count++] = v;
    return (yOperand)(p->count - 1);
}

void *yapArrayPop(struct yapContext *Y, yapArray *p)
{
    if(!p->count)
    {
        return NULL;
    }
    return p->data[--p->count];
}

void *yapArrayTop(struct yapContext *Y, yapArray *p)
{
    if(!p->count)
    {
        return NULL;
    }
    return p->data[p->count - 1];
}

yU32 yapArrayCount(struct yapContext *Y, yapArray *p)
{
    return p->count;
}

void yapArrayClear(struct yapContext *Y, yapArray *p, yapDestroyCB cb)
{
    if(cb)
    {
        int i;
        for(i = 0; i < p->count; i++)
        {
            cb(Y, p->data[i]);
        }
    }
    p->count = 0;
    if(p->data)
    {
        yapFree(p->data);
    }
    p->data = NULL;
    p->capacity = 0;
}

void yapArrayClearP1(struct yapContext *Y, yapArray *p, yapDestroyCB1 cb, void *arg1)
{
    if(cb)
    {
        int i;
        for(i = 0; i < p->count; i++)
        {
            cb(Y, arg1, p->data[i]);
        }
    }
    yapArrayClear(Y, p, NULL);
}

void yapArrayDestroy(struct yapContext *Y, yapArray *p, yapDestroyCB cb)
{
    yapArrayClear(Y, p, cb);
    yapFree(p);
}
