#ifndef YAPTYPES_H
#define YAPTYPES_H

// ---------------------------------------------------------------------------
// Forwards

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
typedef unsigned int ySize;
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

void *yapAlloc(ySize bytes);
void *yapRealloc(void *ptr, ySize bytes);
void yapFree(void *ptr);
char *yapStrdup(const char *s);

typedef void (*yapDestroyCB)(void *p);
typedef void (*yapDestroyCB1)(void *arg1, void *p);

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

#include <assert.h>
#define yapAssert assert

//#define YAP_TRACE_OPS

#define yapTrace(ARGS)
//#define yapTrace(ARGS) printf ARGS

// ---------------------------------------------------------------------------

#endif
