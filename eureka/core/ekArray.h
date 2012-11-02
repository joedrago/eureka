// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EUREKAARRAY_H
#define EUREKAARRAY_H

#include "ekTypes.h"

// creation / destruction / cleanup
void ekArrayCreate(struct ekContext *Y, void *daptr);
void ekArrayDestroy(struct ekContext *Y, void *daptr, void * /*dynDestroyFunc*/ destroyFunc);
void ekArrayDestroyP1(struct ekContext *Y, void *daptr, void * /*dynDestroyFuncP1*/ destroyFunc, void *p1);
void ekArrayDestroyP2(struct ekContext *Y, void *daptr, void * /*dynDestroyFuncP2*/ destroyFunc, void *p1, void *p2);
void ekArrayDestroyStrings(struct ekContext *Y, void *daptr);
void ekArrayClear(struct ekContext *Y, void *daptr, void * /*dynDestroyFunc*/ destroyFunc);
void ekArrayClearP1(struct ekContext *Y, void *daptr, void * /*dynDestroyFuncP1*/ destroyFunc, void *p1);
void ekArrayClearP2(struct ekContext *Y, void *daptr, void * /*dynDestroyFuncP2*/ destroyFunc, void *p1, void *p2);
void ekArrayClearStrings(struct ekContext *Y, void *daptr);

// front/back manipulation
void *daShift(struct ekContext *Y, void *daptr);
void ekArrayUnshift(struct ekContext *Y, void *daptr, void *p);
ySize ekArrayPush(struct ekContext *Y, void *daptr, void *entry);
ySize ekArrayPushUniqueString(struct ekContext *Y, void *daptr, char *s);
void *ekArrayTop(struct ekContext *Y, void *daptr);
void *ekArrayPop(struct ekContext *Y, void *daptr);

// random access manipulation
void ekArrayInsert(struct ekContext *Y, void *daptr, ySize index, void *p);
void ekArrayErase(struct ekContext *Y, void *daptr, ySize index);

// Size manipulation
void ekArraySetSize(struct ekContext *Y, void *daptr, ySize newSize, void * /*dynDestroyFunc*/ destroyFunc);
void ekArraySetSizeP1(struct ekContext *Y, void *daptr, ySize newSize, void * /*dynDestroyFuncP1*/ destroyFunc, void *p1);
void ekArraySetSizeP2(struct ekContext *Y, void *daptr, ySize newSize, void * /*dynDestroyFuncP2*/ destroyFunc, void *p1, void *p2);
ySize ekArraySize(struct ekContext *Y, void *daptr);
void ekArraySetCapacity(struct ekContext *Y, void *daptr, ySize newCapacity, void * /*dynDestroyFunc*/ destroyFunc);
void ekArraySetCapacityP1(struct ekContext *Y, void *daptr, ySize newCapacity, void * /*dynDestroyFuncP1*/ destroyFunc, void *p1);
void ekArraySetCapacityP2(struct ekContext *Y, void *daptr, ySize newCapacity, void * /*dynDestroyFuncP2*/ destroyFunc, void *p1, void *p2);
ySize ekArrayCapacity(struct ekContext *Y, void *daptr);
void ekArraySquash(struct ekContext *Y, void *daptr);
void ekArrayShrink(struct ekContext *Y, void *daptr, int n, ekDestroyCB cb); // Causes array to be at-most n in count (does not affect capacity)

// 32bit-values-only array
ySize ek32ArraySize(struct ekContext *Y, void *daptr);
ySize ek32ArrayPushUnique(struct ekContext *Y, void *daptr, yU32 *v);
ySize ek32ArrayPush(struct ekContext *Y, void *daptr, yU32 *v);
void ek32ArrayClear(struct ekContext *Y, void *daptr);
void ek32ArrayDestroy(struct ekContext *Y, void *daptr);

#endif