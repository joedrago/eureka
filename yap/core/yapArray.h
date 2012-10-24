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
void yapArrayCreate(struct yapContext *Y, void *daptr);
void yapArrayDestroy(struct yapContext *Y, void *daptr, void * /*dynDestroyFunc*/ destroyFunc);
void yapArrayDestroyP1(struct yapContext *Y, void *daptr, void * /*dynDestroyFuncP1*/ destroyFunc, void *p1);
void yapArrayDestroyP2(struct yapContext *Y, void *daptr, void * /*dynDestroyFuncP2*/ destroyFunc, void *p1, void *p2);
void yapArrayDestroyStrings(struct yapContext *Y, void *daptr);
void yapArrayClear(struct yapContext *Y, void *daptr, void * /*dynDestroyFunc*/ destroyFunc);
void yapArrayClearP1(struct yapContext *Y, void *daptr, void * /*dynDestroyFuncP1*/ destroyFunc, void *p1);
void yapArrayClearP2(struct yapContext *Y, void *daptr, void * /*dynDestroyFuncP2*/ destroyFunc, void *p1, void *p2);
void yapArrayClearStrings(struct yapContext *Y, void *daptr);

// front/back manipulation
void *daShift(struct yapContext *Y, void *daptr);
void yapArrayUnshift(struct yapContext *Y, void *daptr, void *p);
ySize yapArrayPush(struct yapContext *Y, void *daptr, void *entry);
ySize yapArrayPushUniqueString(struct yapContext *Y, void *daptr, char *s);
void *yapArrayTop(struct yapContext *Y, void *daptr);
void *yapArrayPop(struct yapContext *Y, void *daptr);

// random access manipulation
void yapArrayInsert(struct yapContext *Y, void *daptr, ySize index, void *p);
void yapArrayErase(struct yapContext *Y, void *daptr, ySize index);

// Size manipulation
void yapArraySetSize(struct yapContext *Y, void *daptr, ySize newSize, void * /*dynDestroyFunc*/ destroyFunc);
void yapArraySetSizeP1(struct yapContext *Y, void *daptr, ySize newSize, void * /*dynDestroyFuncP1*/ destroyFunc, void *p1);
void yapArraySetSizeP2(struct yapContext *Y, void *daptr, ySize newSize, void * /*dynDestroyFuncP2*/ destroyFunc, void *p1, void *p2);
ySize yapArraySize(struct yapContext *Y, void *daptr);
void yapArraySetCapacity(struct yapContext *Y, void *daptr, ySize newCapacity, void * /*dynDestroyFunc*/ destroyFunc);
void yapArraySetCapacityP1(struct yapContext *Y, void *daptr, ySize newCapacity, void * /*dynDestroyFuncP1*/ destroyFunc, void *p1);
void yapArraySetCapacityP2(struct yapContext *Y, void *daptr, ySize newCapacity, void * /*dynDestroyFuncP2*/ destroyFunc, void *p1, void *p2);
ySize yapArrayCapacity(struct yapContext *Y, void *daptr);
void yapArraySquash(struct yapContext *Y, void *daptr);
void yapArrayShrink(struct yapContext *Y, void *daptr, int n, yapDestroyCB cb); // Causes array to be at-most n in count (does not affect capacity)

// 32bit-values-only array
ySize yap32ArraySize(struct yapContext *Y, void *daptr);
ySize yap32ArrayPushUnique(struct yapContext *Y, void *daptr, yU32 *v);
ySize yap32ArrayPush(struct yapContext *Y, void *daptr, yU32 *v);
void yap32ArrayClear(struct yapContext *Y, void *daptr);
void yap32ArrayDestroy(struct yapContext *Y, void *daptr);

#endif
