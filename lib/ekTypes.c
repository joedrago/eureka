// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekTypes.h"
#include "ekContext.h"

#include "ekLexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef EUREKA_TRACE_MEMORY

// I realize this is large. This is just for memory profiling.
#define MAX_TRACKERS 100000

typedef struct ekMemoryTracker
{
    void * ptr;
    ekS32 bytes;
    const char * file;
    int line;
} ekMemoryTracker;
static ekMemoryTracker sMemoryTrackers[MAX_TRACKERS] = { 0 };

typedef struct ekMemoryStats
{
    ekS32 allocs;
    ekS32 frees;
    ekS32 allocSize;
    ekS32 freeSize;

    // never reset
    ekS32 totalAllocSize;
    ekS32 totalFreeSize;
} ekMemoryStats;
static ekMemoryStats sMemoryStats = { 0 };

void ekMemoryStatsReset()
{
    sMemoryStats.allocs = 0;
    sMemoryStats.frees = 0;
    sMemoryStats.allocSize = 0;
    sMemoryStats.freeSize = 0;
    // no point in reseting sMemoryTrackers
}

void ekMemoryStatsPrint(const char * prefix)
{
    ekTraceMem(("%sMemory Stats: %d/%d A/F count [%d]. %d/%d A/F bytes [%d]. %d/%d A/F total [%d]\n",
                prefix,
                sMemoryStats.allocs,
                sMemoryStats.frees,
                sMemoryStats.allocs - sMemoryStats.frees,
                sMemoryStats.allocSize,
                sMemoryStats.freeSize,
                sMemoryStats.allocSize - sMemoryStats.freeSize,
                sMemoryStats.totalAllocSize,
                sMemoryStats.totalFreeSize,
                sMemoryStats.totalAllocSize - sMemoryStats.totalFreeSize
                ));
}

ekS32 ekMemoryStatsLeftovers()
{
    return sMemoryStats.allocs - sMemoryStats.frees;
}

void ekMemoryStatsDumpLeaks()
{
    ekS32 i;
    for (i=0; i < MAX_TRACKERS; i++) {
        if (sMemoryTrackers[i].ptr != 0) {
#ifndef EUREKA_TRACE_MEMORY_STATS_ONLY
            const char * p = sMemoryTrackers[i].ptr;
            ekS32 j;
            ekTraceMem(("                                     LEAK %p [%d] %s:%d\n",
                        sMemoryTrackers[i].ptr,
                        sMemoryTrackers[i].bytes,
                        sMemoryTrackers[i].file,
                        sMemoryTrackers[i].line));
            ekTraceMem(("                                     **** "));
            for (j = 0; j < sMemoryTrackers[i].bytes; ++j) {
                ekTraceMem((" %02x ", (unsigned char)p[j]));
            }
            ekTraceMem(("\n"));
#endif
        }
    }
}

static void ekTrackAlloc(void * ptr, ekS32 bytes, const char * file, int line)
{
    ekS32 i;
    for (i=0; i < MAX_TRACKERS; i++) {
        if (sMemoryTrackers[i].ptr == 0) {
#ifndef EUREKA_TRACE_MEMORY_STATS_ONLY
            ekTraceMem(("                                     ALLOC %p [%d] %s:%d\n", ptr, bytes, file, line));
#endif
            sMemoryStats.allocs++;
            sMemoryStats.allocSize += bytes;
            sMemoryStats.totalAllocSize += bytes;

            sMemoryTrackers[i].ptr = ptr;
            sMemoryTrackers[i].bytes = bytes;
            sMemoryTrackers[i].file = file;
            sMemoryTrackers[i].line = line;
            return;
        }
    }
    ekAssert(0 && "out of trackers");
}

static void ekTrackFree(void * ptr, ekS32 stomp, const char * file, int line)
{
    ekS32 i;
    for (i=0; i < MAX_TRACKERS; i++) {
        if (sMemoryTrackers[i].ptr == ptr) {
#ifndef EUREKA_TRACE_MEMORY_STATS_ONLY
            ekTraceMem(("                                     FREE %p [%d] %s:%d\n", ptr, sMemoryTrackers[i].bytes, file, line));
#endif
            sMemoryStats.frees++;
            sMemoryStats.freeSize += sMemoryTrackers[i].bytes;
            sMemoryStats.totalFreeSize += sMemoryTrackers[i].bytes;

            if (stomp) {
                // stomp the freed memory
#ifndef EUREKA_TRACE_MEMORY_STATS_ONLY
                ekTraceMem(("                                     STOMP %p [%d] %s:%d\n", ptr, sMemoryTrackers[i].bytes, file, line));
#endif
                memset(ptr, 0xbb, sMemoryTrackers[i].bytes);
            }

            sMemoryTrackers[i].ptr = 0;
            sMemoryTrackers[i].bytes = 0;
            return;
        }
    }
    ekAssert(0 && "unknown ptr in free");
}

void * ekDefaultAllocTracked(ekSize bytes, const char * file, int line)
{
    void * ptr = calloc(1, bytes);
    ekTrackAlloc(ptr, bytes, file, line);
    return ptr;
}

void * ekDefaultReallocTracked(void * ptr, ekSize bytes, const char * file, int line)
{
    void * p = realloc(ptr, bytes);
    if (ptr) { ekTrackFree(ptr, 0, file, line); }
    ekTrackAlloc(p, bytes, file, line);
    return p;
}

void ekDefaultFreeTracked(void * ptr, const char * file, int line)
{
    ekAssert(ptr);
    ekTrackFree(ptr, 1, file, line);
    free(ptr);
}

void * ekDefaultAlloc(ekSize bytes)
{
    return ekDefaultAllocTracked(bytes, "unknown", 0);
}

void * ekDefaultRealloc(void * ptr, ekSize bytes)
{
    return ekDefaultReallocTracked(ptr, bytes, "unknown", 0);
}

void ekDefaultFree(void * ptr)
{
    return ekDefaultFreeTracked(ptr, "unknown", 0);
}
#else /* ifdef EUREKA_TRACE_MEMORY */
void * ekDefaultAlloc(ekSize bytes)
{
    void * ptr = calloc(1, bytes);
    return ptr;
}

void * ekDefaultRealloc(void * ptr, ekSize bytes)
{
    void * p = realloc(ptr, bytes);
    return p;
}

void ekDefaultFree(void * ptr)
{
    ekAssert(ptr);
    free(ptr);
}
#endif /* ifdef EUREKA_TRACE_MEMORY */

void ekDestroyCBFree(struct ekContext * E, void * ptr)
{
    ekFree(ptr);
}

char * ekStrdup(struct ekContext * E, const char * s)
{
    ekS32 len = strlen(s);
    char * copy = ekAlloc(len + 1);
    strcpy(copy, s);
    return copy;
}

char * ekSubstrdup(struct ekContext * E, const char * s, ekS32 start, ekS32 end)
{
    ekS32 len = end - start;
    char * copy = ekAlloc(len + 1);
    memcpy(copy, &s[start], len);
    copy[len] = 0;
    return copy;
}
