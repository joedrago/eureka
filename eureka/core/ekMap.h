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
    EMKT_STRING = 1,
    EMKT_INTEGER
} ekMapKeyType;

typedef struct ekMapEntry
{
    union
    {
        char *keyStr;
        ekU32 keyInt;
    };
    union
    {
        void *valuePtr;
        ekU32 valueInt;
        long long value64;
    };
    struct ekMapEntry *next;
    ekU32 hash;
} ekMapEntry;

typedef struct ekMap
{
    ekMapEntry **table; // Hash table daArray
    ekSize split;         // Linear Hashing 'split'
    ekSize mod;           // pre-split modulus (use mod*2 for overflow)
    int keyType;
    int count;           // count tracking for convenience
} ekMap;

typedef void (*ek2IterateCB1)(struct ekContext *E, void *arg1, ekMapEntry *entry);

ekMap *ekMapCreate(struct ekContext *E, ekMapKeyType keyType);
void ekMapDestroy(struct ekContext *E, ekMap *yh, void * /*ekDestroyCB*/ destroyFunc);
void ekMapClear(struct ekContext *E, ekMap *yh, void * /*ekDestroyCB*/ destroyFunc);

ekMapEntry *ekMapGetS(struct ekContext *E, ekMap *yh, const char *key, ekBool create);
void ekMapEraseS(struct ekContext *E, ekMap *yh, const char *key, void * /*ekDestroyCB*/ destroyFunc);
ekMapEntry *ekMapGetI(struct ekContext *E, ekMap *yh, ekU32 key, ekBool create);
void ekMapEraseI(struct ekContext *E, ekMap *yh, ekU32 key, void * /*ekDestroyCB*/ destroyFunc);

void ekMapIterateP1(struct ekContext *E, ekMap *yh, void * /*ekIterateCB1*/ cb, void *arg1); // One prefixed argument: cb(arg1, p)

// Convenience macros

#define ekMapGetS2P(E, MAP, KEY) (ekMapGetS(E, MAP, KEY, ekTrue)->valuePtr)
#define ekMapGetS2I(E, MAP, KEY) (ekMapGetS(E, MAP, KEY, ekTrue)->valueInt)
#define ekMapGetI2P(E, MAP, KEY) (ekMapGetI(E, MAP, KEY, ekTrue)->valuePtr)
#define ekMapGetI2I(E, MAP, KEY) (ekMapGetI(E, MAP, KEY, ekTrue)->valueInt)

#endif
