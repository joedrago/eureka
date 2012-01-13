#ifndef YAPARRAY_H
#define YAPARRAY_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Dynamic Array macros

typedef struct yap32Array
{
    int count;
    int capacity;
    yU32 *data;
} yap32Array;

yOperand yap32ArrayPushUnique(yap32Array *p, yU32 *v);
yOperand yap32ArrayPush(yap32Array *p, yU32 v);
yU32 yap32ArrayPop(yap32Array *p);
void yap32ArrayClear(yap32Array *p);

typedef struct yapArray
{
    int count;
    int capacity;
    void **data;
} yapArray;

#define yapArrayCreate() ((yapArray*)yapAlloc(sizeof(yapArray)))

yOperand yapArrayPushUniqueString(yapArray *p, char *s);
yOperand yapArrayPushUniqueToken(yapArray *p, struct yapToken *token);
yOperand yapArrayPush(yapArray *p, void *v);
void *yapArrayPop(yapArray *p);
void *yapArrayTop(yapArray *p);
yU32 yapArrayCount(yapArray *p);
void yapArraySquash(yapArray *p);  // Removes all NULL entries
void yapArrayUnshift(yapArray *p, void *v);        // Pushes v n entries on the front of the array
void yapArrayInject(yapArray *p, void *v, int n);  // Injects v n entries from the end (0 being equivalent to Push)

void yapArrayClear(yapArray *p, yapDestroyCB cb);
void yapArrayClearP1(yapArray *p, yapDestroyCB1 cb, void *arg1); // One prefixed argument: cb(arg1, p)
void yapArrayDestroy(yapArray *p, yapDestroyCB cb);
void yapArrayShrink(yapArray *p, int n, yapDestroyCB cb); // Causes array to be at-most n in count (does not affect capacity)

#endif
