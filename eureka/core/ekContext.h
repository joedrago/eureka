// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EUREKACONTEXT_H
#define EUREKACONTEXT_H

#include "ekArray.h"
#include "ekValue.h"                  // for ekCFunction

// ---------------------------------------------------------------------------
// Forwards

struct ekMap;

// ---------------------------------------------------------------------------

// VM Error Types
enum
{
    YVE_NONE = 0,
    YVE_RUNTIME,
    YVE_COMPILE,

    YVE_COUNT
};

typedef struct ekMemFuncs
{
    ekAllocFunc allocFunc;
    ekRellocFunc reallocFunc;
    ekFreeFunc freeFunc;
} ekMemFuncs;

typedef struct ekContext
{
    // memory funcs
    ekAllocFunc allocFunc;
    ekRellocFunc reallocFunc;
    ekFreeFunc freeFunc;

    // type information
    ekValueType **types;

    // things that can own values
    struct ekMap *globals;            // Global variables
    struct ekFrame **frames;          // Current stack frames
    struct ekValue **stack;           // Value stack
    struct ekChunk **chunks;          // the VM owns all chunks, making cheap vars

    // GC data
    ekValue **freeValues;             // Free value pool

    // state
    int lastRet;

    // error data
    ekU32 errorType;
    char *error;
} ekContext;

ekContext *ekContextCreate(ekMemFuncs *memFuncs); // if memFuncs is NULL, it will use ekDefault*()
void ekContextDestroy(ekContext *Y);

void ekContextRegisterGlobal(struct ekContext *Y, const char *name, ekValue *value);
void ekContextRegisterGlobalFunction(struct ekContext *Y, const char *name, ekCFunction func); // shortcut

// Eureka Eval Options
enum
{
    YEO_DEFAULT  = 0,

    YEO_DUMP     = (1 << 0),
    YEO_OPTIMIZE = (1 << 1)
};
void ekContextEval(struct ekContext *Y, const char *text, ekU32 evalOpts);
void ekContextRecover(ekContext *Y); // cleans out frames, clears error

void ekContextSetError(struct ekContext *Y, ekU32 errorType, const char *errorFormat, ...);
void ekContextClearError(ekContext *Y);
const char *ekContextGetError(ekContext *Y);

void ekContextGC(struct ekContext *Y);

struct ekFrame *ekContextPushFrame(struct ekContext *Y, struct ekBlock *block, int argCount, ekU32 frameType, struct ekValue *thisVal, ekValue *closure);
struct ekFrame *ekContextPopFrames(struct ekContext *Y, ekU32 frameTypeToFind, ekBool keepIt);

void ekContextLoop(struct ekContext *Y, ekBool stopAtPop); // stopAtPop means to stop processing if we ever have less frames than we started with

void ekContextPopValues(struct ekContext *Y, ekU32 count);
ekValue *ekContextGetValue(struct ekContext *Y, ekU32 howDeep);  // 0 is "top of stack"
ekValue *ekContextThis(ekContext *Y);  // returns 'this' in current context

ekBool ekContextGetArgs(struct ekContext *Y, int argCount, const char *argFormat, ...);     // Will pop all arguments on success!
int ekContextArgsFailure(struct ekContext *Y, int argCount, const char *errorFormat, ...); // Will always pop all arguments!

ekBool ekContextCallFuncByName(struct ekContext *Y, ekValue *thisVal, const char *name, int argCount); // returns whether or not it found it

#define ekContextGetTop(VM) ekContextGetValue(VM, 0)
#define ekContextGetArg(VM, INDEX, ARGCOUNT) ekContextGetValue(VM, (ARGCOUNT-1) - INDEX)

// ---------------------------------------------------------------------------

#endif
