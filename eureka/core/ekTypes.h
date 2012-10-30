// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EUREKATYPES_H
#define EUREKATYPES_H

#include <stdlib.h> // for size_t

// ---------------------------------------------------------------------------
// Debug Defines

//#define EUREKA_DEBUG_SYMBOLS
//#define EUREKA_TRACE_MEMORY
//#define EUREKA_TRACE_MEMORY_STATS_ONLY
//#define EUREKA_TRACE_PARSER
//#define EUREKA_TRACE_EXECUTION
//#define EUREKA_TRACE_REFS
//#define EUREKA_TRACE_VALUES

// ---------------------------------------------------------------------------
// Forwards

struct ekContext;
struct ekToken;

// ---------------------------------------------------------------------------
// Core Types

typedef int yS32;
typedef unsigned int yU32;
typedef float yF32;
typedef char yS8;
typedef unsigned short yU16;
typedef short yS16;
typedef unsigned char yU8;
typedef size_t ySize;
typedef int yFlag;

typedef yS32 yOpcode;
typedef yS32 yOperand;

typedef int yBool;
#define yTrue 1
#define yFalse 0

#ifndef NULL
#define NULL 0
#endif

// ---------------------------------------------------------------------------
// Memory Routines

typedef void *(*ekAllocFunc)(ySize bytes);
typedef void *(*ekRellocFunc)(void *ptr, ySize bytes);
typedef void (*ekFreeFunc)(void *ptr);

void *ekDefaultAlloc(ySize bytes);
void *ekDefaultRealloc(void *ptr, ySize bytes);
void ekDefaultFree(void *ptr);

#define ekAlloc Y->allocFunc
#define ekRealloc Y->reallocFunc
#define ekFree Y->freeFunc

char *ekStrdup(struct ekContext *Y, const char *s);
char *ekSubstrdup(struct ekContext *Y, const char *s, int start, int end);

typedef void (*ekDestroyCB)(struct ekContext *Y, void *p);
typedef void (*ekDestroyCB1)(struct ekContext *Y, void *arg1, void *p);

void ekDestroyCBFree(struct ekContext *Y, void *ptr); // calls Y->free() on each element

// ---------------------------------------------------------------------------
// Debug/Tracing Functions

#include <assert.h>
#define ekAssert assert
#define ekInline inline

#ifdef EUREKA_TRACE_MEMORY
void ekMemoryStatsReset();
int ekMemoryStatsLeftovers();
void ekMemoryStatsPrint(const char *prefix);
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

#ifdef EUREKA_TRACE_REFS
int ekValueDebugCount();
#define ekTraceRefs(ARGS) printf ARGS
#else
#define ekTraceRefs(ARGS)
#endif

#ifdef EUREKA_TRACE_VALUES
#define ekTraceValues(ARGS) printf ARGS
#else
#define ekTraceValues(ARGS)
#endif

// ---------------------------------------------------------------------------

#endif
