#include "yapTypes.h"
#include "yapArray.h"
#include "yapContext.h"

#include <string.h>

// ------------------------------------------------------------------------------------------------
// Constants

#define DYNAMIC_ARRAY_INITIAL_SIZE 2

// ------------------------------------------------------------------------------------------------
// Internal structures

typedef struct yap2Array
{
    char **values;
    ySize size;
    ySize capacity;
} yap2Array;

// ------------------------------------------------------------------------------------------------
// Internal helper functions

// workhorse function that does all of the allocation and copying
static yap2Array *yap2ArrayChangeCapacity(struct yapContext *Y, ySize newCapacity, char ***prevptr)
{
    yap2Array *newArray;
    yap2Array *prevArray = NULL;
    if(prevptr && *prevptr)
    {
        prevArray = (yap2Array *)((char *)(*prevptr) - sizeof(yap2Array));
        if(newCapacity == prevArray->capacity)
            return prevArray;
    }

    newArray = (yap2Array *)yapAlloc(sizeof(yap2Array) + (sizeof(char*) * newCapacity));
    newArray->capacity = newCapacity;
    newArray->values = (char **)(((char *)newArray) + sizeof(yap2Array));
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

// finds / lazily creates a yap2Array from a regular ptr**
static yap2Array *yap2ArrayGet(struct yapContext *Y, char ***daptr, yBool autoCreate)
{
    yap2Array *da = NULL;
    if(daptr && *daptr)
    {
        // Move backwards one struct's worth (in bytes) to find the actual struct
        da = (yap2Array *)((char *)(*daptr) - sizeof(yap2Array));
    }
    else
    {
        if(autoCreate)
        {
            // Create a new dynamic array
            da = yap2ArrayChangeCapacity(Y, DYNAMIC_ARRAY_INITIAL_SIZE, daptr);
        }
    }
    return da;
}

// this assumes you've already destroyed any soon-to-be orphaned values at the end
static void yap2ArrayChangeSize(struct yapContext *Y, char ***daptr, ySize newSize)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 1);
    if(da->size == newSize)
        return;

    if(newSize > da->capacity)
    {
        da = yap2ArrayChangeCapacity(Y, newSize, daptr);
    }
    if(newSize > da->size)
    {
        memset(da->values + da->size, 0, sizeof(char*) * (newSize - da->size));
    }
    da->size = newSize;
}

// calls yap2ArrayChangeCapacity in preparation for new data, if necessary
static yap2Array *daMakeRoom(struct yapContext *Y, char ***daptr, int incomingCount)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 1);
    int capacityNeeded = da->size + incomingCount;
    int newCapacity = da->capacity;
    while(newCapacity < capacityNeeded)
        newCapacity *= 2; // is this dumb?
    if(newCapacity != da->capacity)
    {
        da = yap2ArrayChangeCapacity(Y, newCapacity, daptr);
    }
    return da;
}

// clears [start, (end-1)]
static void yap2ArrayClearRange(struct yapContext *Y, yap2Array *da, int start, int end, void * destroyFunc)
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

static void yap2ArrayClearRangeP1(struct yapContext *Y, yap2Array *da, int start, int end, void * destroyFunc, void *p1)
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

void yap2ArrayCreate(struct yapContext *Y, void *daptr)
{
    yap2ArrayGet(Y, daptr, 1);
}

void yap2ArrayDestroy(struct yapContext *Y, void *daptr, void * destroyFunc)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 0);
    if(da)
    {
        yap2ArrayClear(Y, daptr, destroyFunc);
        yapFree(da);
        *((char ***)daptr) = NULL;
    }
}

void yap2ArrayDestroyStrings(struct yapContext *Y, void *daptr)
{
    //yap2ArrayDestroy(Y, daptr, dsDestroyIndirect);
}

void yap2ArrayDestroyP1(struct yapContext *Y, void *daptr, void * destroyFunc, void *p1)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 0);
    if(da)
    {
        yap2ArrayClearP1(Y, daptr, destroyFunc, p1);
        yapFree(da);
        *((char ***)daptr) = NULL;
    }
}

void yap2ArrayClear(struct yapContext *Y, void *daptr, void * destroyFunc)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 0);
    if(da)
    {
        yap2ArrayClearRange(Y, da, 0, da->size, destroyFunc);
        da->size = 0;
    }
}

void yap2ArrayClearP1(struct yapContext *Y, void *daptr, void * destroyFunc, void *p1)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 0);
    if(da)
    {
        yap2ArrayClearRangeP1(Y, da, 0, da->size, destroyFunc, p1);
        da->size = 0;
    }
}

void yap2ArrayClearStrings(struct yapContext *Y, void *daptr)
{
    //yap2ArrayClear(Y, daptr, dsDestroyIndirect);
}

// ------------------------------------------------------------------------------------------------
// Front/back manipulation

// aka "pop front"
void *yap2ArrayShift(struct yapContext *Y, void *daptr)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 0);
    if(da && da->size > 0)
    {
        void *ret = da->values[0];
        memmove(da->values, da->values + 1, sizeof(char*) * da->size);
        --da->size;
        return ret;
    }
    return NULL;
}

void yap2ArrayUnshift(struct yapContext *Y, void *daptr, void *p)
{
    yap2Array *da = daMakeRoom(Y, daptr, 1);
    if(da->size > 0)
    {
        memmove(da->values + 1, da->values, sizeof(char*) * da->size);
    }
    da->values[0] = p;
    da->size++;
}

ySize yap2ArrayPush(struct yapContext *Y, void *daptr, void *entry)
{
    yap2Array *da = daMakeRoom(Y, daptr, 1);
    da->values[da->size++] = entry;
    return da->size - 1;
}

ySize yap2ArrayPushUniqueString(struct yapContext *Y, void *daptr, char *s)
{
    yap2Array *da = yap2ArrayGet(Y, daptr, yTrue);
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
    return yap2ArrayPush(Y, daptr, s);
}

void *yap2ArrayTop(struct yapContext *Y, void *daptr)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 0);
    if(da && (da->size > 0))
    {
        return da->values[da->size - 1];
    }
    return NULL;
}

void *yap2ArrayPop(struct yapContext *Y, void *daptr)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 0);
    if(da && (da->size > 0))
    {
        return da->values[--da->size];
    }
    return NULL;
}

// ------------------------------------------------------------------------------------------------
// Random access manipulation

void yap2ArrayInsert(struct yapContext *Y, void *daptr, ySize index, void *p)
{
    yap2Array *da = daMakeRoom(Y, daptr, 1);
    if((index < 0) || (!da->size) || (index >= da->size))
    {
        yap2ArrayPush(Y, daptr, p);
    }
    else
    {
        memmove(da->values + index + 1, da->values + index, sizeof(char*) * (da->size - index));
        da->values[index] = p;
        ++da->size;
    }
}

void yap2ArrayErase(struct yapContext *Y, void *daptr, ySize index)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 0);
    if(!da)
        return;
    if((index < 0) || (!da->size) || (index >= da->size))
        return;

    memmove(da->values + index, da->values + index + 1, sizeof(char*) * (da->size - index));
    --da->size;
}

// ------------------------------------------------------------------------------------------------
// Size manipulation

void yap2ArraySetSize(struct yapContext *Y, void *daptr, ySize newSize, void * destroyFunc)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 1);
    yap2ArrayClearRange(Y, da, newSize, da->size, destroyFunc);
    yap2ArrayChangeSize(Y, daptr, newSize);
}

void yap2ArraySetSizeP1(struct yapContext *Y, void *daptr, ySize newSize, void * destroyFunc, void *p1)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 1);
    yap2ArrayClearRangeP1(Y, da, newSize, da->size, destroyFunc, p1);
    yap2ArrayChangeSize(Y, daptr, newSize);
}

ySize yap2ArraySize(struct yapContext *Y, void *daptr)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 0);
    if(da)
        return da->size;
    return 0;
}

void yap2ArraySetCapacity(struct yapContext *Y, void *daptr, ySize newCapacity, void * destroyFunc)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 1);
    yap2ArrayClearRange(Y, da, newCapacity, da->size, destroyFunc);
    yap2ArrayChangeCapacity(Y, newCapacity, daptr);
}

void yap2ArraySetCapacityP1(struct yapContext *Y, void *daptr, ySize newCapacity, void * destroyFunc, void *p1)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 1);
    yap2ArrayClearRangeP1(Y, da, newCapacity, da->size, destroyFunc, p1);
    yap2ArrayChangeCapacity(Y, newCapacity, daptr);
}

ySize yap2ArrayCapacity(struct yapContext *Y, void *daptr)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 0);
    if(da)
        return da->capacity;
    return 0;
}

void yap2ArraySquash(struct yapContext *Y, void *daptr)
{
    yap2Array *da = yap2ArrayGet(Y, (char ***)daptr, 0);
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
