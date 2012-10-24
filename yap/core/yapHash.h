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















typedef enum yap2HashKeyType
{
    KEYTYPE_STRING = 1,
    KEYTYPE_INTEGER
} yap2HashKeyType;

typedef struct yap2HashEntry
{
    union
    {
        char *keyStr;
        yU32 keyInt;
    };
    union
    {
        void *valuePtr;
        yU32 valueInt;
        long long value64;
    };
    struct yap2HashEntry *next;
    yU32 hash;
} yap2HashEntry;

typedef struct yap2Hash
{
    yap2HashEntry **table; // Hash table daArray
    ySize split;       // Linear Hashing 'split'
    ySize mod;         // pre-split modulus (use mod*2 for overflow)
    int keyType;
    int count;           // count tracking for convenience
} yap2Hash;

typedef void (*yap2IterateCB1)(struct yapContext *Y, void *arg1, yap2HashEntry *entry);

yap2Hash *yap2HashCreate(struct yapContext *Y, yap2HashKeyType keyType, yU32 estimatedSize);
void yap2HashDestroy(struct yapContext *Y, yap2Hash *yh, void * /*dynDestroyFunc*/ destroyFunc);
void yap2HashClear(struct yapContext *Y, yap2Hash *yh, void * /*dynDestroyFunc*/ destroyFunc);

yap2HashEntry *yap2HashGetString(struct yapContext *Y, yap2Hash *yh, const char *key);
yap2HashEntry *yap2HashHasString(struct yapContext *Y, yap2Hash *yh, const char *key);
void yap2HashEraseString(struct yapContext *Y, yap2Hash *yh, const char *key, void * /*dynDestroyFunc*/ destroyFunc);

yap2HashEntry *yap2HashGetInteger(struct yapContext *Y, yap2Hash *yh, yU32 key);
yap2HashEntry *yap2HashHasInteger(struct yapContext *Y, yap2Hash *yh, yU32 key);
void yap2HashEraseInteger(struct yapContext *Y, yap2Hash *yh, yU32 key, void * /*dynDestroyFunc*/ destroyFunc);

void yap2HashIterateP1(struct yapContext *Y, yap2Hash *yh, void * /*yapIterateCB1*/ cb, void *arg1); // One prefixed argument: cb(arg1, p)

// Convenience macros

#define yap2HashGetS2P(Y, MAP, KEY) (yap2HashGetString(Y, MAP, KEY)->valuePtr)
#define yap2HashGetS2I(Y, MAP, KEY) (yap2HashGetString(Y, MAP, KEY)->valueInt)
#define yap2HashGetI2P(Y, MAP, KEY) (yap2HashGetInteger(Y, MAP, KEY)->valuePtr)
#define yap2HashGetI2I(Y, MAP, KEY) (yap2HashGetInteger(Y, MAP, KEY)->valueInt)
#define yap2HashHasS yap2HashHasString
#define yap2HashHasI yap2HashHasInteger

#endif
