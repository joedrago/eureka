// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPMAP_H
#define YAPMAP_H

#include "yapTypes.h"

typedef enum yapMapKeyType
{
    YMKT_STRING = 1,
    YMKT_INTEGER
} yapMapKeyType;

typedef struct yapMapEntry
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
    struct yapMapEntry *next;
    yU32 hash;
} yapMapEntry;

typedef struct yapMap
{
    yapMapEntry **table; // Hash table daArray
    ySize split;         // Linear Hashing 'split'
    ySize mod;           // pre-split modulus (use mod*2 for overflow)
    int keyType;
    int count;           // count tracking for convenience
} yapMap;

typedef void (*yap2IterateCB1)(struct yapContext *Y, void *arg1, yapMapEntry *entry);

yapMap *yapMapCreate(struct yapContext *Y, yapMapKeyType keyType);
void yapMapDestroy(struct yapContext *Y, yapMap *yh, void * /*yapDestroyCB*/ destroyFunc);
void yapMapClear(struct yapContext *Y, yapMap *yh, void * /*yapDestroyCB*/ destroyFunc);

yapMapEntry *yapMapGetS(struct yapContext *Y, yapMap *yh, const char *key, yBool create);
void yapMapEraseS(struct yapContext *Y, yapMap *yh, const char *key, void * /*yapDestroyCB*/ destroyFunc);
yapMapEntry *yapMapGetI(struct yapContext *Y, yapMap *yh, yU32 key, yBool create);
void yapMapEraseI(struct yapContext *Y, yapMap *yh, yU32 key, void * /*yapDestroyCB*/ destroyFunc);

void yapMapIterateP1(struct yapContext *Y, yapMap *yh, void * /*yapIterateCB1*/ cb, void *arg1); // One prefixed argument: cb(arg1, p)

// Convenience macros

#define yapMapGetS2P(Y, MAP, KEY) (yapMapGetS(Y, MAP, KEY, yTrue)->valuePtr)
#define yapMapGetS2I(Y, MAP, KEY) (yapMapGetS(Y, MAP, KEY, yTrue)->valueInt)
#define yapMapGetI2P(Y, MAP, KEY) (yapMapGetI(Y, MAP, KEY, yTrue)->valuePtr)
#define yapMapGetI2I(Y, MAP, KEY) (yapMapGetI(Y, MAP, KEY, yTrue)->valueInt)

#endif
