#include "ekTypes.h"
#include "ekArray.h"
#include "ekContext.h"

#include <string.h>

// ------------------------------------------------------------------------------------------------
// Constants

#define DYNAMIC_ARRAY_INITIAL_SIZE 2

// ------------------------------------------------------------------------------------------------
// Internal structures

typedef struct ekArray
{
    char **values;
    ekSize size;
    ekSize capacity;
} ekArray;

typedef struct ek32Array
{
    ekU32 *values;
    ekSize size;
    ekSize capacity;
} ek32Array;

// ------------------------------------------------------------------------------------------------
// Internal helper functions

// workhorse function that does all of the allocation and copying
static ekArray *ekArrayChangeCapacity(struct ekContext *Y, ekSize newCapacity, char ***prevptr)
{
    ekArray *newArray;
    ekArray *prevArray = NULL;
    if(prevptr && *prevptr)
    {
        prevArray = (ekArray *)((char *)(*prevptr) - sizeof(ekArray));
        if(newCapacity == prevArray->capacity)
        {
            return prevArray;
        }
    }

    newArray = (ekArray *)ekAlloc(sizeof(ekArray) + (sizeof(char *) * newCapacity));
    newArray->capacity = newCapacity;
    newArray->values = (char **)(((char *)newArray) + sizeof(ekArray));
    if(prevptr)
    {
        if(prevArray)
        {
            int copyCount = prevArray->size;
            if(copyCount > newArray->capacity)
            {
                copyCount = newArray->capacity;
            }
            memcpy(newArray->values, prevArray->values, sizeof(char *) * copyCount);
            newArray->size = copyCount;
            ekFree(prevArray);
        }
        *prevptr = newArray->values;
    }
    return newArray;
}

// finds / lazily creates a ekArray from a regular ptr**
static ekArray *ekArrayGet(struct ekContext *Y, char ***daptr, ekBool autoCreate)
{
    ekArray *da = NULL;
    if(daptr && *daptr)
    {
        // Move backwards one struct's worth (in bytes) to find the actual struct
        da = (ekArray *)((char *)(*daptr) - sizeof(ekArray));
    }
    else
    {
        if(autoCreate)
        {
            // Create a new dynamic array
            da = ekArrayChangeCapacity(Y, DYNAMIC_ARRAY_INITIAL_SIZE, daptr);
        }
    }
    return da;
}

// this assumes you've already destroyed any soon-to-be orphaned values at the end
static void ekArrayChangeSize(struct ekContext *Y, char ***daptr, ekSize newSize)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 1);
    if(da->size == newSize)
    {
        return;
    }

    if(newSize > da->capacity)
    {
        da = ekArrayChangeCapacity(Y, newSize, daptr);
    }
    if(newSize > da->size)
    {
        memset(da->values + da->size, 0, sizeof(char *) * (newSize - da->size));
    }
    da->size = newSize;
}

// calls ekArrayChangeCapacity in preparation for new data, if necessary
static ekArray *ekArrayMakeRoom(struct ekContext *Y, char ***daptr, int incomingCount)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 1);
    int capacityNeeded = da->size + incomingCount;
    int newCapacity = da->capacity;
    while(newCapacity < capacityNeeded)
    {
        newCapacity *= 2;    // is this dumb?
    }
    if(newCapacity != da->capacity)
    {
        da = ekArrayChangeCapacity(Y, newCapacity, daptr);
    }
    return da;
}

// clears [start, (end-1)]
static void ekArrayClearRange(struct ekContext *Y, ekArray *da, int start, int end, void *destroyFunc)
{
    ekDestroyCB func = destroyFunc;
    if(func)
    {
        int i;
        for(i = start; i < end; ++i)
        {
            if(da->values[i])
            {
                func(Y, da->values[i]);
            }
        }
    }
}

static void ekArrayClearRangeP1(struct ekContext *Y, ekArray *da, int start, int end, void *destroyFunc, void *p1)
{
    ekDestroyCB1 func = destroyFunc;
    if(func)
    {
        int i;
        for(i = start; i < end; ++i)
        {
            if(da->values[i])
            {
                func(Y, p1, da->values[i]);
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------
// creation / destruction / cleanup

void ekArrayCreate(struct ekContext *Y, void *daptr)
{
    ekArrayGet(Y, daptr, 1);
}

void ekArrayDestroy(struct ekContext *Y, void *daptr, void *destroyFunc)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 0);
    if(da)
    {
        ekArrayClear(Y, daptr, destroyFunc);
        ekFree(da);
        *((char ** *)daptr) = NULL;
    }
}

void ekArrayDestroyStrings(struct ekContext *Y, void *daptr)
{
    //ekArrayDestroy(Y, daptr, dsDestroyIndirect);
}

void ekArrayDestroyP1(struct ekContext *Y, void *daptr, void *destroyFunc, void *p1)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 0);
    if(da)
    {
        ekArrayClearP1(Y, daptr, destroyFunc, p1);
        ekFree(da);
        *((char ** *)daptr) = NULL;
    }
}

void ekArrayClear(struct ekContext *Y, void *daptr, void *destroyFunc)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 0);
    if(da)
    {
        ekArrayClearRange(Y, da, 0, da->size, destroyFunc);
        da->size = 0;
    }
}

void ekArrayClearP1(struct ekContext *Y, void *daptr, void *destroyFunc, void *p1)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 0);
    if(da)
    {
        ekArrayClearRangeP1(Y, da, 0, da->size, destroyFunc, p1);
        da->size = 0;
    }
}

void ekArrayClearStrings(struct ekContext *Y, void *daptr)
{
    //ekArrayClear(Y, daptr, dsDestroyIndirect);
}

// ------------------------------------------------------------------------------------------------
// Front/back manipulation

// aka "pop front"
void *ekArrayShift(struct ekContext *Y, void *daptr)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 0);
    if(da && da->size > 0)
    {
        void *ret = da->values[0];
        memmove(da->values, da->values + 1, sizeof(char *) * da->size);
        --da->size;
        return ret;
    }
    return NULL;
}

void ekArrayUnshift(struct ekContext *Y, void *daptr, void *p)
{
    ekArray *da = ekArrayMakeRoom(Y, daptr, 1);
    if(da->size > 0)
    {
        memmove(da->values + 1, da->values, sizeof(char *) * da->size);
    }
    da->values[0] = p;
    da->size++;
}

ekSize ekArrayPush(struct ekContext *Y, void *daptr, void *entry)
{
    ekArray *da = ekArrayMakeRoom(Y, daptr, 1);
    da->values[da->size++] = entry;
    return da->size - 1;
}

ekSize ekArrayPushUniqueString(struct ekContext *Y, void *daptr, char *s)
{
    ekArray *da = ekArrayGet(Y, daptr, ekTrue);
    ekSize i;
    for(i = 0; i < da->size; i++)
    {
        const char *v = (const char *)da->values[i];
        if(!strcmp(s, v))
        {
            ekFree(s);
            return i;
        }
    }
    return ekArrayPush(Y, daptr, s);
}

void *ekArrayTop(struct ekContext *Y, void *daptr)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 0);
    if(da && (da->size > 0))
    {
        return da->values[da->size - 1];
    }
    return NULL;
}

void *ekArrayPop(struct ekContext *Y, void *daptr)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 0);
    if(da && (da->size > 0))
    {
        return da->values[--da->size];
    }
    return NULL;
}

// ------------------------------------------------------------------------------------------------
// Random access manipulation

void ekArrayInsert(struct ekContext *Y, void *daptr, ekSize index, void *p)
{
    ekArray *da = ekArrayMakeRoom(Y, daptr, 1);
    if((index < 0) || (!da->size) || (index >= da->size))
    {
        ekArrayPush(Y, daptr, p);
    }
    else
    {
        memmove(da->values + index + 1, da->values + index, sizeof(char *) * (da->size - index));
        da->values[index] = p;
        ++da->size;
    }
}

void ekArrayErase(struct ekContext *Y, void *daptr, ekSize index)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 0);
    if(!da)
    {
        return;
    }
    if((index < 0) || (!da->size) || (index >= da->size))
    {
        return;
    }

    memmove(da->values + index, da->values + index + 1, sizeof(char *) * (da->size - index));
    --da->size;
}

// ------------------------------------------------------------------------------------------------
// Size manipulation

void ekArraySetSize(struct ekContext *Y, void *daptr, ekSize newSize, void *destroyFunc)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 1);
    ekArrayClearRange(Y, da, newSize, da->size, destroyFunc);
    ekArrayChangeSize(Y, daptr, newSize);
}

void ekArraySetSizeP1(struct ekContext *Y, void *daptr, ekSize newSize, void *destroyFunc, void *p1)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 1);
    ekArrayClearRangeP1(Y, da, newSize, da->size, destroyFunc, p1);
    ekArrayChangeSize(Y, daptr, newSize);
}

ekSize ekArraekSize(struct ekContext *Y, void *daptr)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 0);
    if(da)
    {
        return da->size;
    }
    return 0;
}

void ekArraySetCapacity(struct ekContext *Y, void *daptr, ekSize newCapacity, void *destroyFunc)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 1);
    ekArrayClearRange(Y, da, newCapacity, da->size, destroyFunc);
    ekArrayChangeCapacity(Y, newCapacity, daptr);
}

void ekArraySetCapacityP1(struct ekContext *Y, void *daptr, ekSize newCapacity, void *destroyFunc, void *p1)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 1);
    ekArrayClearRangeP1(Y, da, newCapacity, da->size, destroyFunc, p1);
    ekArrayChangeCapacity(Y, newCapacity, daptr);
}

ekSize ekArrayCapacity(struct ekContext *Y, void *daptr)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 0);
    if(da)
    {
        return da->capacity;
    }
    return 0;
}

void ekArraySquash(struct ekContext *Y, void *daptr)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 0);
    if(da)
    {
        int head = 0;
        int tail = 0;
        for(; tail < da->size ; tail++)
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

void ekArrayShrink(struct ekContext *Y, void *daptr, int n, ekDestroyCB cb)
{
    ekArray *da = ekArrayGet(Y, (char ** *)daptr, 0);
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
// ek32Array

// workhorse function that does all of the allocation and copying
static ek32Array *ek32ArrayChangeCapacity(struct ekContext *Y, ekSize newCapacity, ekU32 **prevptr)
{
    ek32Array *newArray;
    ek32Array *prevArray = NULL;
    if(prevptr && *prevptr)
    {
        prevArray = (ek32Array *)((char *)(*prevptr) - sizeof(ek32Array));
        if(newCapacity == prevArray->capacity)
        {
            return prevArray;
        }
    }

    newArray = (ek32Array *)ekAlloc(sizeof(ek32Array) + (sizeof(ekU32) * newCapacity));
    newArray->capacity = newCapacity;
    newArray->values = (ekU32 *)(((char *)newArray) + sizeof(ek32Array));
    if(prevptr)
    {
        if(prevArray)
        {
            int copyCount = prevArray->size;
            if(copyCount > newArray->capacity)
            {
                copyCount = newArray->capacity;
            }
            memcpy(newArray->values, prevArray->values, sizeof(ekU32) * copyCount);
            newArray->size = copyCount;
            ekFree(prevArray);
        }
        *prevptr = newArray->values;
    }
    return newArray;
}

// finds / lazily creates a ek32Array from a regular ptr**
static ek32Array *ek32ArrayGet(struct ekContext *Y, ekU32 **daptr, ekBool autoCreate)
{
    ek32Array *da = NULL;
    if(daptr && *daptr)
    {
        // Move backwards one struct's worth (in bytes) to find the actual struct
        da = (ek32Array *)((char *)(*daptr) - sizeof(ek32Array));
    }
    else
    {
        if(autoCreate)
        {
            // Create a new dynamic array
            da = ek32ArrayChangeCapacity(Y, DYNAMIC_ARRAY_INITIAL_SIZE, daptr);
        }
    }
    return da;
}

// calls ekArrayChangeCapacity in preparation for new data, if necessary
static ek32Array *ek32ArrayMakeRoom(struct ekContext *Y, ekU32 **daptr, int incomingCount)
{
    ek32Array *da = ek32ArrayGet(Y, daptr, 1);
    int capacityNeeded = da->size + incomingCount;
    int newCapacity = da->capacity;
    while(newCapacity < capacityNeeded)
    {
        newCapacity *= 2;    // is this dumb?
    }
    if(newCapacity != da->capacity)
    {
        da = ek32ArrayChangeCapacity(Y, newCapacity, daptr);
    }
    return da;
}

ekSize ek32ArrayPushUnique(struct ekContext *Y, void *daptr, ekU32 *v)
{
    int i;
    ek32Array *da = ek32ArrayGet(Y, daptr, ekTrue);
    for(i = 0; i < da->size; ++i)
    {
        if(da->values[i] == *v)
        {
            return i;
        }
    }
    return ek32ArrayPush(Y, daptr, v);
}

ekSize ek32ArrayPush(struct ekContext *Y, void *daptr, ekU32 *v)
{
    ek32Array *da = ek32ArrayMakeRoom(Y, daptr, 1);
    da->values[da->size++] = *v;
    return da->size - 1;
}

void ek32ArrayClear(struct ekContext *Y, void *daptr)
{
    ek32Array *da = ek32ArrayGet(Y, (ekU32 **)daptr, 0);
    if(da)
    {
        da->size = 0;
    }
}

void ek32ArrayDestroy(struct ekContext *Y, void *daptr)
{
    ek32Array *da = ek32ArrayGet(Y, (ekU32 **)daptr, 0);
    if(da)
    {
        ekFree(da);
        *((ekU32 **)daptr) = NULL;
    }
}

ekSize ek32ArraekSize(struct ekContext *Y, void *daptr)
{
    ek32Array *da = ek32ArrayGet(Y, (ekU32 **)daptr, 0);
    if(da)
    {
        return da->size;
    }
    return 0;
}

