// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EUREKAMAP_H
#define EUREKAMAP_H

#include "ekTypes.h"

typedef enum ekMapKeyType
{
    YMKT_STRING = 1,
    YMKT_INTEGER
} ekMapKeyType;

typedef struct ekMapEntry
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
    struct ekMapEntry *next;
    yU32 hash;
} ekMapEntry;

typedef struct ekMap
{
    ekMapEntry **table; // Hash table daArray
    ySize split;         // Linear Hashing 'split'
    ySize mod;           // pre-split modulus (use mod*2 for overflow)
    int keyType;
    int count;           // count tracking for convenience
} ekMap;

typedef void (*ek2IterateCB1)(struct ekContext *Y, void *arg1, ekMapEntry *entry);

ekMap *ekMapCreate(struct ekContext *Y, ekMapKeyType keyType);
void ekMapDestroy(struct ekContext *Y, ekMap *yh, void * /*ekDestroyCB*/ destroyFunc);
void ekMapClear(struct ekContext *Y, ekMap *yh, void * /*ekDestroyCB*/ destroyFunc);

ekMapEntry *ekMapGetS(struct ekContext *Y, ekMap *yh, const char *key, yBool create);
void ekMapEraseS(struct ekContext *Y, ekMap *yh, const char *key, void * /*ekDestroyCB*/ destroyFunc);
ekMapEntry *ekMapGetI(struct ekContext *Y, ekMap *yh, yU32 key, yBool create);
void ekMapEraseI(struct ekContext *Y, ekMap *yh, yU32 key, void * /*ekDestroyCB*/ destroyFunc);

void ekMapIterateP1(struct ekContext *Y, ekMap *yh, void * /*ekIterateCB1*/ cb, void *arg1); // One prefixed argument: cb(arg1, p)

// Convenience macros

#define ekMapGetS2P(Y, MAP, KEY) (ekMapGetS(Y, MAP, KEY, yTrue)->valuePtr)
#define ekMapGetS2I(Y, MAP, KEY) (ekMapGetS(Y, MAP, KEY, yTrue)->valueInt)
#define ekMapGetI2P(Y, MAP, KEY) (ekMapGetI(Y, MAP, KEY, yTrue)->valuePtr)
#define ekMapGetI2I(Y, MAP, KEY) (ekMapGetI(Y, MAP, KEY, yTrue)->valueInt)

#endif
