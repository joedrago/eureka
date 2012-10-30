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
    void *ptr;
    int bytes;
} ekMemoryTracker;
static ekMemoryTracker sMemoryTrackers[MAX_TRACKERS] = {0};

typedef struct ekMemoryStats
{
    int allocs;
    int frees;
    int allocSize;
    int freeSize;

    // never reset
    int totalAllocSize;
    int totalFreeSize;
} ekMemoryStats;
static ekMemoryStats sMemoryStats = {0};

void ekMemoryStatsReset()
{
    sMemoryStats.allocs = 0;
    sMemoryStats.frees = 0;
    sMemoryStats.allocSize = 0;
    sMemoryStats.freeSize = 0;
    // no point in reseting sMemoryTrackers
}

void ekMemoryStatsPrint(const char *prefix)
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

int ekMemoryStatsLeftovers()
{
    return sMemoryStats.allocs - sMemoryStats.frees;
}

void ekMemoryStatsDumpLeaks()
{
    int i;
    for(i=0; i<MAX_TRACKERS; i++)
    {
        if(sMemoryTrackers[i].ptr != 0)
        {
#ifndef EUREKA_TRACE_MEMORY_STATS_ONLY
            ekTraceMem(("                                     LEAK %p [%d]\n",
                         sMemoryTrackers[i].ptr,
                         sMemoryTrackers[i].bytes));
#endif
        }
    }
}

void ekTrackAlloc(void *ptr, int bytes)
{
    int i;
    for(i=0; i<MAX_TRACKERS; i++)
    {
        if(sMemoryTrackers[i].ptr == 0)
        {
#ifndef EUREKA_TRACE_MEMORY_STATS_ONLY
            ekTraceMem(("                                     ALLOC %p [%d]\n", ptr, bytes));
#endif
            sMemoryStats.allocs++;
            sMemoryStats.allocSize += bytes;
            sMemoryStats.totalAllocSize += bytes;

            sMemoryTrackers[i].ptr = ptr;
            sMemoryTrackers[i].bytes = bytes;
            return;
        }
    }
    ekAssert(0 && "out of trackers");
}

void ekTrackFree(void *ptr, int stomp)
{
    int i;
    for(i=0; i<MAX_TRACKERS; i++)
    {
        if(sMemoryTrackers[i].ptr == ptr)
        {
#ifndef EUREKA_TRACE_MEMORY_STATS_ONLY
            ekTraceMem(("                                     FREE %p [%d]\n", ptr, sMemoryTrackers[i].bytes));
#endif
            sMemoryStats.frees++;
            sMemoryStats.freeSize += sMemoryTrackers[i].bytes;
            sMemoryStats.totalFreeSize += sMemoryTrackers[i].bytes;

            if(stomp)
            {
                // stomp the freed memory
#ifndef EUREKA_TRACE_MEMORY_STATS_ONLY
                ekTraceMem(("                                     STOMP %p [%d]\n", ptr, sMemoryTrackers[i].bytes));
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

void ekTrackRealloc(void *oldptr, void *newptr, int bytes)
{
    if(oldptr) { ekTrackFree(oldptr, 0); }
    ekTrackAlloc(newptr, bytes);
}

#define TRACK_ALLOC(PTR, BYTES) ekTrackAlloc(PTR, BYTES);
#define TRACK_FREE(PTR) if(PTR) ekTrackFree(PTR, 1);
#define TRACK_REALLOC(OLDPTR, NEWPTR, BYTES) ekTrackRealloc(OLDPTR, NEWPTR, BYTES);
#else
#define TRACK_ALLOC(PTR, BYTES)
#define TRACK_FREE(PTR)
#define TRACK_REALLOC(OLDPTR, NEWPTR, BYTES)
#endif

void *ekDefaultAlloc(ySize bytes)
{
    void *ptr = calloc(1, bytes);
    TRACK_ALLOC(ptr, bytes)
    return ptr;
}

void *ekDefaultRealloc(void *ptr, ySize bytes)
{
    void *p = realloc(ptr, bytes);
    TRACK_REALLOC(ptr, p, bytes)
    return p;
}

void ekDefaultFree(void *ptr)
{
    ekAssert(ptr);
    TRACK_FREE(ptr)
    free(ptr);
}

void ekDestroyCBFree(struct ekContext *Y, void *ptr)
{
    ekFree(ptr);
}

char *ekStrdup(struct ekContext *Y, const char *s)
{
    int len = strlen(s);
    char *copy = ekAlloc(len+1);
    strcpy(copy, s);
    return copy;
}

char *ekSubstrdup(struct ekContext *Y, const char *s, int start, int end)
{
    int len = end - start;
    char *copy = ekAlloc(len+1);
    memcpy(copy, &s[start], len);
    copy[len] = 0;
    return copy;
}

