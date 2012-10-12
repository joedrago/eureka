// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPHASH_H
#define YAPHASH_H

#include "yapTypes.h"

typedef struct yapHashEntry
{
    yU32 hash;
    char *key;
    void *value;
    struct yapHashEntry *next;    
} yapHashEntry;

typedef struct yapHash
{
    int count;
    int width; // number of ptrs in table array
    yapHashEntry **table;
} yapHash;

yapHash *yapHashCreate(struct yapContext *Y, int sizeEstimate);
void yapHashClear(struct yapContext *Y, yapHash *yh, yapDestroyCB cb);
void yapHashDestroy(struct yapContext *Y, yapHash *yh, yapDestroyCB cb);

typedef void (*yapIterateCB)(struct yapContext *Y, yapHashEntry *entry);
void yapHashIterate(struct yapContext *Y, yapHash *yh, yapIterateCB cb);
typedef void (*yapIterateCB1)(struct yapContext *Y, void *arg1, yapHashEntry *entry);
void yapHashIterateP1(struct yapContext *Y, yapHash *yh, yapIterateCB1 cb, void *arg1); // One prefixed argument: cb(arg1, p)

void **yapHashLookup(struct yapContext *Y, yapHash *yh, const char *key, yBool create);
void **yapHashSet(struct yapContext *Y, yapHash *yh, const char *key, void *value);       // assumes 'create' and 'replace'
void yapHashDelete(struct yapContext *Y, yapHash *yh, const char *key, yapDestroyCB cb);

#endif
