#include "yapTypes.h"
#include "yapArray.h"
#include "yapContext.h"

#include <string.h>

// ------------------------------------------------------------------------------------------------
// Constants

#define DYNAMIC_ARRAY_INITIAL_SIZE 2

// ------------------------------------------------------------------------------------------------
// Internal structures

typedef struct yapArray
{
    char **values;
    ySize size;
    ySize capacity;
} yapArray;

typedef struct yap32Array
{
    yU32 *values;
    ySize size;
    ySize capacity;
} yap32Array;

// ------------------------------------------------------------------------------------------------
// Internal helper functions

// workhorse function that does all of the allocation and copying
static yapArray *yapArrayChangeCapacity(struct yapContext *Y, ySize newCapacity, char ***prevptr)
{
    yapArray *newArray;
    yapArray *prevArray = NULL;
    if(prevptr && *prevptr)
    {
        prevArray = (yapArray *)((char *)(*prevptr) - sizeof(yapArray));
        if(newCapacity == prevArray->capacity)
            return prevArray;
    }

    newArray = (yapArray *)yapAlloc(sizeof(yapArray) + (sizeof(char*) * newCapacity));
    newArray->capacity = newCapacity;
    newArray->values = (char **)(((char *)newArray) + sizeof(yapArray));
    if(prevptr)
    {
        if(prevArray)
        {
            int copyCount = prevArray->size;
            if(copyCount > newArray->capacity)
                copyCount = newArray->capacity;
            memcpy(newArray->values, prevArray->values, sizeof(char*) * copyCount);
            newArray->size = copyCount;
            yapFree(prevArray);
        }
        *prevptr = newArray->values;
    }
    return newArray;
}

// finds / lazily creates a yapArray from a regular ptr**
static yapArray *yapArrayGet(struct yapContext *Y, char ***daptr, yBool autoCreate)
{
    yapArray *da = NULL;
    if(daptr && *daptr)
    {
        // Move backwards one struct's worth (in bytes) to find the actual struct
        da = (yapArray *)((char *)(*daptr) - sizeof(yapArray));
    }
    else
    {
        if(autoCreate)
        {
            // Create a new dynamic array
            da = yapArrayChangeCapacity(Y, DYNAMIC_ARRAY_INITIAL_SIZE, daptr);
        }
    }
    return da;
}

// this assumes you've already destroyed any soon-to-be orphaned values at the end
static void yapArrayChangeSize(struct yapContext *Y, char ***daptr, ySize newSize)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 1);
    if(da->size == newSize)
        return;

    if(newSize > da->capacity)
    {
        da = yapArrayChangeCapacity(Y, newSize, daptr);
    }
    if(newSize > da->size)
    {
        memset(da->values + da->size, 0, sizeof(char*) * (newSize - da->size));
    }
    da->size = newSize;
}

// calls yapArrayChangeCapacity in preparation for new data, if necessary
static yapArray *yapArrayMakeRoom(struct yapContext *Y, char ***daptr, int incomingCount)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 1);
    int capacityNeeded = da->size + incomingCount;
    int newCapacity = da->capacity;
    while(newCapacity < capacityNeeded)
        newCapacity *= 2; // is this dumb?
    if(newCapacity != da->capacity)
    {
        da = yapArrayChangeCapacity(Y, newCapacity, daptr);
    }
    return da;
}

// clears [start, (end-1)]
static void yapArrayClearRange(struct yapContext *Y, yapArray *da, int start, int end, void * destroyFunc)
{
    yapDestroyCB func = destroyFunc;
    if(func)
    {
        int i;
        for(i = start; i < end; ++i)
        {
            if(da->values[i])
                func(Y, da->values[i]);
        }
    }
}

static void yapArrayClearRangeP1(struct yapContext *Y, yapArray *da, int start, int end, void * destroyFunc, void *p1)
{
    yapDestroyCB1 func = destroyFunc;
    if(func)
    {
        int i;
        for(i = start; i < end; ++i)
        {
            if(da->values[i])
                func(Y, p1, da->values[i]);
        }
    }
}

// ------------------------------------------------------------------------------------------------
// creation / destruction / cleanup

void yapArrayCreate(struct yapContext *Y, void *daptr)
{
    yapArrayGet(Y, daptr, 1);
}

void yapArrayDestroy(struct yapContext *Y, void *daptr, void * destroyFunc)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 0);
    if(da)
    {
        yapArrayClear(Y, daptr, destroyFunc);
        yapFree(da);
        *((char ***)daptr) = NULL;
    }
}

void yapArrayDestroyStrings(struct yapContext *Y, void *daptr)
{
    //yapArrayDestroy(Y, daptr, dsDestroyIndirect);
}

void yapArrayDestroyP1(struct yapContext *Y, void *daptr, void * destroyFunc, void *p1)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 0);
    if(da)
    {
        yapArrayClearP1(Y, daptr, destroyFunc, p1);
        yapFree(da);
        *((char ***)daptr) = NULL;
    }
}

void yapArrayClear(struct yapContext *Y, void *daptr, void * destroyFunc)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 0);
    if(da)
    {
        yapArrayClearRange(Y, da, 0, da->size, destroyFunc);
        da->size = 0;
    }
}

void yapArrayClearP1(struct yapContext *Y, void *daptr, void * destroyFunc, void *p1)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 0);
    if(da)
    {
        yapArrayClearRangeP1(Y, da, 0, da->size, destroyFunc, p1);
        da->size = 0;
    }
}

void yapArrayClearStrings(struct yapContext *Y, void *daptr)
{
    //yapArrayClear(Y, daptr, dsDestroyIndirect);
}

// ------------------------------------------------------------------------------------------------
// Front/back manipulation

// aka "pop front"
void *yapArrayShift(struct yapContext *Y, void *daptr)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 0);
    if(da && da->size > 0)
    {
        void *ret = da->values[0];
        memmove(da->values, da->values + 1, sizeof(char*) * da->size);
        --da->size;
        return ret;
    }
    return NULL;
}

void yapArrayUnshift(struct yapContext *Y, void *daptr, void *p)
{
    yapArray *da = yapArrayMakeRoom(Y, daptr, 1);
    if(da->size > 0)
    {
        memmove(da->values + 1, da->values, sizeof(char*) * da->size);
    }
    da->values[0] = p;
    da->size++;
}

ySize yapArrayPush(struct yapContext *Y, void *daptr, void *entry)
{
    yapArray *da = yapArrayMakeRoom(Y, daptr, 1);
    da->values[da->size++] = entry;
    return da->size - 1;
}

ySize yapArrayPushUniqueString(struct yapContext *Y, void *daptr, char *s)
{
    yapArray *da = yapArrayGet(Y, daptr, yTrue);
    ySize i;
    for(i = 0; i < da->size; i++)
    {
        const char *v = (const char *)da->values[i];
        if(!strcmp(s, v))
        {
            yapFree(s);
            return i;
        }
    }
    return yapArrayPush(Y, daptr, s);
}

void *yapArrayTop(struct yapContext *Y, void *daptr)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 0);
    if(da && (da->size > 0))
    {
        return da->values[da->size - 1];
    }
    return NULL;
}

void *yapArrayPop(struct yapContext *Y, void *daptr)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 0);
    if(da && (da->size > 0))
    {
        return da->values[--da->size];
    }
    return NULL;
}

// ------------------------------------------------------------------------------------------------
// Random access manipulation

void yapArrayInsert(struct yapContext *Y, void *daptr, ySize index, void *p)
{
    yapArray *da = yapArrayMakeRoom(Y, daptr, 1);
    if((index < 0) || (!da->size) || (index >= da->size))
    {
        yapArrayPush(Y, daptr, p);
    }
    else
    {
        memmove(da->values + index + 1, da->values + index, sizeof(char*) * (da->size - index));
        da->values[index] = p;
        ++da->size;
    }
}

void yapArrayErase(struct yapContext *Y, void *daptr, ySize index)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 0);
    if(!da)
        return;
    if((index < 0) || (!da->size) || (index >= da->size))
        return;

    memmove(da->values + index, da->values + index + 1, sizeof(char*) * (da->size - index));
    --da->size;
}

// ------------------------------------------------------------------------------------------------
// Size manipulation

void yapArraySetSize(struct yapContext *Y, void *daptr, ySize newSize, void * destroyFunc)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 1);
    yapArrayClearRange(Y, da, newSize, da->size, destroyFunc);
    yapArrayChangeSize(Y, daptr, newSize);
}

void yapArraySetSizeP1(struct yapContext *Y, void *daptr, ySize newSize, void * destroyFunc, void *p1)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 1);
    yapArrayClearRangeP1(Y, da, newSize, da->size, destroyFunc, p1);
    yapArrayChangeSize(Y, daptr, newSize);
}

ySize yapArraySize(struct yapContext *Y, void *daptr)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 0);
    if(da)
        return da->size;
    return 0;
}

void yapArraySetCapacity(struct yapContext *Y, void *daptr, ySize newCapacity, void * destroyFunc)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 1);
    yapArrayClearRange(Y, da, newCapacity, da->size, destroyFunc);
    yapArrayChangeCapacity(Y, newCapacity, daptr);
}

void yapArraySetCapacityP1(struct yapContext *Y, void *daptr, ySize newCapacity, void * destroyFunc, void *p1)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 1);
    yapArrayClearRangeP1(Y, da, newCapacity, da->size, destroyFunc, p1);
    yapArrayChangeCapacity(Y, newCapacity, daptr);
}

ySize yapArrayCapacity(struct yapContext *Y, void *daptr)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 0);
    if(da)
        return da->capacity;
    return 0;
}

void yapArraySquash(struct yapContext *Y, void *daptr)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 0);
    if(da)
    {
        int head = 0;
        int tail = 0;
        for( ; tail < da->size ; tail++)
        {
            if(da->values[tail] != NULL)
            {
                da->values[head] = da->values[tail];
                head++;
            }
        }
        da->size = head;
    }
}

void yapArrayShrink(struct yapContext *Y, void *daptr, int n, yapDestroyCB cb)
{
    yapArray *da = yapArrayGet(Y, (char ***)daptr, 0);
    if(!da || !da->size)
    {
        return;
    }

    while(da->size > n)
    {
        if(cb)
        {
            cb(Y, da->values[da->size - 1]);
        }
        --da->size;
    }
}

// ------------------------------------------------------------------------------------------------
// yap32Array

// workhorse function that does all of the allocation and copying
static yap32Array *yap32ArrayChangeCapacity(struct yapContext *Y, ySize newCapacity, yU32 **prevptr)
{
    yap32Array *newArray;
    yap32Array *prevArray = NULL;
    if(prevptr && *prevptr)
    {
        prevArray = (yap32Array *)((char *)(*prevptr) - sizeof(yap32Array));
        if(newCapacity == prevArray->capacity)
            return prevArray;
    }

    newArray = (yap32Array *)yapAlloc(sizeof(yap32Array) + (sizeof(yU32) * newCapacity));
    newArray->capacity = newCapacity;
    newArray->values = (yU32 *)(((char *)newArray) + sizeof(yap32Array));
    if(prevptr)
    {
        if(prevArray)
        {
            int copyCount = prevArray->size;
            if(copyCount > newArray->capacity)
                copyCount = newArray->capacity;
            memcpy(newArray->values, prevArray->values, sizeof(yU32) * copyCount);
            newArray->size = copyCount;
            yapFree(prevArray);
        }
        *prevptr = newArray->values;
    }
    return newArray;
}

// finds / lazily creates a yap32Array from a regular ptr**
static yap32Array *yap32ArrayGet(struct yapContext *Y, yU32 **daptr, yBool autoCreate)
{
    yap32Array *da = NULL;
    if(daptr && *daptr)
    {
        // Move backwards one struct's worth (in bytes) to find the actual struct
        da = (yap32Array *)((char *)(*daptr) - sizeof(yap32Array));
    }
    else
    {
        if(autoCreate)
        {
            // Create a new dynamic array
            da = yap32ArrayChangeCapacity(Y, DYNAMIC_ARRAY_INITIAL_SIZE, daptr);
        }
    }
    return da;
}

// calls yapArrayChangeCapacity in preparation for new data, if necessary
static yap32Array *yap32ArrayMakeRoom(struct yapContext *Y, yU32 **daptr, int incomingCount)
{
    yap32Array *da = yap32ArrayGet(Y, daptr, 1);
    int capacityNeeded = da->size + incomingCount;
    int newCapacity = da->capacity;
    while(newCapacity < capacityNeeded)
        newCapacity *= 2; // is this dumb?
    if(newCapacity != da->capacity)
    {
        da = yap32ArrayChangeCapacity(Y, newCapacity, daptr);
    }
    return da;
}

ySize yap32ArrayPushUnique(struct yapContext *Y, void *daptr, yU32 *v)
{
    int i;
    yap32Array *da = yap32ArrayGet(Y, daptr, yTrue);
    for(i = 0; i < da->size; ++i)
    {
        if(da->values[i] == *v)
        {
            return i;
        }
    }
    return yap32ArrayPush(Y, daptr, v);
}

ySize yap32ArrayPush(struct yapContext *Y, void *daptr, yU32 *v)
{
    yap32Array *da = yap32ArrayMakeRoom(Y, daptr, 1);
    da->values[da->size++] = *v;
    return da->size - 1;
}

void yap32ArrayClear(struct yapContext *Y, void *daptr)
{
    yap32Array *da = yap32ArrayGet(Y, (yU32 **)daptr, 0);
    if(da)
    {
        da->size = 0;
    }
}

void yap32ArrayDestroy(struct yapContext *Y, void *daptr)
{
    yap32Array *da = yap32ArrayGet(Y, (yU32 **)daptr, 0);
    if(da)
    {
        yapFree(da);
        *((yU32 **)daptr) = NULL;
    }
}

ySize yap32ArraySize(struct yapContext *Y, void *daptr)
{
    yap32Array *da = yap32ArrayGet(Y, (yU32 **)daptr, 0);
    if(da)
        return da->size;
    return 0;
}

