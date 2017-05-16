// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKTYPES_H
#define EKTYPES_H

#include <stdlib.h> // for size_t

// ---------------------------------------------------------------------------
// Debug Defines

// #define EUREKA_DEBUG_SYMBOLS
// #define EUREKA_TRACE_MEMORY
// #define EUREKA_TRACE_MEMORY_STATS_ONLY
// #define EUREKA_TRACE_PARSER
// #define EUREKA_TRACE_EXECUTION
// #define EUREKA_TRACE_VALUES

// ---------------------------------------------------------------------------
// Forwards

struct ekContext;
struct ekToken;

// ---------------------------------------------------------------------------
// Core Types

typedef int ekS32;
typedef unsigned int ekU32;
typedef float ekF32;
typedef char ekS8;
typedef unsigned short ekU16;
typedef short ekS16;
typedef unsigned char ekU8;
typedef size_t ekSize;
typedef int ekFlag;

typedef ekS32 ekOpcode;
typedef ekS32 ekOperand;

typedef int ekBool;
#define ekTrue 1
#define ekFalse 0

#ifndef NULL
#define NULL 0
#endif

// ---------------------------------------------------------------------------
// Memory Routines

typedef void *(*ekAllocFunc)(ekSize bytes);
typedef void *(*ekRellocFunc)(void * ptr, ekSize bytes);
typedef void (*ekFreeFunc)(void * ptr);

void * ekDefaultAlloc(ekSize bytes);
void * ekDefaultRealloc(void * ptr, ekSize bytes);
void ekDefaultFree(void * ptr);

#ifdef EUREKA_TRACE_MEMORY
void * ekDefaultAllocTracked(ekSize bytes, const char * file, int line);
void * ekDefaultReallocTracked(void * ptr, ekSize bytes, const char * file, int line);
void ekDefaultFreeTracked(void * ptr, const char * file, int line);
#define ekAlloc(B) ekDefaultAllocTracked(B, __FILE__, __LINE__)
#define ekRealloc(P, B) ekDefaultReallocTracked(P, B, __FILE__, __LINE__)
#define ekFree(P) ekDefaultFreeTracked(P, __FILE__, __LINE__)
#else
#define ekAlloc E->allocFunc
#define ekRealloc E->reallocFunc
#define ekFree E->freeFunc
#endif

char * ekStrdup(struct ekContext * E, const char * s);
char * ekSubstrdup(struct ekContext * E, const char * s, ekS32 start, ekS32 end);

typedef void (*ekDestroyCB)(struct ekContext * E, void * p);
typedef void (*ekDestroyCB1)(struct ekContext * E, void * arg1, void * p);

void ekDestroyCBFree(struct ekContext * E, void * ptr); // calls E->free() on each element

// ---------------------------------------------------------------------------
// Debug/Tracing Functions

#include <assert.h>
#define ekAssert assert
#define ekInline inline

#ifdef EUREKA_TRACE_MEMORY
void ekMemoryStatsReset();
ekS32 ekMemoryStatsLeftovers();
void ekMemoryStatsPrint(const char * prefix);
void ekMemoryStatsDumpLeaks();
#define ekTraceMem(ARGS) printf ARGS
#else
#define ekTraceMem(ARGS)
#endif

#ifdef EUREKA_TRACE_EXECUTION
#define ekTraceExecution(ARGS) printf ARGS
#else
#define ekTraceExecution(ARGS)
#endif

#ifdef EUREKA_TRACE_VALUES
#define ekTraceValues(ARGS) printf ARGS
#else
#define ekTraceValues(ARGS)
#endif

// ---------------------------------------------------------------------------

#endif // ifndef EKTYPES_H
