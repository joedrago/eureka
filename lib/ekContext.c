// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekContext.h"

#include "ekBlock.h"
#include "ekChunk.h"
#include "ekCompiler.h"
#include "ekMap.h"
#include "ekObject.h"
#include "ekFrame.h"
#include "ekOp.h"
#include "ekValue.h"
#include "ekValueType.h"

#include "ekmAll.h"
#include "ekIntrinsics.h"

#ifdef EUREKA_ENABLE_EXT_DISASM
#include "ekxDisasm.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// ------------------------------------------------------------------------------------------------
// Constants

#define MAX_ERROR_LENGTH 1023

// ------------------------------------------------------------------------------------------------
// Creation / Destruction

ekContext *ekContextCreate(ekMemoryInfo *memInfo)
{
    ekContext *E = ekDefaultAlloc(sizeof(ekContext));

    // Set defaults
    E->allocFunc = ekDefaultAlloc;
    E->reallocFunc = ekDefaultRealloc;
    E->freeFunc = ekDefaultFree;
    E->maxFreeValues = EMFV_UNLIMITED;

    // LCOV_EXCL_START - I don't care about testing other allocators.
    if(memInfo)
    {
        if(memInfo->allocFunc)
        {
            E->allocFunc = memInfo->allocFunc;
        }
        if(memInfo->reallocFunc)
        {
            E->reallocFunc = memInfo->reallocFunc;
        }
        if(memInfo->freeFunc)
        {
            E->freeFunc = memInfo->freeFunc;
        }
        E->maxFreeValues = memInfo->maxFreeValues;
    }
    // LCOV_EXCL_STOP
    E->intrinsics = ekMapCreate(E, EMKT_STRING);
    E->globals = ekMapCreate(E, EMKT_STRING);

    ekValueTypeRegisterAllBasicTypes(E);
    ekIntrinsicsRegister(E);
    ekModuleRegisterAll(E);
    return E;
}

void ekContextDestroy(ekContext *E)
{
    ekMapDestroy(E, E->globals, ekValueRemoveRefHashed);
    ekArrayDestroy(E, &E->stack, ekValueRemoveRefHashed);
    ekArrayDestroy(E, &E->freeValues, ekValueDestroy);
    ekArrayDestroy(E, &E->frames, ekFrameDestroy);
    ekArrayDestroy(E, &E->chunks, ekChunkDestroy);

    ekMapDestroy(E, E->intrinsics, NULL);
    ekArrayDestroy(E, &E->types, (ekDestroyCB)ekValueTypeDestroy);
    ekContextClearError(E);

    ekFree(E);
}

// ------------------------------------------------------------------------------------------------
// Evaluation / Recovery

static ekBool ekChunkCanBeTemporary(ekChunk *chunk)
{
    // A simple test ... if there aren't any functions in this chunk and all ktable lookups
    // are duped (chunk flagged as temporary), than the chunk can safely go away in between
    // calls to ekContextLoop (assuming zero ekFrames on the stack).
    return !chunk->hasFuncs;
}

void ekContextEval(struct ekContext *E, const char *text, ekU32 evalOpts, ekValue *result)
{
    ekChunk *chunk;
    ekCompiler *compiler;
    ekU32 compileFlags = ECO_DEFAULT;
    ekString formattedError = {0};

    if(evalOpts & EEO_OPTIMIZE)
    {
        compileFlags |= ECO_OPTIMIZE;
    }

    compiler = ekCompilerCreate(E);
    ekCompile(compiler, text, compileFlags);
    if(ekCompilerFormatErrors(compiler, &formattedError))
    {
        ekContextSetError(E, EVE_COMPILE, ekStringSafePtr(&formattedError));
    }
    ekStringClear(E, &formattedError);

    // take ownership of the chunk
    chunk = compiler->chunk;
    compiler->chunk = NULL;

    ekCompilerDestroy(compiler);

    if(chunk)
    {
        if(chunk->block)
        {
            chunk->temporary = ekChunkCanBeTemporary(chunk);

            if(evalOpts & EEO_DUMP)
            {
#ifdef EUREKA_ENABLE_EXT_DISASM
                ekChunkDump(E, chunk);
#else
                ekAssert(0 && "Requesting disasm dump when disasm support is disabled");
#endif
            }

            if(!(evalOpts & EEO_COMPILE))
            {
#ifdef EUREKA_TRACE_EXECUTION
                ekTraceExecution(("--- begin chunk execution ---\n"));
#endif
                // Execute the chunk's block
                ekContextPushFrame(E, chunk->block, 0, EFT_FUNC|EFT_CHUNK, ekValueNullPtr, NULL);
                ekContextLoop(E, ekTrue, result);

#ifdef EUREKA_TRACE_EXECUTION
                ekTraceExecution(("---  end  chunk execution ---\n"));
#endif
                if(!chunk->temporary)
                {
                    ekArrayPush(E, &E->chunks, chunk);
                    chunk = NULL; // forget the ptr
                }
            }
        }

        if(chunk)
        {
            ekChunkDestroy(E, chunk);
        }
    }
}

// LCOV_EXCL_START - TODO: ektest embedding
void ekContextRecover(ekContext *E)
{
    if(E->errorType == EVE_RUNTIME)
    {
        ekS32 prevStackCount = 0;

        ekFrame *frame = ekContextPopFrames(E, EFT_CHUNK, ekTrue);
        if(frame)  // recovery should work on an empty frame stack
        {
            prevStackCount = frame->prevStackCount;
            ekContextPopFrames(E, EFT_CHUNK, ekFalse);
        }
        ekArrayShrink(E, &E->stack, prevStackCount, NULL);
    }
    ekContextClearError(E);
}
// LCOV_EXCL_STOP

// ------------------------------------------------------------------------------------------------
// Error Handling

void ekContextSetError(struct ekContext *E, ekU32 errorType, const char *errorFormat, ...)
{
    va_list args;
    char tempStr[MAX_ERROR_LENGTH + 1];

    ekContextClearError(E);
    va_start(args, errorFormat);
    vsprintf(tempStr, errorFormat, args);
    va_end(args);

    E->errorType = errorType;
    E->error = ekStrdup(E, tempStr);
}

void ekContextClearError(ekContext *E)
{
    if(E->error)
    {
        ekFree(E->error);
        E->error = NULL;
    }
    E->errorType = EVE_NONE;
}

const char *ekContextGetError(ekContext *E)
{
    return E->error;
}

// ------------------------------------------------------------------------------------------------
// Internal Helper Functions

static ekValue *ekFindFunc(struct ekContext *E, ekValue *object, const char *name)
{
    ekValue *v = *(ekObjectGetRef(E, object->objectVal, name, ekFalse));
    if(v == &ekValueNull)
    {
        if(object->objectVal->prototype)
        {
            return ekFindFunc(E, object->objectVal->prototype, name);
        }
        return NULL;
    }
    else
    {
        if(!ekValueIsCallable(v))
        {
            return NULL;
        }
    }
    return v;
}

// ------------------------------------------------------------------------------------------------
// Variable / Value Stack Manipulation

ekValue *ekContextAddModule(struct ekContext *E, const char *name, ekModuleFunc *funcs)
{
    ekValue *module = ekValueCreateObject(E, NULL, 0, ekFalse);
    for(; funcs->name; ++funcs)
    {
        ekValueObjectSetMember(E, module, funcs->name, ekValueCreateCFunction(E, funcs->func));
    }
    ekContextAddGlobal(E, name, module);
    return module;
}

void ekContextAddIntrinsic(struct ekContext *E, const char *name, ekCFunction func)
{
    ekMapGetS2P(E, E->intrinsics, name) = func;
}

void ekContextAddGlobal(struct ekContext *E, const char *name, ekValue *v)
{
    ekValue *prev = ekMapGetS2P(E, E->globals, name);
    ekMapGetS2P(E, E->globals, name) = v;
    if(prev)
    {
        ekValueRemoveRefNote(E, prev, "ekContextAddGlobal overriding prev");
    }
}

void ekContextPushValue(struct ekContext *E, ekValue *v)
{
    ekArrayPush(E, &E->stack, v);
}

// TODO: this needs to protect against variable masking/shadowing
static ekValue *ekContextRegister(struct ekContext *E, const char *name, ekValue *value)
{
    ekMapEntry *hashEntry;
    ekValue **valueRef;
    ekFrame *frame = ekArrayTop(E, &E->frames);
    if(!frame)
    {
        return NULL;
    }

    ekValueAddRefNote(E, value, "ekContextRegister");

    if(frame->block == frame->block->chunk->block)
    {
        // If we're in the chunk's "main" function, all variable
        // registration goes into the globals
        hashEntry = ekMapGetS(E, E->globals, name, ekTrue);
    }
    else
    {
        hashEntry = ekMapGetS(E, frame->locals, name, ekTrue);
    }
    hashEntry->valuePtr = value;
    valueRef = (ekValue **)&hashEntry->valuePtr;
    return ekValueCreateRef(E, valueRef);
}

static ekValue *ekContextResolve(struct ekContext *E, const char *name, ekBool lvalue)
{
    ekS32 i;
    ekFrame *frame;
    ekValue **valueRef = NULL;
    ekMapEntry *hashEntry;

    // Intrinsics have the highest priority, and can only be rvalues
    hashEntry = ekMapGetS(E, E->intrinsics, name, ekFalse);
    if(hashEntry)
    {
        if(lvalue)
        {
            ekContextSetError(E, EVE_RUNTIME, "Cannot use intrinsic function '%s' as an lvalue", name);
            return NULL;
        }
        else
        {
            ekAssert(hashEntry->valuePtr);
            return ekValueCreateCFunction(E, hashEntry->valuePtr);
        }
    }

    for(i = ekArraySize(E, &E->frames) - 1; i >= 0; i--)
    {
        frame = E->frames[i];

        // Check the locals
        hashEntry = ekMapGetS(E, frame->locals, name, ekFalse);
        if(hashEntry)
        {
            valueRef = (ekValue **)&hashEntry->valuePtr;
            break;
        }

        // Check closure vars
        if(frame->closure && frame->closure->closureVars)
        {
            hashEntry = ekMapGetS(E, frame->closure->closureVars, name, ekFalse);
            if(hashEntry)
            {
                valueRef = (ekValue **)&hashEntry->valuePtr;
                break;
            }
        }

        if(frame->type & EFT_FUNC)
        {
            break;
        }
    }

    if(!valueRef)
    {
        // check globals
        hashEntry = ekMapGetS(E, E->globals, name, ekFalse);
        if(hashEntry)
        {
            valueRef = (ekValue **)&hashEntry->valuePtr;
        }
    }

    if(valueRef)
    {
        if(lvalue)
        {
            return ekValueCreateRef(E, valueRef);
        }
        else
        {
            ekValueAddRefNote(E, *valueRef, "ekContextResolve");
            return *valueRef;
        }
    }
    return NULL;
}

static ekBool ekContextCreateObject(struct ekContext *E, ekFrame **framePtr, ekValue *prototype, ekS32 argCount)
{
    ekBool ret = ekTrue;
    ekValue *initFunc = ekFindFunc(E, prototype, "init");
    ekValue *newObject = ekValueCreateObject(E, prototype, (initFunc) ? 0 : argCount, ekFalse);

    if(initFunc)
    {
        ret = ekContextCall(E, framePtr, newObject, initFunc, argCount);
        ekValueRemoveRefNote(E, newObject, "created object passed into call");
        return ret;
    }

    ekArrayPush(E, &E->stack, newObject);
    E->lastRet = 1; // leaving the new object on the stack (object creation via this function is a CALL)
    return ret;
}

void ekContextPopValues(struct ekContext *E, ekU32 count)
{
    while(count)
    {
        ekValue *p = ekArrayPop(E, &E->stack);
        ekValueRemoveRefNote(E, p, "ekContextPopValues");
        count--;
    }
}

ekValue *ekContextGetValue(struct ekContext *E, ekU32 howDeep)
{
    if(howDeep >= ekArraySize(E, &E->stack))
    {
        return NULL;
    }

    return E->stack[(ekArraySize(E, &E->stack) - 1) - howDeep];
}

static ekS32 ekContextPopInts(struct ekContext *E, ekS32 count, ekS32 *output)
{
    ekS32 i;
    for(i=0; i<count; i++)
    {
        ekValue *v = ekArrayPop(E, &E->stack);
        if(!v)
        {
            return i;
        }
        v = ekValueToInt(E, v);
        ekAssert(v);
        output[i] = v->intVal;
        ekValueRemoveRefNote(E, v, "ekContextPopInts");
    }
    return i;
}

ekValue *ekContextThis(ekContext *E)
{
    ekS32 i;
    for(i = ekArraySize(E, &E->frames) - 1; i >= 0; i--)
    {
        ekFrame *frame = E->frames[i];
        if(frame->type & EFT_FUNC)
        {
            ekValueAddRefNote(E, frame->thisVal, "ekContextThis");
            return frame->thisVal;
        }
    }
    return ekValueNullPtr;
}

ekBool ekContextGetArgs(struct ekContext *E, ekS32 argCount, const char *argFormat, ...)
{
    ekBool required = ekTrue;
    ekBool nextUsesThis = ekFalse;
    const char *c;
    ekValue *v;
    ekValue **valuePtr;
    ekValue ***leftovers = NULL;
    ekS32 argsTaken = 0; // from the ek stack (the amount of incoming varargs solely depends on argFormat)
    va_list args;
    va_start(args, argFormat);
    int type;

    for(c = argFormat; *c; c++)
    {
        if(*c == '|')
        {
            required = ekFalse;
            continue;
        }

        if(*c == '*')
        {
            nextUsesThis = ekTrue;
            continue;
        }

        if(*c == '.')
        {
            leftovers = va_arg(args, ekValue ** *);
            break;
        };

        if(nextUsesThis)
        {
            v = ekContextThis(E);
            ekValueRemoveRefNote(E, v, "GetArgs remove +ref from ekContextThis (it will be incremented at end of loop if ok)");
            nextUsesThis = ekFalse;
        }
        else
        {
            if(argsTaken == argCount)
            {
                // We have run out of incoming ek arguments!
                // If the current argument is required, we've just failed.
                // If not, what we've gathered is "enough". Pop the args and return success.
                if(!required)
                {
                    ekContextPopValues(E, argCount);
                    return ekTrue;
                }
                return ekFalse;
            };

            v = ekContextGetArg(E, argsTaken, argCount);
            if(!v)
            {
                // this is a very serious failure (argCount doesn't agree with ekContextGetArg)
                ekContextSetError(E, EVE_RUNTIME, "ekContextGetArgs(): VM stack and argCount disagree!");
                return ekFalse;
            }
            argsTaken++;
        }

        for(type = 0; type < ekArraySize(E, &E->types); ++type)
        {
            ekValueType *vt = E->types[type];
            if(vt->format && (vt->format == *c))
            {
                if(v->type != type)
                {
                    return ekFalse;
                }
                break;
            }
        }

        if(type == ekArraySize(E, &E->types)) // didn't find format in types table, check builtins
        {
            switch(*c)
            {
                default:
                case '?': /* can be anything */
                {
                    break;
                }
                case 'c':
                {
                    // "callable" - function, object, etc
                    if(!ekValueIsCallable(v))
                    {
                        return ekFalse;
                    }
                }
                break;
            };
        }

        valuePtr = va_arg(args, ekValue **);
        *valuePtr = v;
        ekValueAddRefNote(E, v, "ekContextGetArgs");
    }

    if(leftovers)
    {
        for(; argsTaken < argCount; argsTaken++)
        {
            ekValue *v = ekContextGetArg(E, argsTaken, argCount);
            ekArrayPush(E, leftovers, v);
            ekValueAddRefNote(E, v, "ekContextGetArgs leftovers");
        }
    }

    if(argsTaken != argCount)
    {
        // too many args!
        return ekFalse;
    }

    va_end(args);
    ekContextPopValues(E, argCount);
    return ekTrue;
}

// TODO: reuse code between ekContextArgsFailure and ekContextSetError
ekS32 ekContextArgsFailure(struct ekContext *E, ekS32 argCount, const char *errorFormat, ...)
{
    va_list args;
    char tempStr[MAX_ERROR_LENGTH + 1];

    ekContextClearError(E);
    va_start(args, errorFormat);
    vsprintf(tempStr, errorFormat, args);
    va_end(args);

    E->errorType = EVE_RUNTIME;
    E->error = ekStrdup(E, tempStr);

    ekContextPopValues(E, argCount);
    return 0;
}

ekU32 ekContextIterOp(struct ekContext *E, ekU32 argCount)
{
    ekValue *p = ekArrayTop(E, &E->stack);
    ekCFunction *iter = ekValueIter(E, p);
    if(iter)
    {
        return iter(E, argCount);
    }

    // Left the argument on the stack, so return 1
    return 1;
}

// ------------------------------------------------------------------------------------------------
// Frame Stack Manipulation

ekFrame *ekContextPushFrame(struct ekContext *E, ekBlock *block, ekS32 argCount, ekU32 frameType, struct ekValue *thisVal, ekValue *closure)
{
    ekFrame *frame;
    ekS32 i;

    if(thisVal == NULL)
    {
        thisVal = ekValueNullPtr;
    }

    if(block->argCount != EAV_ALL_ARGS)
    {
        // accomodate the function's arglist by padding/removing stack entries
        if(argCount > block->argCount)
        {
            // Too many arguments passed to this function. Pop some!
            ekS32 i;
            for(i = 0; i < (argCount - block->argCount); i++)
            {
                ekValue *v = ekArrayPop(E, &E->stack);
                ekValueRemoveRefNote(E, v, "removing unused args");
            }
        }
        else if(block->argCount > argCount)
        {
            // Too few arguments -- pad with nulls
            ekS32 i;
            for(i = 0; i < (block->argCount - argCount); i++)
            {
                ekArrayPush(E, &E->stack, &ekValueNull); // No need to refcount here
            }
        }
    }

    frame = ekFrameCreate(E, frameType, thisVal, block, ekArraySize(E, &E->stack), argCount, closure);
    ekArrayPush(E, &E->frames, frame);

    return frame;
}

ekBool ekContextCall(struct ekContext *E, ekFrame **framePtr, ekValue *thisVal, ekValue *callable, ekS32 argCount)
{
    // LCOV_EXCL_START - TODO: ektest embedding
    if(!callable)
    {
        ekContextSetError(E, EVE_RUNTIME, "EOP_CALL: empty stack!");
        return ekFalse;
    }
    if(callable->type == EVT_REF)
    {
        callable = *callable->refVal;
    }
    // LCOV_EXCL_STOP
    if(!ekValueIsCallable(callable))
    {
        ekContextSetError(E, EVE_RUNTIME, "EOP_CALL: variable not callable");
        return ekFalse;
    }
    if(callable->type == EVT_CFUNCTION)
    {
        ekValue *closure = (callable->closureVars) ? callable : NULL;
        if(!ekContextCallCFunction(E, *callable->cFuncVal, argCount, thisVal, closure))
        {
            return ekFalse; // LCOV_EXCL_LINE - TODO: ektest embedding. A cfunction needs to ruin the stack frame array for this to return false.
        }
    }
    else if(callable->type == EVT_OBJECT)
    {
        return ekContextCreateObject(E, framePtr, callable, argCount);
    }
    else
    {
        ekValue *closure = (callable->closureVars) ? callable : NULL;
        *framePtr = ekContextPushFrame(E, callable->blockVal, argCount, EFT_FUNC, thisVal, closure);
    }
    return ekTrue;
}

// LCOV_EXCL_START - TODO: ektest embedding
ekBool ekContextCallFuncByName(struct ekContext *E, ekValue *thisVal, const char *name, ekS32 argCount)
{
    ekFrame *frame = NULL;
    ekValue *func = ekContextResolve(E, name, ekFalse);
    if(!func)
    {
        return ekFalse;
    }
    if(ekContextCall(E, &frame, thisVal, func, argCount))
    {
        ekContextLoop(E, ekTrue, NULL);
        return ekTrue;
    }
    return ekFalse;
}
// LCOV_EXCL_STOP

// TODO: merge this function with PushFrame and _RET
ekBool ekContextCallCFunction(struct ekContext *E, ekCFunction func, ekU32 argCount, ekValue *thisVal, ekValue *closure)
{
    ekS32 retCount;
    ekFrame *frame = ekFrameCreate(E, EFT_FUNC, thisVal, NULL, ekArraySize(E, &E->stack), argCount, closure);
    ekArrayPush(E, &E->frames, frame);

    retCount = func(E, argCount);

    ekArrayPop(E, &E->frames); // Removes 'frame' from top of stack
    ekFrameDestroy(E, frame);
    frame = ekArrayTop(E, &E->frames);
    if(!frame)
    {
        return ekFalse;
    }

    // Stash lastRet for any EOP_KEEPs in the pipeline
    E->lastRet = retCount;
    return ekTrue;
}

// LCOV_EXCL_START - I don't care about testing this yet.
static void ekContextFrameCleanup(struct ekContext *E, ekFrame *frame)
{
    if(frame->cleanupCount)
    {
        ekS32 i;
        for(i=0; i < frame->cleanupCount; i++)
        {
            ekS32 index = ((ekArraySize(E, &E->stack) - 1) - E->lastRet) - i;
            ekValueRemoveRefNote(E, E->stack[index], "ekContextFrameCleanup");
            E->stack[index] = NULL;
        }
        ekArraySquash(E, &E->stack);
    }
}
// LCOV_EXCL_STOP

struct ekFrame *ekContextPopFrames(struct ekContext *E, ekU32 frameTypeToFind, ekBool keepIt)
{
    ekFrame *frame = ekArrayTop(E, &E->frames);

    if(frameTypeToFind != EFT_ANY)
    {
        while(frame && !(frame->type & frameTypeToFind))
        {
            ekContextFrameCleanup(E, frame);
            ekFrameDestroy(E, frame);
            ekArrayPop(E, &E->frames);
            frame = ekArrayTop(E, &E->frames);
        };
    }

    if(frame && !keepIt)
    {
        ekContextFrameCleanup(E, frame);
        ekFrameDestroy(E, frame);
        ekArrayPop(E, &E->frames);
        frame = ekArrayTop(E, &E->frames);
    }

    return frame;
}

// ------------------------------------------------------------------------------------------------
// Debug Tracing

#ifdef EUREKA_TRACE_EXECUTION
static const char *ekValueDebugString(struct ekContext *E, ekValue *v)
{
    static char buffer[2048];
    static char valString[2048];

    valString[0] = 0;
    switch(v->type)
    {
        case EVT_INT:
            sprintf(valString, "(%d)", v->intVal);
            break;
        case EVT_FLOAT:
            sprintf(valString, "(%2.2f)", v->floatVal);
            break;
        case EVT_STRING:
            sprintf(valString, "(%s)", ekStringSafePtr(&v->stringVal));
            break;
        case EVT_ARRAY:
            sprintf(valString, "(count: %d)", (ekS32)ekArraySize(E, &v->arrayVal));
            break;
    }

    sprintf(buffer, "[%02d] %s 0x%p %s", v->refs, ekValueTypeName(E, v->type), v, valString);
    return buffer;
}

static void ekContextLogState(ekContext *E)
{
    ekS32 i;

    ekTraceExecution(("\n\n\n------------------------------------------\n"));

    if(ekArraySize(E, &E->frames) > 0)
    {
        ekFrame *frame = ekArrayTop(E, &E->frames);
        ekTraceExecution(("0x%p [cleanup:%d][lastRet:%d] IP: ", frame, frame->cleanupCount, E->lastRet));
        ekOpsDump(frame->ip, 1);
    }

    ekTraceExecution(("\n"));

    ekTraceExecution(("-- Stack Top --\n"));
    for(i=0; i<ekArraySize(E, &E->stack); i++)
    {
        ekValue *v = (ekValue *)E->stack[ekArraySize(E, &E->stack) - 1 - i];
        ekTraceExecution(("%2.2d: %s\n", i, ekValueDebugString(E, v)));
    }
    ekTraceExecution(("-- Stack Bot --\n"));
}
#endif

// ------------------------------------------------------------------------------------------------
// VM / Opcode Interpreter

void ekContextLoop(struct ekContext *E, ekBool stopAtPop, ekValue *result)
{
    ekFrame *frame = ekArrayTop(E, &E->frames);
    ekBool continueLooping = ekTrue;
    ekBool newFrame;
    ekOpcode opcode;
    ekOperand operand;
    ekU32 startingFrameCount = ekArraySize(E, &E->frames);

    if(!frame)
    {
        ekContextSetError(E, EVE_RUNTIME, "ekContextLoop(): No stack frame!");
        return;
    }

    // Main VM loop!
    while(continueLooping && !E->error)
    {
        if(stopAtPop && (ekArraySize(E, &E->frames) < startingFrameCount))
        {
            break;
        }

        newFrame = ekFalse;

        // These are put into temporary variables for future ntohs() cross-platform safety
        opcode  = frame->ip->opcode;
        operand = frame->ip->operand;

#ifdef EUREKA_TRACE_EXECUTION
        ekContextLogState(E);
#endif
        switch(opcode)
        {
            case EOP_NOP:
                break;

            case EOP_START:
                break;

            case EOP_AND:
            case EOP_OR:
            {
                ekS32 i;
                ekValue *performSkipValue = ekArrayTop(E, &E->stack);
                ekBool performSkip = ekFalse;
                if(!performSkipValue)
                {
                    ekContextSetError(E, EVE_RUNTIME, "EOP_SKIP: empty stack!");
                }
                ekValueAddRefNote(E, performSkipValue, "EOP_AND/EOP_OR skip value staying on top of stack");
                performSkipValue = ekValueToBool(E, performSkipValue);
                performSkip = performSkipValue->boolVal;
                ekValueRemoveRefNote(E, performSkipValue, "removing skip value in bool form");
                if(opcode == EOP_AND)
                {
                    performSkip = !performSkip;
                }
                if(performSkip)
                {
                    for(i = 0; i < operand; i++)
                    {
                        frame->ip++;
                    }
                }
                else
                {
                    ekValue *p = ekArrayPop(E, &E->stack);
                    ekValueRemoveRefNote(E, p, "EOP_AND/EOP_OR popping skip value");
                }
            }
            break;

            case EOP_PUSHNULL:
            {
                ekArrayPush(E, &E->stack, &ekValueNull);
            }
            break;

            case EOP_PUSHTHIS:
            {
                ekArrayPush(E, &E->stack, ekContextThis(E)); // ekContextThis +refs
            }
            break;

            case EOP_PUSHI:
            {
                ekValue *value = ekValueCreateInt(E, operand);
                ekArrayPush(E, &E->stack, value);
            }
            break;

            case EOP_PUSH_KB:
            {
                ekValue *value = ekValueCreateFunction(E, frame->block->chunk->blocks[operand]);
                ekArrayPush(E, &E->stack, value);
            }
            break;

            case EOP_PUSH_KI:
            {
                ekValue *value = ekValueCreateInt(E, frame->block->chunk->kInts[operand]);
                ekArrayPush(E, &E->stack, value);
            }
            break;

            case EOP_PUSH_KF:
            {
                ekValue *value = ekValueCreateFloat(E, *((ekF32 *)&frame->block->chunk->kFloats[operand]));
                ekArrayPush(E, &E->stack, value);
            }
            break;

            case EOP_PUSH_BOOL:
            {
                ekValue *value = ekValueCreateBool(E, operand);
                ekArrayPush(E, &E->stack, value);
            }
            break;

            case EOP_PUSH_KS:
            {
                ekValue *value;
                if(frame->block->chunk->temporary)
                {
                    value = ekValueCreateString(E, frame->block->chunk->kStrings[operand]);
                }
                else
                {
                    value = ekValueCreateKString(E, frame->block->chunk->kStrings[operand]);
                }
                ekArrayPush(E, &E->stack, value);
            }
            break;

            case EOP_VREG:
            {
                ekValue *value = ekContextRegister(E, frame->block->chunk->kStrings[operand], ekValueNullPtr);
                ekArrayPush(E, &E->stack, value);
            }
            break;

            case EOP_VREF:
            case EOP_VVAL:
            {
                ekValue *value = ekContextResolve(E, frame->block->chunk->kStrings[operand], (opcode == EOP_VREF));
                if(value)
                {
                    ekArrayPush(E, &E->stack, value);
                }
                else
                {
                    if(!E->error) // Could be set due to bad intrinsic usage in Resolve
                    {
                        ekContextSetError(E, EVE_RUNTIME, "No variable named '%s'", frame->block->chunk->kStrings[operand]);
                    }
                }
            }
            break;

            case EOP_REFVAL:
            {
                ekValue *value = ekArrayPop(E, &E->stack);
                if(!value)
                {
                    ekContextSetError(E, EVE_RUNTIME, "EOP_REFVAL: empty stack!");
                    break;
                };
                if(value->type != EVT_REF)
                {
                    ekContextSetError(E, EVE_RUNTIME, "EOP_REFVAL: requires ref on top of stack");
                    break;
                }
                ekArrayPush(E, &E->stack, *value->refVal);
                ekValueAddRefNote(E, *value->refVal, "RefVal value");
                ekValueRemoveRefNote(E, value, "RefVal ref");
            }
            break;

            case EOP_ADD:
            {
                ekValue *b = ekArrayPop(E, &E->stack);
                ekValue *a = ekArrayPop(E, &E->stack);
                ekValue *c = ekValueAdd(E, a, b);
                ekValueRemoveRefNote(E, b, "add operand 2");
                ekValueRemoveRefNote(E, a, "add operand 1");
                if(c)
                {
                    ekArrayPush(E, &E->stack, c);
                }
                else
                {
                    continueLooping = ekFalse;
                }
            }
            break;

            case EOP_SUB:
            {
                ekValue *b = ekArrayPop(E, &E->stack);
                ekValue *a = ekArrayPop(E, &E->stack);
                ekValue *c = ekValueSub(E, a, b);
                if(operand)
                {
                    // Leave entries on the stack. Used in for loops.
                    ekArrayPush(E, &E->stack, a);
                    ekArrayPush(E, &E->stack, b);
                }
                else
                {
                    ekValueRemoveRefNote(E, b, "sub operand 2");
                    ekValueRemoveRefNote(E, a, "sub operand 1");
                }
                if(c)
                {
                    ekArrayPush(E, &E->stack, c);
                }
                else
                {
                    continueLooping = ekFalse;
                }
            }
            break;

            case EOP_MUL:
            {
                ekValue *b = ekArrayPop(E, &E->stack);
                ekValue *a = ekArrayPop(E, &E->stack);
                ekValue *c = ekValueMul(E, a, b);
                ekValueRemoveRefNote(E, b, "mul operand 2");
                ekValueRemoveRefNote(E, a, "mul operand 1");
                if(c)
                {
                    ekArrayPush(E, &E->stack, c);
                }
                else
                {
                    continueLooping = ekFalse;
                }
            }
            break;

            case EOP_DIV:
            {
                ekValue *b = ekArrayPop(E, &E->stack);
                ekValue *a = ekArrayPop(E, &E->stack);
                ekValue *c = ekValueDiv(E, a, b);
                ekValueRemoveRefNote(E, b, "div operand 2");
                ekValueRemoveRefNote(E, a, "div operand 1");
                if(c)
                {
                    ekArrayPush(E, &E->stack, c);
                }
                else
                {
                    continueLooping = ekFalse;
                }
            }
            break;

            case EOP_CMP:
            case EOP_EQUALS:
            case EOP_NOTEQUALS:
            case EOP_LESSTHAN:
            case EOP_LESSTHANOREQUAL:
            case EOP_GREATERTHAN:
            case EOP_GREATERTHANOREQUAL:
            {
                ekValue *b = ekArrayPop(E, &E->stack);
                ekValue *a = ekArrayPop(E, &E->stack);
                ekS32 cmp = ekValueCmp(E, a, b);
                ekValueRemoveRefNote(E, b, "cmp operand 2");
                ekValueRemoveRefNote(E, a, "cmp operand 1");
                if(opcode != EOP_CMP)
                {
                    if((opcode == EOP_EQUALS) || (opcode == EOP_NOTEQUALS))
                    {
                        // Boolean tests
                        cmp = !cmp;
                        if(opcode == EOP_NOTEQUALS)
                        {
                            cmp = !cmp;
                        }
                    }
                    else
                    {
                        // Comparisons-to-bool
                        switch(opcode)
                        {
                            case EOP_LESSTHAN:
                                cmp = (cmp < 0) ? 1 : 0;
                                break;
                            case EOP_LESSTHANOREQUAL:
                                cmp = (cmp <= 0) ? 1 : 0;
                                break;
                            case EOP_GREATERTHAN:
                                cmp = (cmp > 0) ? 1 : 0;
                                break;
                            case EOP_GREATERTHANOREQUAL:
                                cmp = (cmp >= 0) ? 1 : 0;
                                break;
                        }
                    }
                }
                ekArrayPush(E, &E->stack, ekValueCreateInt(E, cmp));
            }
            break;

            case EOP_VSET:
            {
                ekValue *ref = ekArrayPop(E, &E->stack);
                ekValue *val = ekArrayTop(E, &E->stack);
                if(!operand)
                {
                    ekArrayPop(E, &E->stack);
                }
                if(result)
                {
                    ekValueArrayClear(E, result);
                    ekValueAddRefNote(E, val, "remembering VSET in eval result");
                    ekValueArrayPush(E, result, val);
                }
                continueLooping = ekValueSetRefVal(E, ref, val);
                ekValueRemoveRefNote(E, ref, "VSET temporary reference");
                if(!operand)
                {
                    ekValueRemoveRefNote(E, val, "VSET value not needed anymore");
                }
            }
            break;

            case EOP_INHERITS:
            {
                ekValue *l = ekArrayPop(E, &E->stack);
                ekValue *r = ekArrayPop(E, &E->stack);
                ekBool inherits = ekValueTestInherits(E, l, r);
                ekValueRemoveRefNote(E, r, "inherits operand 2");
                ekValueRemoveRefNote(E, l, "inherits operand 1");

                ekArrayPush(E, &E->stack, ekValueCreateInt(E, inherits ? 1 : 0));
            }
            break;

            case EOP_VARARGS:
            {
                ekS32 i;
                ekS32 varargCount = frame->argCount - operand;
                ekValue *varargsArray = ekValueCreateArray(E);

                // Only one of these for loops will actually loop
                for(; varargCount < 0; varargCount++)
                {
                    ekArrayPush(E, &E->stack, &ekValueNull);
                }
                for(; varargCount > 0; varargCount--)
                {
                    ekArrayUnshift(E, &varargsArray->arrayVal, ekArrayPop(E, &E->stack));
                }

                ekArrayPush(E, &E->stack, varargsArray);
            }
            break;

            case EOP_INDEX:
            {
                ekValue *index = ekArrayPop(E, &E->stack);
                ekValue *value = ekArrayPop(E, &E->stack);
                if(value && index)
                {
                    ekS32 opFlags = operand;
                    ekValue *ret = ekValueIndex(E, value, index, (opFlags & EOF_LVALUE) ? ekTrue : ekFalse);
                    if(ret)
                    {
                        ekArrayPush(E, &E->stack, ret); // +ref implicit in ekValueIndex
                        if(opFlags & EOF_PUSHOBJ)
                        {
                            ekValueAddRefNote(E, value, "INDEX + EOF_PUSHOBJ");
                            ekArrayPush(E, &E->stack, value);
                        }
                        else if(opFlags & EOF_PUSHTHIS)
                        {
                            ekArrayPush(E, &E->stack, ekContextThis(E)); // +ref from ekContextThis
                        }
                    }
                    else
                    {
                        if(E->errorType == EVE_NONE)
                        {
                            ekContextSetError(E, EVE_RUNTIME, "EOP_INDEX: Failed attempt to index into type %s", ekValueTypeName(E, value->type));
                        }
                        continueLooping = ekFalse;
                    }
                }
                else
                {
                    ekContextSetError(E, EVE_RUNTIME, "EOP_INDEX: empty stack!");
                    continueLooping = ekFalse;
                }
                if(value)
                {
                    ekValueRemoveRefNote(E, value, "INDEX value done");
                }
                if(index)
                {
                    ekValueRemoveRefNote(E, index, "INDEX index done");
                }
            }
            break;

            case EOP_DUPE:
            case EOP_MOVE:
            {
                ekS32 topIndex = ekArraySize(E, &E->stack) - 1;
                ekS32 requestedIndex = topIndex - operand;
                if(requestedIndex >= 0)
                {
                    ekValue *val = E->stack[requestedIndex];
                    if(opcode == EOP_MOVE)
                    {
                        if(operand == 0)
                        {
                            break;    // no sense in moving the top to the top
                        }
                        E->stack[requestedIndex] = NULL;
                        ekArraySquash(E, &E->stack);
                    }
                    else
                    {
                        ekValueAddRefNote(E, val, "DUPE");
                    }
                    ekArrayPush(E, &E->stack, val);
                }
                else
                {
                    ekContextSetError(E, EVE_RUNTIME, "%s: impossible index", (opcode == EOP_DUPE) ? "EOP_DUPE" : "EOP_MOVE");
                    continueLooping = ekFalse;
                }
            }
            break;

            case EOP_POP:
            {
                ekS32 i;
                if(result)
                {
                    ekValueArrayClear(E, result);
                    for(i = 0; i < operand; i++)
                    {
                        ekValue *v = ekArrayPop(E, &E->stack);
#ifdef EUREKA_TRACE_REFS
                        ekValueTraceRefs(E, v, 0, "ownership transferred to result");
#endif
                        ekValueArrayPush(E, result, v);
                    }
                }
                else
                {
                    for(i = 0; i < operand; i++)
                    {
                        ekValue *v = ekArrayPop(E, &E->stack);
                        ekValueRemoveRefNote(E, v, "POP");
                    }
                }
            }
            break;

            case EOP_CALL:
            {
                ekS32 argCount = operand;
                ekFrame *oldFrame = frame;
                ekValue *thisVal = ekArrayPop(E, &E->stack);
                ekValue *callable = ekArrayPop(E, &E->stack);
                continueLooping = ekContextCall(E, &frame, thisVal, callable, argCount);
                ekValueRemoveRefNote(E, thisVal, "CALL this done");
                ekValueRemoveRefNote(E, callable, "CALL callable done");
                if(frame != oldFrame)
                {
                    newFrame = ekTrue;
                }
            }
            break;

            case EOP_RET:
            {
                ekS32 argCount = operand;
                frame = ekContextPopFrames(E, EFT_FUNC, ekTrue);
                //                if(!argCount && frame && (frame->flags & EFF_INIT) && frame->thisVal)
                //                {
                //                    // We are leaving an init function that does not return any value (which
                //                    // is different from returning null). This bit of magic makes init() functions
                //                    // return 'this' instead of nothing. It avoids the boilerplate 'return this'
                //                    // at the end of every init() in order for it to work at all, but allows for
                //                    // generator init functions.
                //                    argCount = 1;
                //                    ekArrayPush(E, &E->stack, frame->thisVal);
                //                }
                frame = ekContextPopFrames(E, EFT_FUNC, ekFalse);
                if(frame)
                {
                    // Stash lastRet for any EOP_KEEPs in the pipeline
                    E->lastRet = argCount;
                }
                else
                {
                    // Throw away return values and bail out of loop
                    continueLooping = ekFalse;
                }
            };
            break;

            case EOP_CLEANUP:
            {
                frame->cleanupCount += operand;
                E->lastRet = 0; // reset this here in case as a 'normal' termination of a for loop won't ever set it, but it is legal to override it with a return
            }
            break;

            case EOP_KEEP:
            {
                ekS32 keepCount = operand;
                ekS32 offerCount = E->lastRet;

                if(keepCount < offerCount)
                {
                    ekS32 i;
                    for(i = 0; i < (offerCount - keepCount); i++)
                    {
                        ekValue *v = ekArrayPop(E, &E->stack);
                        ekTraceExecution(("-- cleaning stack entry --\n"));
                        ekValueRemoveRefNote(E, v, "KEEP cleaning stack");
                    }
                }
                else if(keepCount > offerCount)
                {
                    ekS32 i;
                    for(i = 0; i < (keepCount - offerCount); i++)
                    {
                        ekTraceExecution(("-- padding stack with null --\n"));
                        ekArrayPush(E, &E->stack, &ekValueNull);
                    }
                }
                E->lastRet = 0;
            }
            break;

            case EOP_CLOSE:
            {
                ekValue *v = ekArrayTop(E, &E->stack);
                ekValueAddClosureVars(E, v);
            }
            break;

            case EOP_IF:
            {
                ekBlock *block = NULL;
                ekValue *cond, *ifBody, *elseBody = NULL;
                cond   = ekArrayPop(E, &E->stack);
                ifBody = ekArrayPop(E, &E->stack);
                if(operand)
                {
                    elseBody = ekArrayPop(E, &E->stack);
                }
                // TODO: verify ifBody/elseBody are EVT_BLOCK
                cond = ekValueToBool(E, cond);
                if(cond->boolVal)
                {
                    block = ifBody->blockVal;
                }
                else if(elseBody)
                {
                    block = elseBody->blockVal;
                }

                if(block)
                {
                    frame = ekContextPushFrame(E, block, 0, EFT_COND, NULL, NULL);
                    if(frame)
                    {
                        newFrame = ekTrue;
                    }
                    else
                    {
                        continueLooping = ekFalse;
                    }
                }

                ekValueRemoveRefNote(E, cond, "IF cond done");
                ekValueRemoveRefNote(E, ifBody, "IF ifBody done");
                if(elseBody)
                {
                    ekValueRemoveRefNote(E, elseBody, "IF elseBody done");
                }
            }
            break;

            case EOP_ENTER:
            {
                ekValue *blockRef = ekArrayPop(E, &E->stack);

                if(blockRef)
                {
                    if(blockRef->type == EVT_BLOCK && blockRef->blockVal)
                    {
                        ekU32 frameType = operand;
                        frame = ekContextPushFrame(E, blockRef->blockVal, 0, frameType, NULL, NULL);
                        if(frame)
                        {
                            newFrame = ekTrue;
                        }
                        else
                        {
                            continueLooping = ekFalse;
                        }
                    }
                    ekValueRemoveRefNote(E, blockRef, "ENTER: removing block ref");
                }
                else
                {
                    ekContextSetError(E, EVE_RUNTIME, "hurr");
                    continueLooping = ekFalse;
                }
            }
            break;

            case EOP_LEAVE:
            {
                ekBool performLeave = ekTrue;
                if(operand == 1 /* TODO: make this an enum */)
                {
                    ekValue *cond = ekArrayPop(E, &E->stack);
                    cond = ekValueToBool(E, cond);
                    performLeave = !cond->boolVal; // don't leave if expr is true!
                    ekValueRemoveRefNote(E, cond, "LEAVE cond done");
                }
                else if(operand == 2)
                {
                    ekValue *cond = ekArrayPop(E, &E->stack);
                    performLeave = (cond->type == EVT_NULL);
                    ekValueRemoveRefNote(E, cond, "LEAVE cond null check done");
                }

                if(performLeave)
                {
                    frame = ekContextPopFrames(E, EFT_ANY, ekFalse);
                    continueLooping = (frame) ? ekTrue : ekFalse;
                }
            }
            break;

            case EOP_BREAK:
            {
                // a C-style break. Find the innermost loop and kill it.
                frame = ekContextPopFrames(E, EFT_LOOP, ekFalse);
                continueLooping = (frame) ? ekTrue : ekFalse;
            }
            break;

            case EOP_CONTINUE:
            {
                // a C-style continue. Find the innermost loop and reset it.
                frame = ekContextPopFrames(E, EFT_LOOP, ekTrue);
                if(frame)
                {
                    ekFrameReset(E, frame, ekTrue);
                    newFrame = ekTrue;
                }
                else
                {
                    continueLooping = ekFalse;
                }
            }
            break;

            case EOP_NOT:
            {
                ekValue *not;
                ekValue *value = ekArrayPop(E, &E->stack);
                if(!value)
                {
                    ekContextSetError(E, EVE_RUNTIME, "EOP_NOT: empty stack!");
                    continueLooping = ekFalse;
                    break;
                };
                value = ekValueToBool(E, value);
                not = ekValueCreateInt(E, !value->boolVal); // Double temporary?
                ekArrayPush(E, &E->stack, not);
                ekValueRemoveRefNote(E, value, "NOT value done");
            }
            break;

            case EOP_SHIFTLEFT:
            case EOP_SHIFTRIGHT:
            case EOP_BITWISE_NOT:
            case EOP_BITWISE_XOR:
            case EOP_BITWISE_AND:
            case EOP_BITWISE_OR:
            {
                ekS32 i[2];
                ekS32 ret;
                ekS32 argsNeeded = (opcode == EOP_BITWISE_NOT) ? 1 : 2;
                if(ekContextPopInts(E, argsNeeded, i) != argsNeeded)
                {
                    ekContextSetError(E, EVE_RUNTIME, "Bitwise operations require integer friendly arguments");
                    continueLooping = ekFalse;
                    break;
                }

                switch(opcode)
                {
                    case EOP_SHIFTLEFT:   ret =  i[0] << i[1]; break;
                    case EOP_SHIFTRIGHT:  ret =  i[0] >> i[1]; break;
                    case EOP_BITWISE_NOT: ret = ~i[0];         break;
                    case EOP_BITWISE_XOR: ret =  i[0] ^  i[1]; break;
                    case EOP_BITWISE_AND: ret =  i[0] &  i[1]; break;
                    case EOP_BITWISE_OR:  ret =  i[0] |  i[1]; break;
                }
                ekArrayPush(E, &E->stack, ekValueCreateInt(E, ret));
            }
            break;

            case EOP_FORMAT:
            {
                ekValue *format = ekArrayPop(E, &E->stack);
                ekValue *val;
                if(!format)
                {
                    ekContextSetError(E, EVE_RUNTIME, "EOP_FORMAT: empty stack!");
                    continueLooping = ekFalse;
                    break;
                };
                val = ekValueStringFormat(E, format, operand);
                if(!val)
                {
                    ekContextSetError(E, EVE_RUNTIME, "EOP_FORMAT: bad format");
                    continueLooping = ekFalse;
                    break;
                };
                ekArrayPush(E, &E->stack, val);
            }
            break;

            case EOP_ARRAY:
            {
                ekValue *a = ekValueCreateArray(E);
                ekS32 i;
                for(i = 0; i < operand; i++)
                {
                    ekValue *v = ekContextGetArg(E, i, operand);
                    ekArrayPush(E, &a->arrayVal, v);
                }
                for(i = 0; i < operand; i++)
                {
                    // No need to removeref here, as they're owned by the new array
                    ekArrayPop(E, &E->stack);
                }
                ekArrayPush(E, &E->stack, a);
            }
            break;

            case EOP_MAP:
            {
                ekValue *map = ekValueCreateObject(E, NULL, operand, ekFalse);
                ekArrayPush(E, &E->stack, map);
            }
            break;

            case EOP_ITER:
            {
                continueLooping = ekContextCallCFunction(E, ekContextIterOp, 1, ekValueNullPtr, NULL);
            }
            break;

            default:
                ekContextSetError(E, EVE_RUNTIME, "Unknown VM Opcode: %d", opcode);
                continueLooping = ekFalse;
                break;
        }

        if(continueLooping && !newFrame)
        {
            frame->ip++;
        }
    }
}
