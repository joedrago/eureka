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

// 32bit-values-only array
ySize yap232ArraySize(struct yapContext *Y, void *daptr);
ySize yap232ArrayPushUnique(struct yapContext *Y, void *daptr, yU32 *v);
ySize yap232ArrayPush(struct yapContext *Y, void *daptr, yU32 *v);
void yap232ArrayClear(struct yapContext *Y, void *daptr);
void yap232ArrayDestroy(struct yapContext *Y, void *daptr);

#endif
