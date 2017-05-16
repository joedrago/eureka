// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekMap.h"
#include "ekContext.h"

#include <stdlib.h>
#include <string.h>

// A quick thanks to the author or this web page:
//
// http://www.concentric.net/~ttwang/tech/sorthash.htm
//
// It seemed to offer the most straightforward explanation of how to implement this that I could
// find on the Internet, with its only flaw being that their example explaining a split uses
// bucket 2 at a time when their modulus is also 2, making it unclear that a rewind needs to
// rebucket split+mod instead of split*2 (or anything else). I eventually figured it out.

// ------------------------------------------------------------------------------------------------
// Hash function declarations

#define USE_MURMUR3
//#define USE_DJB2

#ifdef USE_MURMUR3
static ekU32 murmur3string(const unsigned char * str);
static ekU32 murmur3int(ekS32 i);
#define HASHSTRING murmur3string
#define HASHINT murmur3int
#endif

#ifdef USE_DJB2
static ekU32 djb2string(const unsigned char * str);
static unsigned ekS32 djb2int(ekS32 i);
#define HASHSTRING djb2string
#define HASHINT djb2int
#endif

#ifndef HASHSTRING
#error Please choose a hash function!
#endif

// ------------------------------------------------------------------------------------------------
// Constants and Macros

#define INITIAL_MODULUS 2 // "N" on Wikipedia's explanation of linear hashes
#define SHRINK_FACTOR   4 // How many times bigger does the table capacity have to be to its width to cause the table to shrink?

// ------------------------------------------------------------------------------------------------
// Internal helper functions

static ekSize linearHashCompute(ekMap * yh, ekU32 hash)
{
    // Use a simple modulus on the hash, and if the resulting bucket is behind
    // "the split" (putting it in the front partition instead of the expansion),
    // rehash with the next-size-up modulus.

    ekSize addr = hash % yh->mod;
    if (addr < yh->split) {
        addr = hash % (yh->mod << 1);
    }
    return addr;
}

// This is used by ekMapNewEntry to chain a new entry into a bucket, and it is used
// by the split and rewind functions to rebucket everything in a single bucket.
static void ekMapBucketEntryChain(struct ekContext * E, ekMap * yh, ekMapEntry * chain)
{
    while (chain) {
        ekMapEntry * entry = chain;
        ekS32 tableIndex = linearHashCompute(yh, entry->hash);
        chain = chain->next;

        entry->next = yh->table[tableIndex];
        yh->table[tableIndex] = entry;
    }
}

static ekMapEntry * ekMapNewEntry(struct ekContext * E, ekMap * yh, ekU32 hash, void * key)
{
    ekMapEntry * entry;
    ekMapEntry * chain;
    ekS32 found = 0;

    // Create the new entry and bucket it
    entry = (ekMapEntry *)ekAlloc(sizeof(*entry));
    switch (yh->keyType) {
        case EMKT_STRING:
            entry->keyStr = ekStrdup(E, (char *)key);
            break;
        case EMKT_INTEGER:
            entry->keyInt = *((ekS32 *)key);
            break;
    }
    entry->hash = hash;
    entry->value64 = 0;
    ekMapBucketEntryChain(E, yh, entry);

    // Steal the chain at the split boundary...
    chain = yh->table[yh->split];
    yh->table[yh->split] = NULL;

    // ...advance the split...
    ++yh->split;
    if (yh->split == yh->mod) {
        // It is time to grow our linear hash!
        yh->mod *= 2;
        yh->split = 0;
        ekArraySetSize(E, &yh->table, yh->mod << 1, NULL);
    }

    // ... and reattach the stolen chain.
    ekMapBucketEntryChain(E, yh, chain);

    ++yh->count;
    return entry;
}

static void ekMapRewindSplit(struct ekContext * E, ekMap * yh)
{
    ekMapEntry * chain;
    ekSize indexToRebucket;

    --yh->split;
    if (yh->split < 0) {
        yh->mod >>= 1;
        yh->split = yh->mod - 1;
        ekArraySetSize(E, &yh->table, yh->mod << 1, NULL);

        // Time to shrink!
        if ((ekArraySize(E, &yh->table) * SHRINK_FACTOR) < ekArrayCapacity(E, &yh->table)) {
            ekArraySetCapacity(E, &yh->table, ekArraySize(E, &yh->table) * SHRINK_FACTOR, NULL); // Should be no need to destroy anything
        }
    }

    indexToRebucket = yh->split + yh->mod;
    chain = yh->table[indexToRebucket];
    yh->table[indexToRebucket] = NULL;

    ekMapBucketEntryChain(E, yh, chain);
}

// ------------------------------------------------------------------------------------------------
// creation / destruction / cleanup

ekMap * ekMapCreate(struct ekContext * E, ekMapKeyType keyType)
{
    ekMap * yh = (ekMap *)ekAlloc(sizeof(*yh));
    yh->keyType = keyType;
    yh->split = 0;
    yh->mod   = INITIAL_MODULUS;
    yh->count = 0;
    ekArraySetSize(E, &yh->table, yh->mod << 1, NULL);
    return yh;
}

void ekMapDestroy(struct ekContext * E, ekMap * yh, void * /*ekDestroyCB*/ destroyFunc)
{
    if (yh) {
        ekMapClear(E, yh, destroyFunc);
        ekArrayDestroy(E, &yh->table, NULL);
        ekFree(yh);
    }
}

static void ekMapDestroyEntry(struct ekContext * E, ekMap * yh, ekMapEntry * p)
{
    if (yh->keyType == EMKT_STRING) {
        ekFree(p->keyStr);
    }
    ekFree(p);
}

void ekMapClear(struct ekContext * E, ekMap * yh, void * /*ekDestroyCB*/ destroyFunc)
{
    ekDestroyCB func = destroyFunc;
    if (yh) {
        ekS32 tableIndex;
        for (tableIndex = 0; tableIndex < ekArraySize(E, &yh->table); ++tableIndex) {
            ekMapEntry * entry = yh->table[tableIndex];
            while (entry) {
                ekMapEntry * freeme = entry;
                if (func && entry->valuePtr) {
                    func(E, entry->valuePtr);
                }
                entry = entry->next;
                ekMapDestroyEntry(E, yh, freeme);
            }
        }
        memset(yh->table, 0, ekArraySize(E, &yh->table) * sizeof(ekMapEntry *));
    }
}

static ekMapEntry * ekMapFindString(struct ekContext * E, ekMap * yh, const char * key, ekS32 autoCreate)
{
    ekU32 hash = (ekU32)HASHSTRING((const unsigned char *)key);
    ekS32 index = linearHashCompute(yh, hash);
    ekMapEntry * entry = yh->table[index];
    for (; entry; entry = entry->next) {
        if (!strcmp(entry->keyStr, key)) {
            return entry;
        }
    }

    if (autoCreate) {
        // A new entry!
        return ekMapNewEntry(E, yh, hash, (void *)key);
    }
    return NULL;
}

static ekMapEntry * ekMapFindInteger(struct ekContext * E, ekMap * yh, ekU32 key, ekS32 autoCreate)
{
    ekU32 hash = (ekU32)HASHINT(key);
    ekS32 index = linearHashCompute(yh, hash);
    ekMapEntry * entry = yh->table[index];
    for (; entry; entry = entry->next) {
        if (entry->keyInt == key) {
            return entry;
        }
    }

    if (autoCreate) {
        // A new entry!
        return ekMapNewEntry(E, yh, hash, (void *)&key);
    }
    return NULL;
}

ekMapEntry * ekMapGetS(struct ekContext * E, ekMap * yh, const char * key, ekBool create)
{
    return ekMapFindString(E, yh, key, create);
}

void ekMapEraseS(struct ekContext * E, ekMap * yh, const char * key, void * /*ekDestroyCB*/ destroyFunc)
{
    ekDestroyCB func = destroyFunc;
    ekU32 hash = (ekU32)HASHSTRING((const unsigned char *)key);
    ekS32 index = linearHashCompute(yh, hash);
    ekMapEntry * prev = NULL;
    ekMapEntry * entry = yh->table[index];
    for (; entry; prev = entry, entry = entry->next) {
        if (!strcmp(entry->keyStr, key)) {
            if (prev) {
                prev->next = entry->next;
            } else {
                yh->table[index] = entry->next;
            }
            if (func && entry->valuePtr) {
                func(E, entry->valuePtr);
            }
            ekMapDestroyEntry(E, yh, entry);
            --yh->count;
            ekMapRewindSplit(E, yh);
            return;
        }
    }
}

ekMapEntry * ekMapGetI(struct ekContext * E, ekMap * yh, ekU32 key, ekBool create)
{
    return ekMapFindInteger(E, yh, key, create);
}

void ekMapEraseI(struct ekContext * E, ekMap * yh, ekU32 key, void * /*ekDestroyCB*/ destroyFunc)
{
    ekDestroyCB func = destroyFunc;
    ekU32 hash = (ekU32)HASHINT(key);
    ekS32 index = linearHashCompute(yh, hash);
    ekMapEntry * prev = NULL;
    ekMapEntry * entry = yh->table[index];
    for (; entry; prev = entry, entry = entry->next) {
        if (entry->keyInt == key) {
            if (prev) {
                prev->next = entry->next;
            } else {
                yh->table[index] = entry->next;
            }
            if (func && entry->valuePtr) {
                func(E, entry->valuePtr);
            }
            ekMapDestroyEntry(E, yh, entry);
            --yh->count;
            ekMapRewindSplit(E, yh);
            return;
        }
    }
}

void ekMapIterateP1(struct ekContext * E, ekMap * yh, void * rawcb, void * arg1)
{
    ekIterateCB1 cb = (ekIterateCB1)rawcb;
    ekS32 i;
    ekS32 endIndex = yh->split + yh->mod;
    if (!cb) {
        return;
    }
    for (i=0; i < endIndex; i++) {
        ekMapEntry * entry = yh->table[i];
        while (entry) {
            cb(E, arg1, entry);
            entry = entry->next;
        }
    }
}

#ifdef USE_MURMUR3

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. Eou can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER)

typedef unsigned char uint8_t;
typedef unsigned long uint32_t;
typedef unsigned __int64 uint64_t;

// Other compilers

#else // defined(_MSC_VER)

#include <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

static void MurmurHash3_x86_32(const void * key, ekS32 len, uint32_t seed, void * out);
static void MurmurHash3_x86_128(const void * key, ekS32 len, uint32_t seed, void * out);
static void MurmurHash3_x64_128(const void * key, ekS32 len, uint32_t seed, void * out);

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER)

#define FORCE_INLINE    __forceinline

#include <stdlib.h>

#define ROTL32(x, y) _rotl(x, y)
#define ROTL64(x, y) _rotl64(x, y)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else // defined(_MSC_VER)

#define FORCE_INLINE __attribute__((always_inline))

static inline uint32_t rotl32(uint32_t x, int8_t r)
{
    return (x << r) | (x >> (32 - r));
}

static inline uint64_t rotl64(uint64_t x, int8_t r)
{
    return (x << r) | (x >> (64 - r));
}

#define ROTL32(x, y) rotl32(x, y)
#define ROTL64(x, y) rotl64(x, y)

#define BIG_CONSTANT(x) (x ## LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

static FORCE_INLINE uint32_t getblock32(const uint32_t * p, ekS32 i)
{
    return p[i];
}

static FORCE_INLINE uint64_t getblock64(const uint64_t * p, ekS32 i)
{
    return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

static FORCE_INLINE uint32_t fmix32(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}

//----------

static FORCE_INLINE uint64_t fmix64(uint64_t k)
{
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;

    return k;
}

//-----------------------------------------------------------------------------

static void MurmurHash3_x86_32(const void * key, ekS32 len,
                               uint32_t seed, void * out)
{
    const uint8_t * tail;
    uint32_t k1;

    const uint8_t * data = (const uint8_t *)key;
    const ekS32 nblocks = len / 4;

    uint32_t h1 = seed;

    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    ekS32 i;

    //----------
    // body

    const uint32_t * blocks = (const uint32_t *)(data + nblocks * 4);

    for (i = -nblocks; i; i++) {
        uint32_t k1 = getblock32(blocks, i);

        k1 *= c1;
        k1 = ROTL32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = ROTL32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    //----------
    // tail

    tail = (const uint8_t *)(data + nblocks * 4);

    k1 = 0;

    switch (len & 3) {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
            k1 *= c1; k1 = ROTL32(k1, 15); k1 *= c2; h1 ^= k1;
    }

    //----------
    // finalization

    h1 ^= len;

    h1 = fmix32(h1);

    *(uint32_t *)out = h1;
}

//-----------------------------------------------------------------------------

static void MurmurHash3_x86_128(const void * key, ekS32 len,
                                uint32_t seed, void * out)
{
    ekS32 i;
    const uint8_t * tail;
    const uint8_t * data = (const uint8_t *)key;
    const ekS32 nblocks = len / 16;

    uint32_t h1 = seed;
    uint32_t h2 = seed;
    uint32_t h3 = seed;
    uint32_t h4 = seed;

    uint32_t k1 = 0;
    uint32_t k2 = 0;
    uint32_t k3 = 0;
    uint32_t k4 = 0;

    const uint32_t c1 = 0x239b961b;
    const uint32_t c2 = 0xab0e9789;
    const uint32_t c3 = 0x38b34ae5;
    const uint32_t c4 = 0xa1e38b93;

    //----------
    // body

    const uint32_t * blocks = (const uint32_t *)(data + nblocks * 16);

    for (i = -nblocks; i; i++) {
        uint32_t k1 = getblock32(blocks, i * 4 + 0);
        uint32_t k2 = getblock32(blocks, i * 4 + 1);
        uint32_t k3 = getblock32(blocks, i * 4 + 2);
        uint32_t k4 = getblock32(blocks, i * 4 + 3);

        k1 *= c1; k1  = ROTL32(k1, 15); k1 *= c2; h1 ^= k1;

        h1 = ROTL32(h1, 19); h1 += h2; h1 = h1 * 5 + 0x561ccd1b;

        k2 *= c2; k2  = ROTL32(k2, 16); k2 *= c3; h2 ^= k2;

        h2 = ROTL32(h2, 17); h2 += h3; h2 = h2 * 5 + 0x0bcaa747;

        k3 *= c3; k3  = ROTL32(k3, 17); k3 *= c4; h3 ^= k3;

        h3 = ROTL32(h3, 15); h3 += h4; h3 = h3 * 5 + 0x96cd1c35;

        k4 *= c4; k4  = ROTL32(k4, 18); k4 *= c1; h4 ^= k4;

        h4 = ROTL32(h4, 13); h4 += h1; h4 = h4 * 5 + 0x32ac3b17;
    }

    //----------
    // tail

    tail = (const uint8_t *)(data + nblocks * 16);

    switch (len & 15) {
        case 15: k4 ^= tail[14] << 16;
        case 14: k4 ^= tail[13] << 8;
        case 13: k4 ^= tail[12] << 0;
            k4 *= c4; k4  = ROTL32(k4, 18); k4 *= c1; h4 ^= k4;

        case 12: k3 ^= tail[11] << 24;
        case 11: k3 ^= tail[10] << 16;
        case 10: k3 ^= tail[ 9] << 8;
        case  9: k3 ^= tail[ 8] << 0;
            k3 *= c3; k3  = ROTL32(k3, 17); k3 *= c4; h3 ^= k3;

        case  8: k2 ^= tail[ 7] << 24;
        case  7: k2 ^= tail[ 6] << 16;
        case  6: k2 ^= tail[ 5] << 8;
        case  5: k2 ^= tail[ 4] << 0;
            k2 *= c2; k2  = ROTL32(k2, 16); k2 *= c3; h2 ^= k2;

        case  4: k1 ^= tail[ 3] << 24;
        case  3: k1 ^= tail[ 2] << 16;
        case  2: k1 ^= tail[ 1] << 8;
        case  1: k1 ^= tail[ 0] << 0;
            k1 *= c1; k1  = ROTL32(k1, 15); k1 *= c2; h1 ^= k1;
    }

    //----------
    // finalization

    h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;

    h1 += h2; h1 += h3; h1 += h4;
    h2 += h1; h3 += h1; h4 += h1;

    h1 = fmix32(h1);
    h2 = fmix32(h2);
    h3 = fmix32(h3);
    h4 = fmix32(h4);

    h1 += h2; h1 += h3; h1 += h4;
    h2 += h1; h3 += h1; h4 += h1;

    ((uint32_t *)out)[0] = h1;
    ((uint32_t *)out)[1] = h2;
    ((uint32_t *)out)[2] = h3;
    ((uint32_t *)out)[3] = h4;
}

//-----------------------------------------------------------------------------

static void MurmurHash3_x64_128(const void * key, ekS32 len,
                                uint32_t seed, void * out)
{
    ekS32 i;
    const uint8_t * data = (const uint8_t *)key;
    const ekS32 nblocks = len / 16;
    const uint8_t * tail;

    uint64_t h1 = seed;
    uint64_t h2 = seed;

    uint64_t k1 = 0;
    uint64_t k2 = 0;

    const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
    const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

    //----------
    // body

    const uint64_t * blocks = (const uint64_t *)(data);

    for (i = 0; i < nblocks; i++) {
        uint64_t k1 = getblock64(blocks, i * 2 + 0);
        uint64_t k2 = getblock64(blocks, i * 2 + 1);

        k1 *= c1; k1  = ROTL64(k1, 31); k1 *= c2; h1 ^= k1;

        h1 = ROTL64(h1, 27); h1 += h2; h1 = h1 * 5 + 0x52dce729;

        k2 *= c2; k2  = ROTL64(k2, 33); k2 *= c1; h2 ^= k2;

        h2 = ROTL64(h2, 31); h2 += h1; h2 = h2 * 5 + 0x38495ab5;
    }

    //----------
    // tail

    tail = (const uint8_t *)(data + nblocks * 16);

    switch (len & 15) {
        case 15: k2 ^= ((uint64_t)tail[14]) << 48;
        case 14: k2 ^= ((uint64_t)tail[13]) << 40;
        case 13: k2 ^= ((uint64_t)tail[12]) << 32;
        case 12: k2 ^= ((uint64_t)tail[11]) << 24;
        case 11: k2 ^= ((uint64_t)tail[10]) << 16;
        case 10: k2 ^= ((uint64_t)tail[ 9]) << 8;
        case  9: k2 ^= ((uint64_t)tail[ 8]) << 0;
            k2 *= c2; k2  = ROTL64(k2, 33); k2 *= c1; h2 ^= k2;

        case  8: k1 ^= ((uint64_t)tail[ 7]) << 56;
        case  7: k1 ^= ((uint64_t)tail[ 6]) << 48;
        case  6: k1 ^= ((uint64_t)tail[ 5]) << 40;
        case  5: k1 ^= ((uint64_t)tail[ 4]) << 32;
        case  4: k1 ^= ((uint64_t)tail[ 3]) << 24;
        case  3: k1 ^= ((uint64_t)tail[ 2]) << 16;
        case  2: k1 ^= ((uint64_t)tail[ 1]) << 8;
        case  1: k1 ^= ((uint64_t)tail[ 0]) << 0;
            k1 *= c1; k1  = ROTL64(k1, 31); k1 *= c2; h1 ^= k1;
    }

    //----------
    // finalization

    h1 ^= len; h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 = fmix64(h1);
    h2 = fmix64(h2);

    h1 += h2;
    h2 += h1;

    ((uint64_t *)out)[0] = h1;
    ((uint64_t *)out)[1] = h2;
}

static ekU32 murmur3string(const unsigned char * str)
{
    ekU32 hash;
    MurmurHash3_x86_32(str, strlen((const char *)str), 0, &hash);
    return hash;
}

static ekU32 murmur3int(ekS32 i)
{
    ekU32 hash;
    MurmurHash3_x86_32(&i, sizeof(ekS32), 0, &hash);
    return hash;
}

#endif /* ifdef USE_MURMUR3 */

//-----------------------------------------------------------------------------

#ifdef USE_DJB2

static ekU32 djb2string(const unsigned char * str)
{
    ekU32 hash = 5381;
    ekS32 c;

    while (c = *str++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

static unsigned ekS32 djb2int(ekS32 i)
{
    const char * p = (const char *)&i;
    unsigned ekS32 hash = 5381;

    hash = ((hash << 5) + hash) + p[0];
    hash = ((hash << 5) + hash) + p[1];
    hash = ((hash << 5) + hash) + p[2];
    hash = ((hash << 5) + hash) + p[3];
    return hash;
}

#endif
