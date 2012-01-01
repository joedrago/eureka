#include "yapTypes.h"

#include "yapLexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef YAP_ENABLE_MEMORY_STATS

// I realize this is large. This is just for memory profiling.
#define MAX_TRACKERS 100000

typedef struct yapMemoryTracker
{
    void *ptr;
    int bytes;
} yapMemoryTracker;
static yapMemoryTracker sMemoryTrackers[MAX_TRACKERS] = {0};

typedef struct yapMemoryStats
{
    int allocs;
    int frees;
    int allocSize;
    int freeSize;

    // never reset
    int totalAllocSize;
    int totalFreeSize;
} yapMemoryStats;
static yapMemoryStats sMemoryStats = {0};

void yapMemoryStatsReset()
{
    sMemoryStats.allocs = 0;
    sMemoryStats.frees = 0;
    sMemoryStats.allocSize = 0;
    sMemoryStats.freeSize = 0;
    // no point in reseting sMemoryTrackers
}

void yapMemoryStatsPrint(const char* prefix)
{
    printf("%sMemory Stats: %d/%d A/F count [%d]. %d/%d A/F bytes [%d]. %d/%d A/F total [%d]\n",
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
    );
}

void yapMemoryStatsDumpLeaks()
{
    int i;
    for(i=0; i<MAX_TRACKERS; i++)
    {
        if(sMemoryTrackers[i].ptr != 0)
        {
            yapTraceMem(("                                     LEAK %p [%d]\n",
                sMemoryTrackers[i].ptr,
                sMemoryTrackers[i].bytes));
        }
    }
}

void yapTrackAlloc(void *ptr, int bytes)
{
    int i;
    for(i=0; i<MAX_TRACKERS; i++)
    {
        if(sMemoryTrackers[i].ptr == 0)
        {
            yapTraceMem(("                                     ALLOC %p [%d]\n", ptr, bytes));
            sMemoryStats.allocs++;
            sMemoryStats.allocSize += bytes;
            sMemoryStats.totalAllocSize += bytes;

            sMemoryTrackers[i].ptr = ptr;
            sMemoryTrackers[i].bytes = bytes;
            return;
        }
    }
    yapAssert(0 && "out of trackers");
}

void yapTrackFree(void *ptr)
{
    int i;
    for(i=0; i<MAX_TRACKERS; i++)
    {
        if(sMemoryTrackers[i].ptr == ptr)
        {
            yapTraceMem(("                                     FREE %p [%d]\n", ptr, sMemoryTrackers[i].bytes));
            sMemoryStats.frees++;
            sMemoryStats.freeSize += sMemoryTrackers[i].bytes;
            sMemoryStats.totalFreeSize += sMemoryTrackers[i].bytes;

            sMemoryTrackers[i].ptr = 0;
            sMemoryTrackers[i].bytes = 0;
            return;
        }
    }
    yapAssert(0 && "unknown ptr in free");
}

void yapTrackRealloc(void *oldptr, void *newptr, int bytes)
{
    if(oldptr) yapTrackFree(oldptr);
    yapTrackAlloc(newptr, bytes);
}

#define TRACK_ALLOC(PTR, BYTES) yapTrackAlloc(PTR, BYTES);
#define TRACK_FREE(PTR) if(PTR) yapTrackFree(PTR);
#define TRACK_REALLOC(OLDPTR, NEWPTR, BYTES) yapTrackRealloc(OLDPTR, NEWPTR, BYTES);
#else
#define TRACK_ALLOC(PTR, BYTES)
#define TRACK_FREE(PTR)
#define TRACK_REALLOC(OLDPTR, NEWPTR, BYTES)
#endif

void *yapAlloc(ySize bytes)
{
    void *ptr = calloc(1, bytes);
    TRACK_ALLOC(ptr, bytes)
    return ptr;
}

void *yapRealloc(void *ptr, ySize bytes)
{
    void *p = realloc(ptr, bytes);
    TRACK_REALLOC(ptr, p, bytes)
    return p;
}

void yapFree(void *ptr)
{
    yapAssert(ptr);
    if(ptr == 0)
        printf("huh\n");
    free(ptr);
    TRACK_FREE(ptr)
}

char *yapStrdup(const char *s)
{
    int len = strlen(s);
    char *copy = yapAlloc(len+1);
    strcpy(copy, s);
    return copy;
}

