// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKCONTEXT_H
#define EKCONTEXT_H

#include "ekArray.h"
#include "ekValue.h" // for ekCFunction

#define EMFV_UNLIMITED (-1)

// ---------------------------------------------------------------------------
// Forwards

struct ekChunk;
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

// Eureka Eval Options
enum
{
    EEO_DEFAULT  = 0,

    EEO_DUMP     = (1 << 0),
    EEO_OPTIMIZE = (1 << 1),
    EEO_COMPILE  = (1 << 2), // compile only, skip evaluation
    EEO_REQUIRE  = (1 << 3)  // Eval in a fresh object's context, and return that new object instead.
};

typedef struct ekMemoryInfo
{
    ekAllocFunc allocFunc;
    ekRellocFunc reallocFunc;
    ekFreeFunc freeFunc;
    ekS32 maxFreeValues;
} ekMemoryInfo;

typedef struct ekContext
{
    // memory funcs
    ekAllocFunc allocFunc;
    ekRellocFunc reallocFunc;
    ekFreeFunc freeFunc;

    // type information
    struct ekValueType ** types;

    struct ekMap * intrinsics; // map of funcName -> ekCFunction

    // things that can own values
    struct ekMap * globals;   // Global variables
    struct ekFrame ** frames; // Current stack frames
    struct ekValue ** stack;  // Value stack
    struct ekChunk ** chunks; // the VM owns all chunks, making cheap vars

    // GC data
    ekValue ** usedValues; // All values used by the system
    ekValue ** freeValues; // Free value pool
    ekS32 maxFreeValues;   // Set to zero to not use freeValues pool; EMFV_UNLIMITED for unlimited

    // import related
    char ** importSearchPaths; // Array of paths to search when trying to import (walked in reverse)

    // state
    ekS32 lastRet;

    // error data
    ekU32 errorType;
    char * error;
} ekContext;

ekContext * ekContextCreate(ekMemoryInfo * memFuncs); // if memFuncs is NULL, it will use ekDefault*()
void ekContextDestroy(ekContext * E);

// importSearchPath will be converted to absolute. If it is a file, the dir it sits in will be used instead
void ekContextAddImportSearchPath(struct ekContext * E, const char * importSearchPath);
void ekContextAbsolutePath(struct ekContext * E, const char * path, ekString * output, ekBool dirOnly);

void ekContextEval(struct ekContext * E, const char * sourcePath, const char * text, ekU32 evalOpts, ekValue * result);
void ekContextRecover(ekContext * E); // cleans out frames, clears error

void ekContextSetError(struct ekContext * E, ekU32 errorType, const char * errorFormat, ...);
void ekContextClearError(ekContext * E);
const char * ekContextGetError(ekContext * E);

typedef struct ekModuleFunc
{
    const char * name;
    ekCFunction * func;
} ekModuleFunc;

ekValue * ekContextAddModule(struct ekContext * E, const char * name, ekModuleFunc * funcs); // array should end in NULL
void ekContextAddIntrinsic(struct ekContext * E, const char * name, ekCFunction func);
void ekContextAddGlobal(struct ekContext * E, const char * name, ekValue * v); // takes ownership
void ekContextPushValue(struct ekContext * E, ekValue * v);                    // takes ownership
#define ekContextPushInt(E, I) ekContextPushValue(E, ekValueCreateInt(E, I))
#define ekContextPushFloat(E, F) ekContextPushValue(E, ekValueCreateFloat(E, F))
#define ekContextPushString(E, STR) ekContextPushValue(E, ekValueCreateString(E, STR))
#define ekContextReturn(E, V) { ekContextPushValue(E, V); return 1; }
#define ekContextReturnInt(E, I) { ekContextPushValue(E, ekValueCreateInt(E, I)); return 1; }
#define ekContextReturnFloat(E, F) { ekContextPushValue(E, ekValueCreateFloat(E, F)) return 1; }
#define ekContextReturnString(E, STR) { ekContextPushValue(E, ekValueCreateString(E, STR)) return 1; }

void ekContextPopValues(struct ekContext * E, ekU32 count);
ekValue * ekContextGetValue(struct ekContext * E, ekU32 howDeep);                                // 0 is "top of stack"
ekValue * ekContextThis(ekContext * E);                                                          // returns 'this' in current context
ekBool ekContextGetArgs(struct ekContext * E, ekS32 argCount, const char * argFormat, ...);      // Will pop all arguments on success!
ekS32 ekContextArgsFailure(struct ekContext * E, ekS32 argCount, const char * errorFormat, ...); // Will always pop all arguments!
ekU32 ekContextIterOp(struct ekContext * E, ekU32 argCount);                                     // Perform EOP_ITER
struct ekChunk * ekContextGetCurrentChunk(struct ekContext * E);

struct ekFrame * ekContextPushFrame(struct ekContext * E, struct ekBlock * block, ekS32 argCount, ekU32 frameType, struct ekValue * thisVal, ekValue * closure);
ekBool ekContextCall(struct ekContext * E, struct ekFrame ** framePtr, ekValue * thisVal, ekValue * callable, ekS32 argCount);
struct ekFrame * ekContextPopFrames(struct ekContext * E, ekU32 frameTypeToFind, ekBool keepIt);

void ekContextLoop(struct ekContext * E, ekBool stopAtPop, ekValue * result); // stopAtPop means to stop processing if we ever have less frames than we started with
void ekContextGC(struct ekContext * E);

ekBool ekContextCallFuncByName(struct ekContext * E, ekValue * thisVal, const char * name, ekS32 argCount); // returns whether or not it found it
ekBool ekContextCallCFunction(struct ekContext * E, ekCFunction func, ekU32 argCount, ekValue * thisVal, ekValue * closure);

#define ekContextGetTop(VM) ekContextGetValue(VM, 0)
#define ekContextGetArg(VM, INDEX, ARGCOUNT) ekContextGetValue(VM, (ARGCOUNT - 1) - INDEX)

// ---------------------------------------------------------------------------

#endif // ifndef EKCONTEXT_H
