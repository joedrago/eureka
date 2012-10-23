// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPARRAY_H
#define YAPARRAY_H

#include "yapTypes.h"

// creation / destruction / cleanup
void yap2ArrayCreate(struct yapContext *Y, void *daptr);
void yap2ArrayDestroy(struct yapContext *Y, void *daptr, void * /*dynDestroyFunc*/ destroyFunc);
void yap2ArrayDestroyP1(struct yapContext *Y, void *daptr, void * /*dynDestroyFuncP1*/ destroyFunc, void *p1);
void yap2ArrayDestroyP2(struct yapContext *Y, void *daptr, void * /*dynDestroyFuncP2*/ destroyFunc, void *p1, void *p2);
void yap2ArrayDestroyStrings(struct yapContext *Y, void *daptr);
void yap2ArrayClear(struct yapContext *Y, void *daptr, void * /*dynDestroyFunc*/ destroyFunc);
void yap2ArrayClearP1(struct yapContext *Y, void *daptr, void * /*dynDestroyFuncP1*/ destroyFunc, void *p1);
void yap2ArrayClearP2(struct yapContext *Y, void *daptr, void * /*dynDestroyFuncP2*/ destroyFunc, void *p1, void *p2);
void yap2ArrayClearStrings(struct yapContext *Y, void *daptr);

// front/back manipulation
void *daShift(struct yapContext *Y, void *daptr);
void yap2ArrayUnshift(struct yapContext *Y, void *daptr, void *p);
ySize yap2ArrayPush(struct yapContext *Y, void *daptr, void *entry);
ySize yap2ArrayPushUniqueString(struct yapContext *Y, void *daptr, char *s);
void *yap2ArrayTop(struct yapContext *Y, void *daptr);
void *yap2ArrayPop(struct yapContext *Y, void *daptr);

// random access manipulation
void yap2ArrayInsert(struct yapContext *Y, void *daptr, ySize index, void *p);
void yap2ArrayErase(struct yapContext *Y, void *daptr, ySize index);

// Size manipulation
void yap2ArraySetSize(struct yapContext *Y, void *daptr, ySize newSize, void * /*dynDestroyFunc*/ destroyFunc);
void yap2ArraySetSizeP1(struct yapContext *Y, void *daptr, ySize newSize, void * /*dynDestroyFuncP1*/ destroyFunc, void *p1);
void yap2ArraySetSizeP2(struct yapContext *Y, void *daptr, ySize newSize, void * /*dynDestroyFuncP2*/ destroyFunc, void *p1, void *p2);
ySize yap2ArraySize(struct yapContext *Y, void *daptr);
void yap2ArraySetCapacity(struct yapContext *Y, void *daptr, ySize newCapacity, void * /*dynDestroyFunc*/ destroyFunc);
void yap2ArraySetCapacityP1(struct yapContext *Y, void *daptr, ySize newCapacity, void * /*dynDestroyFuncP1*/ destroyFunc, void *p1);
void yap2ArraySetCapacityP2(struct yapContext *Y, void *daptr, ySize newCapacity, void * /*dynDestroyFuncP2*/ destroyFunc, void *p1, void *p2);
ySize yap2ArrayCapacity(struct yapContext *Y, void *daptr);
void yap2ArraySquash(struct yapContext *Y, void *daptr);
void yap2ArrayShrink(struct yapContext *Y, void *daptr, int n, yapDestroyCB cb); // Causes array to be at-most n in count (does not affect capacity)

// ---------------------------------------------------------------------------
// Dynamic Array macros

typedef struct yap32Array
{
    int count;
    int capacity;
    yU32 *data;
} yap32Array;

yOperand yap32ArrayPushUnique(struct yapContext *Y, yap32Array *p, yU32 *v);
yOperand yap32ArrayPush(struct yapContext *Y, yap32Array *p, yU32 v);
yU32 yap32ArrayPop(struct yapContext *Y, yap32Array *p);
void yap32ArrayClear(struct yapContext *Y, yap32Array *p);

typedef struct yapArray
{
    int count;
    int capacity;
    void **data;
} yapArray;

#define yapArrayCreate() ((yapArray*)yapAlloc(sizeof(yapArray)))

yOperand yapArrayPushUniqueString(struct yapContext *Y, yapArray *p, char *s);
yOperand yapArrayPushUniqueToken(struct yapContext *Y, yapArray *p, struct yapToken *token);
yOperand yapArrayPush(struct yapContext *Y, yapArray *p, void *v);
void *yapArrayPop(struct yapContext *Y, yapArray *p);
void *yapArrayTop(struct yapContext *Y, yapArray *p);
yU32 yapArrayCount(struct yapContext *Y, yapArray *p);
void yapArraySquash(struct yapContext *Y, yapArray *p);  // Removes all NULL entries
void yapArrayUnshift(struct yapContext *Y, yapArray *p, void *v);        // Pushes v n entries on the front of the array
void yapArrayInject(struct yapContext *Y, yapArray *p, void *v, int n);  // Injects v n entries from the end (0 being equivalent to Push)

void yapArrayClear(struct yapContext *Y, yapArray *p, yapDestroyCB cb);
void yapArrayClearP1(struct yapContext *Y, yapArray *p, yapDestroyCB1 cb, void *arg1); // One prefixed argument: cb(arg1, p)
void yapArrayDestroy(struct yapContext *Y, yapArray *p, yapDestroyCB cb);
void yapArrayShrink(struct yapContext *Y, yapArray *p, int n, yapDestroyCB cb); // Causes array to be at-most n in count (does not affect capacity)

#endif
