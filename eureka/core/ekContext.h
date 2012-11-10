// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKCONTEXT_H
#define EKCONTEXT_H

#include "ekArray.h"
#include "ekValue.h"                  // for ekCFunction

// ---------------------------------------------------------------------------
// Forwards

struct ekFrame;
struct ekMap;
struct ekValueType;

// ---------------------------------------------------------------------------

// VM Error Types
enum
{
    EVE_NONE = 0,
    EVE_RUNTIME,
    EVE_COMPILE,

    EVE_COUNT
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
    struct ekValueType **types;

    struct ekMap *intrinsics;         // map of funcName -> ekCFunction

    // things that can own values
    struct ekMap *globals;            // Global variables
    struct ekFrame **frames;          // Current stack frames
    struct ekValue **stack;           // Value stack
    struct ekChunk **chunks;          // the VM owns all chunks, making cheap vars

    // pools
    ekValue **freeValues;             // Free value pool

    // state
    ekS32 lastRet;

    // error data
    ekU32 errorType;
    char *error;
} ekContext;

ekContext *ekContextCreate(ekMemFuncs *memFuncs); // if memFuncs is NULL, it will use ekDefault*()
void ekContextDestroy(ekContext *E);

void ekContextAddIntrinsic(struct ekContext *E, const char *name, ekCFunction func);

// Eureka Eval Options
enum
{
    EEO_DEFAULT  = 0,

    EEO_DUMP     = (1 << 0),
    EEO_OPTIMIZE = (1 << 1)
};
void ekContextEval(struct ekContext *E, const char *text, ekU32 evalOpts);
ekBool ekContextCall(struct ekContext *E, struct ekFrame **framePtr, ekValue *thisVal, ekValue *callable, ekS32 argCount);
void ekContextRecover(ekContext *E); // cleans out frames, clears error

void ekContextSetError(struct ekContext *E, ekU32 errorType, const char *errorFormat, ...);
void ekContextClearError(ekContext *E);
const char *ekContextGetError(ekContext *E);

struct ekFrame *ekContextPushFrame(struct ekContext *E, struct ekBlock *block, ekS32 argCount, ekU32 frameType, struct ekValue *thisVal, ekValue *closure);
struct ekFrame *ekContextPopFrames(struct ekContext *E, ekU32 frameTypeToFind, ekBool keepIt);

void ekContextLoop(struct ekContext *E, ekBool stopAtPop); // stopAtPop means to stop processing if we ever have less frames than we started with

void ekContextPopValues(struct ekContext *E, ekU32 count);
ekValue *ekContextGetValue(struct ekContext *E, ekU32 howDeep);  // 0 is "top of stack"
ekValue *ekContextThis(ekContext *E);  // returns 'this' in current context
ekU32 ekContextIterOp(struct ekContext *E, ekU32 argCount); // Perform EOP_ITER

ekBool ekContextGetArgs(struct ekContext *E, ekS32 argCount, const char *argFormat, ...);     // Will pop all arguments on success!
ekS32 ekContextArgsFailure(struct ekContext *E, ekS32 argCount, const char *errorFormat, ...);  // Will always pop all arguments!

ekBool ekContextCallFuncByName(struct ekContext *E, ekValue *thisVal, const char *name, ekS32 argCount); // returns whether or not it found it
ekBool ekContextCallCFunction(struct ekContext *E, ekCFunction func, ekU32 argCount, ekValue *thisVal, ekValue *closure);

#define ekContextGetTop(VM) ekContextGetValue(VM, 0)
#define ekContextGetArg(VM, INDEX, ARGCOUNT) ekContextGetValue(VM, (ARGCOUNT-1) - INDEX)

// ---------------------------------------------------------------------------

#endif
