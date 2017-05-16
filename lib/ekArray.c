// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

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
    char ** values;
    ekSize size;
    ekSize capacity;
} ekArray;

typedef struct ek32Array
{
    ekU32 * values;
    ekSize size;
    ekSize capacity;
} ek32Array;

// ------------------------------------------------------------------------------------------------
// Internal helper functions

// workhorse function that does all of the allocation and copying
static ekArray * ekArrayChangeCapacity(struct ekContext * E, ekSize newCapacity, char *** prevptr)
{
    ekArray * newArray;
    ekArray * prevArray = NULL;
    if (prevptr && *prevptr) {
        prevArray = (ekArray *)((char *)(*prevptr) - sizeof(ekArray));
        if (newCapacity == prevArray->capacity) {
            return prevArray;
        }
    }

    newArray = (ekArray *)ekAlloc(sizeof(ekArray) + (sizeof(char *) * newCapacity));
    newArray->capacity = newCapacity;
    newArray->values = (char **)(((char *)newArray) + sizeof(ekArray));
    if (prevptr) {
        if (prevArray) {
            ekS32 copyCount = prevArray->size;
            if (copyCount > newArray->capacity) {
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
static ekArray * ekArrayGet(struct ekContext * E, char *** daptr, ekBool autoCreate)
{
    ekArray * da = NULL;
    if (daptr && *daptr) {
        // Move backwards one struct's worth (in bytes) to find the actual struct
        da = (ekArray *)((char *)(*daptr) - sizeof(ekArray));
    } else {
        if (autoCreate) {
            // Create a new dynamic array
            da = ekArrayChangeCapacity(E, DYNAMIC_ARRAY_INITIAL_SIZE, daptr);
        }
    }
    return da;
}

// this assumes you've already destroyed any soon-to-be orphaned values at the end
static void ekArrayChangeSize(struct ekContext * E, char *** daptr, ekSize newSize)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 1);
    if (da->size == newSize) {
        return;
    }

    if (newSize > da->capacity) {
        da = ekArrayChangeCapacity(E, newSize, daptr);
    }
    if (newSize > da->size) {
        memset(da->values + da->size, 0, sizeof(char *) * (newSize - da->size));
    }
    da->size = newSize;
}

// calls ekArrayChangeCapacity in preparation for new data, if necessary
static ekArray * ekArrayMakeRoom(struct ekContext * E, char *** daptr, ekS32 incomingCount)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 1);
    ekS32 capacityNeeded = da->size + incomingCount;
    ekS32 newCapacity = da->capacity;
    while (newCapacity < capacityNeeded) {
        newCapacity *= 2; // is this dumb?
    }
    if (newCapacity != da->capacity) {
        da = ekArrayChangeCapacity(E, newCapacity, daptr);
    }
    return da;
}

// clears [start, (end-1)]
static void ekArrayClearRange(struct ekContext * E, ekArray * da, ekS32 start, ekS32 end, void * destroyFunc)
{
    ekDestroyCB func = destroyFunc;
    if (func) {
        ekS32 i;
        for (i = start; i < end; ++i) {
            if (da->values[i]) {
                func(E, da->values[i]);
            }
        }
    }
}

static void ekArrayClearRangeP1(struct ekContext * E, ekArray * da, ekS32 start, ekS32 end, void * destroyFunc, void * p1)
{
    ekDestroyCB1 func = destroyFunc;
    if (func) {
        ekS32 i;
        for (i = start; i < end; ++i) {
            if (da->values[i]) {
                func(E, p1, da->values[i]);
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------
// creation / destruction / cleanup

void ekArrayCreate(struct ekContext * E, void * daptr)
{
    ekArrayGet(E, daptr, 1);
}

void ekArrayDestroy(struct ekContext * E, void * daptr, void * destroyFunc)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 0);
    if (da) {
        ekArrayClear(E, daptr, destroyFunc);
        ekFree(da);
        *((char ***)daptr) = NULL;
    }
}

void ekArrayDestroyStrings(struct ekContext * E, void * daptr)
{
    //ekArrayDestroy(E, daptr, dsDestroyIndirect);
}

void ekArrayDestroyP1(struct ekContext * E, void * daptr, void * destroyFunc, void * p1)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 0);
    if (da) {
        ekArrayClearP1(E, daptr, destroyFunc, p1);
        ekFree(da);
        *((char ***)daptr) = NULL;
    }
}

void ekArrayClear(struct ekContext * E, void * daptr, void * destroyFunc)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 0);
    if (da) {
        ekArrayClearRange(E, da, 0, da->size, destroyFunc);
        da->size = 0;
    }
}

void ekArrayClearP1(struct ekContext * E, void * daptr, void * destroyFunc, void * p1)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 0);
    if (da) {
        ekArrayClearRangeP1(E, da, 0, da->size, destroyFunc, p1);
        da->size = 0;
    }
}

void ekArrayClearStrings(struct ekContext * E, void * daptr)
{
    //ekArrayClear(E, daptr, dsDestroyIndirect);
}

// ------------------------------------------------------------------------------------------------
// Front/back manipulation

// aka "pop front"
void * ekArrayShift(struct ekContext * E, void * daptr)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 0);
    if (da && (da->size > 0)) {
        void * ret = da->values[0];
        memmove(da->values, da->values + 1, sizeof(char *) * da->size);
        --da->size;
        return ret;
    }
    return NULL;
}

void ekArrayUnshift(struct ekContext * E, void * daptr, void * p)
{
    ekArray * da = ekArrayMakeRoom(E, daptr, 1);
    if (da->size > 0) {
        memmove(da->values + 1, da->values, sizeof(char *) * da->size);
    }
    da->values[0] = p;
    da->size++;
}

ekSize ekArrayPush(struct ekContext * E, void * daptr, void * entry)
{
    ekArray * da = ekArrayMakeRoom(E, daptr, 1);
    da->values[da->size++] = entry;
    return da->size - 1;
}

ekSize ekArrayPushUniqueString(struct ekContext * E, void * daptr, char * s)
{
    ekArray * da = ekArrayGet(E, daptr, ekTrue);
    ekSize i;
    for (i = 0; i < da->size; i++) {
        const char * v = (const char *)da->values[i];
        if (!strcmp(s, v)) {
            ekFree(s);
            return i;
        }
    }
    return ekArrayPush(E, daptr, s);
}

void * ekArrayTop(struct ekContext * E, void * daptr)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 0);
    if (da && (da->size > 0)) {
        return da->values[da->size - 1];
    }
    return NULL;
}

void * ekArrayPop(struct ekContext * E, void * daptr)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 0);
    if (da && (da->size > 0)) {
        return da->values[--da->size];
    }
    return NULL;
}

// ------------------------------------------------------------------------------------------------
// Random access manipulation

void ekArrayInsert(struct ekContext * E, void * daptr, ekSize index, void * p)
{
    ekArray * da = ekArrayMakeRoom(E, daptr, 1);
    if ((!da->size) || (index >= da->size)) {
        ekArrayPush(E, daptr, p);
    } else {
        memmove(da->values + index + 1, da->values + index, sizeof(char *) * (da->size - index));
        da->values[index] = p;
        ++da->size;
    }
}

void ekArrayErase(struct ekContext * E, void * daptr, ekSize index)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 0);
    if (!da) {
        return;
    }
    if ((!da->size) || (index >= da->size)) {
        return;
    }

    memmove(da->values + index, da->values + index + 1, sizeof(char *) * (da->size - index));
    --da->size;
}

// ------------------------------------------------------------------------------------------------
// Size manipulation

void ekArraySetSize(struct ekContext * E, void * daptr, ekSize newSize, void * destroyFunc)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 1);
    ekArrayClearRange(E, da, newSize, da->size, destroyFunc);
    ekArrayChangeSize(E, daptr, newSize);
}

void ekArraySetSizeP1(struct ekContext * E, void * daptr, ekSize newSize, void * destroyFunc, void * p1)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 1);
    ekArrayClearRangeP1(E, da, newSize, da->size, destroyFunc, p1);
    ekArrayChangeSize(E, daptr, newSize);
}

ekSize ekArraySize(struct ekContext * E, void * daptr)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 0);
    if (da) {
        return da->size;
    }
    return 0;
}

void ekArraySetCapacity(struct ekContext * E, void * daptr, ekSize newCapacity, void * destroyFunc)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 1);
    ekArrayClearRange(E, da, newCapacity, da->size, destroyFunc);
    ekArrayChangeCapacity(E, newCapacity, daptr);
}

void ekArraySetCapacityP1(struct ekContext * E, void * daptr, ekSize newCapacity, void * destroyFunc, void * p1)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 1);
    ekArrayClearRangeP1(E, da, newCapacity, da->size, destroyFunc, p1);
    ekArrayChangeCapacity(E, newCapacity, daptr);
}

ekSize ekArrayCapacity(struct ekContext * E, void * daptr)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 0);
    if (da) {
        return da->capacity;
    }
    return 0;
}

void ekArraySquash(struct ekContext * E, void * daptr)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 0);
    if (da) {
        ekS32 head = 0;
        ekS32 tail = 0;
        for (; tail < da->size; tail++) {
            if (da->values[tail] != NULL) {
                da->values[head] = da->values[tail];
                head++;
            }
        }
        da->size = head;
    }
}

void ekArrayShrink(struct ekContext * E, void * daptr, ekS32 n, ekDestroyCB cb)
{
    ekArray * da = ekArrayGet(E, (char ***)daptr, 0);
    if (!da || !da->size) {
        return;
    }

    while (da->size > n) {
        if (cb) {
            cb(E, da->values[da->size - 1]);
        }
        --da->size;
    }
}

// ------------------------------------------------------------------------------------------------
// ek32Array

// workhorse function that does all of the allocation and copying
static ek32Array * ek32ArrayChangeCapacity(struct ekContext * E, ekSize newCapacity, ekU32 ** prevptr)
{
    ek32Array * newArray;
    ek32Array * prevArray = NULL;
    if (prevptr && *prevptr) {
        prevArray = (ek32Array *)((char *)(*prevptr) - sizeof(ek32Array));
        if (newCapacity == prevArray->capacity) {
            return prevArray;
        }
    }

    newArray = (ek32Array *)ekAlloc(sizeof(ek32Array) + (sizeof(ekU32) * newCapacity));
    newArray->capacity = newCapacity;
    newArray->values = (ekU32 *)(((char *)newArray) + sizeof(ek32Array));
    if (prevptr) {
        if (prevArray) {
            ekS32 copyCount = prevArray->size;
            if (copyCount > newArray->capacity) {
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
static ek32Array * ek32ArrayGet(struct ekContext * E, ekU32 ** daptr, ekBool autoCreate)
{
    ek32Array * da = NULL;
    if (daptr && *daptr) {
        // Move backwards one struct's worth (in bytes) to find the actual struct
        da = (ek32Array *)((char *)(*daptr) - sizeof(ek32Array));
    } else {
        if (autoCreate) {
            // Create a new dynamic array
            da = ek32ArrayChangeCapacity(E, DYNAMIC_ARRAY_INITIAL_SIZE, daptr);
        }
    }
    return da;
}

// calls ekArrayChangeCapacity in preparation for new data, if necessary
static ek32Array * ek32ArrayMakeRoom(struct ekContext * E, ekU32 ** daptr, ekS32 incomingCount)
{
    ek32Array * da = ek32ArrayGet(E, daptr, 1);
    ekS32 capacityNeeded = da->size + incomingCount;
    ekS32 newCapacity = da->capacity;
    while (newCapacity < capacityNeeded) {
        newCapacity *= 2; // is this dumb?
    }
    if (newCapacity != da->capacity) {
        da = ek32ArrayChangeCapacity(E, newCapacity, daptr);
    }
    return da;
}

ekSize ek32ArrayPushUnique(struct ekContext * E, void * daptr, ekU32 * v)
{
    ekS32 i;
    ek32Array * da = ek32ArrayGet(E, daptr, ekTrue);
    for (i = 0; i < da->size; ++i) {
        if (da->values[i] == *v) {
            return i;
        }
    }
    return ek32ArrayPush(E, daptr, v);
}

ekSize ek32ArrayPush(struct ekContext * E, void * daptr, ekU32 * v)
{
    ek32Array * da = ek32ArrayMakeRoom(E, daptr, 1);
    da->values[da->size++] = *v;
    return da->size - 1;
}

void ek32ArrayClear(struct ekContext * E, void * daptr)
{
    ek32Array * da = ek32ArrayGet(E, (ekU32 **)daptr, 0);
    if (da) {
        da->size = 0;
    }
}

void ek32ArrayDestroy(struct ekContext * E, void * daptr)
{
    ek32Array * da = ek32ArrayGet(E, (ekU32 **)daptr, 0);
    if (da) {
        ekFree(da);
        *((ekU32 **)daptr) = NULL;
    }
}

ekSize ek32ArraySize(struct ekContext * E, void * daptr)
{
    ek32Array * da = ek32ArrayGet(E, (ekU32 **)daptr, 0);
    if (da) {
        return da->size;
    }
    return 0;
}
