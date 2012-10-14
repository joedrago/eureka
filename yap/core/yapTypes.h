// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPTYPES_H
#define YAPTYPES_H

#include <stdlib.h> // for size_t

// ---------------------------------------------------------------------------
// Debug Defines

//#define YAP_DEBUG_SYMBOLS
//#define YAP_TRACE_MEMORY
//#define YAP_TRACE_PARSER
#define YAP_TRACE_EXECUTION
#define YAP_TRACE_REFS
//#define YAP_TRACE_VALUES

// ---------------------------------------------------------------------------
// Forwards

struct yapContext;
struct yapToken;

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

typedef yS16 yOpcode;
typedef yS16 yOperand;

typedef int yBool;
#define yTrue 1
#define yFalse 0

#ifndef NULL
#define NULL 0
#endif

// ---------------------------------------------------------------------------
// Memory Routines

typedef void *(*yapAllocFunc)(ySize bytes);
typedef void *(*yapRellocFunc)(void *ptr, ySize bytes);
typedef void (*yapFreeFunc)(void *ptr);

void *yapDefaultAlloc(ySize bytes);
void *yapDefaultRealloc(void *ptr, ySize bytes);
void yapDefaultFree(void *ptr);

#define yapAlloc Y->allocFunc
#define yapRealloc Y->reallocFunc
#define yapFree Y->freeFunc

char *yapStrdup(struct yapContext *Y, const char *s);
char *yapSubstrdup(struct yapContext *Y, const char *s, int start, int end);

typedef void (*yapDestroyCB)(struct yapContext *Y, void *p);
typedef void (*yapDestroyCB1)(struct yapContext *Y, void *arg1, void *p);

void yapDestroyCBFree(struct yapContext *Y, void *ptr); // calls Y->free() on each element

// ---------------------------------------------------------------------------
// Debug/Tracing Functions

#include <assert.h>
#define yapAssert assert

#ifdef YAP_TRACE_MEMORY
void yapMemoryStatsReset();
void yapMemoryStatsPrint(const char *prefix);
void yapMemoryStatsDumpLeaks();
#define yapTraceMem(ARGS) printf ARGS
#else
#define yapTraceMem(ARGS)
#endif

#ifdef YAP_TRACE_EXECUTION
#define yapTraceExecution(ARGS) printf ARGS
#else
#define yapTraceExecution(ARGS)
#endif

#ifdef YAP_TRACE_REFS
#define yapTraceRefs(ARGS) printf ARGS
#else
#define yapTraceRefs(ARGS)
#endif

#ifdef YAP_TRACE_VALUES
#define yapTraceValues(ARGS) printf ARGS
#else
#define yapTraceValues(ARGS)
#endif

// ---------------------------------------------------------------------------

#endif
