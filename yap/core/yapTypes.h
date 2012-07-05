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

typedef void * (*yapAllocFunc)(ySize bytes);
typedef void * (*yapRellocFunc)(void *ptr, ySize bytes);
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

//#define YAP_ENABLE_MEMORY_STATS
#ifdef YAP_ENABLE_MEMORY_STATS
void yapMemoryStatsReset();
void yapMemoryStatsPrint(const char *prefix);
void yapMemoryStatsDumpLeaks();
#define yapTraceMem(ARGS) printf ARGS
#else
#define yapTraceMem(ARGS)
#endif

// ---------------------------------------------------------------------------
// Debug Functions

//#define YAP_DEBUGGING

#include <assert.h>
#define yapAssert assert

//#define YAP_TRACE_PARSE
//#define YAP_TRACE_OPS

#define yapTrace(ARGS)
//#define yapTrace(ARGS) printf ARGS

// ---------------------------------------------------------------------------

#endif
