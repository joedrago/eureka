// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPCONTEXT_H
#define YAPCONTEXT_H

#include "yapArray.h"
#include "yapValue.h"                  // for yapCFunction

// ---------------------------------------------------------------------------
// Forwards

struct yapHash;

// ---------------------------------------------------------------------------

// VM Error Types
enum
{
    YVE_NONE = 0,
    YVE_RUNTIME,
    YVE_COMPILE,

    YVE_COUNT
};

typedef struct yapMemFuncs
{
    yapAllocFunc allocFunc;
    yapRellocFunc reallocFunc;
    yapFreeFunc freeFunc;
} yapMemFuncs;

typedef struct yapContext
{
    // memory funcs
    yapAllocFunc allocFunc;
    yapRellocFunc reallocFunc;
    yapFreeFunc freeFunc;

    // type information
    yapValueType **types;

    // things that can own values
    struct yapHash *globals;           // Global variables
    struct yapFrame **frames;          // Current stack frames
    yapArray stack;                    // Value stack
    struct yapChunk **chunks;          // the VM owns all chunks, making cheap vars

    // GC data
    yapValue ** freeValues;            // Free value pool

    // state
    int lastRet;

    // error data
    yU32 errorType;
    char *error;
} yapContext;

yapContext *yapContextCreate(yapMemFuncs *memFuncs); // if memFuncs is NULL, it will use yapDefault*()
void yapContextDestroy(yapContext *Y);

void yapContextRegisterGlobal(struct yapContext *Y, const char *name, yapValue *value);
void yapContextRegisterGlobalFunction(struct yapContext *Y, const char *name, yapCFunction func); // shortcut

// Yap Eval Options
enum
{
    YEO_DEFAULT = 0,

    YEO_DUMP = (1 << 0)
};
void yapContextEval(struct yapContext *Y, const char *text, yU32 evalOpts);
void yapContextRecover(yapContext *Y); // cleans out frames, clears error

void yapContextSetError(struct yapContext *Y, yU32 errorType, const char *errorFormat, ...);
void yapContextClearError(yapContext *Y);
const char *yapContextGetError(yapContext *Y);

void yapContextGC(struct yapContext *Y);

struct yapFrame *yapContextPushFrame(struct yapContext *Y, struct yapBlock *block, int argCount, yU32 frameType, struct yapValue *thisVal, yapValue *closure);
struct yapFrame *yapContextPopFrames(struct yapContext *Y, yU32 frameTypeToFind, yBool keepIt);

void yapContextLoop(struct yapContext *Y, yBool stopAtPop); // stopAtPop means to stop processing if we ever have less frames than we started with

void yapContextPopValues(struct yapContext *Y, yU32 count);
yapValue *yapContextGetValue(struct yapContext *Y, yU32 howDeep);  // 0 is "top of stack"
yapValue *yapContextThis(yapContext *Y);  // returns 'this' in current context

yBool yapContextGetArgs(struct yapContext *Y, int argCount, const char *argFormat, ...);     // Will pop all arguments on success!
int yapContextArgsFailure(struct yapContext *Y, int argCount, const char *errorFormat, ...); // Will always pop all arguments!

yBool yapContextCallFuncByName(struct yapContext *Y, yapValue *thisVal, const char *name, int argCount); // returns whether or not it found it

#define yapContextGetTop(VM) yapContextGetValue(VM, 0)
#define yapContextGetArg(VM, INDEX, ARGCOUNT) yapContextGetValue(VM, (ARGCOUNT-1) - INDEX)

// ---------------------------------------------------------------------------

#endif
