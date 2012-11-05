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

#include "ekmAll.h"
#include "ekiCore.h"
#include "ekiConversions.h"
#include "ekiInheritance.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_ERROR_LENGTH 1023

void ekContextRegisterGlobal(struct ekContext *Y, const char *name, ekValue *value)
{
    ekMapGetS2P(Y, Y->globals, name) = value;
    ekValueAddRefNote(Y, value, "ekContextRegisterGlobal");
}

void ekContextRegisterGlobalFunction(struct ekContext *Y, const char *name, ekCFunction func)
{
    ekValue *p = ekValueCreateCFunction(Y, func);
    ekContextRegisterGlobal(Y, name, p);
    ekValueRemoveRefNote(Y, p, "granted ownership to globals table");
}

static ekBool ekChunkCanBeTemporary(ekChunk *chunk)
{
    // A simple test ... if there aren't any functions in this chunk and all ktable lookups
    // are duped (chunk flagged as temporary), than the chunk can safely go away in between
    // calls to ekContextLoop (assuming zero ekFrames on the stack).
    return !chunk->hasFuncs;
}

void ekContextEval(struct ekContext *Y, const char *text, ekU32 evalOpts)
{
    ekChunk *chunk;
    ekCompiler *compiler;

    compiler = ekCompilerCreate(Y);
    ekCompile(compiler, text, YCO_DEFAULT);

    if(ekArraySize(Y, &compiler->errors))
    {
        int i;
        int total = 0;
        for(i = 0; i < ekArraySize(Y, &compiler->errors); i++)
        {
            char *error = (char *)compiler->errors[i];
            total += strlen(error) + 3; // "* " + newline
        }
        if(total > 0)
        {
            char *s = (char *)ekAlloc(total+1);
            for(i = 0; i < ekArraySize(Y, &compiler->errors); i++)
            {
                char *error = compiler->errors[i];
                strcat(s, "* ");
                strcat(s, error);
                strcat(s, "\n");
            }
            ekContextSetError(Y, YVE_COMPILE, s);
            ekFree(s);
        }
    }

    // take ownership of the chunk
    chunk = compiler->chunk;
    compiler->chunk = NULL;

    ekCompilerDestroy(compiler);

    if(chunk)
    {
        if(chunk->block)
        {
            chunk->temporary = ekChunkCanBeTemporary(chunk);

            if(evalOpts & YEO_DUMP)
            {
#ifdef EUREKA_ENABLE_EXT_DISASM
                ekChunkDump(Y, chunk);
#else
                ekAssert(0 && "Requesting disasm dump when disasm support is disabled");
#endif
            }

#ifdef EUREKA_TRACE_EXECUTION
            ekTraceExecution(("--- begin chunk execution ---\n"));
#endif
            // Execute the chunk's block
            ekContextPushFrame(Y, chunk->block, 0, YFT_FUNC|YFT_CHUNK, ekValueNullPtr, NULL);
            ekContextLoop(Y, ekTrue);

#ifdef EUREKA_TRACE_EXECUTION
            ekTraceExecution(("---  end  chunk execution ---\n"));
#endif
            if(!chunk->temporary)
            {
                ekArrayPush(Y, &Y->chunks, chunk);
                chunk = NULL; // forget the ptr
            }
        }

        if(chunk)
        {
            ekChunkDestroy(Y, chunk);
        }
    }
}

ekContext *ekContextCreate(ekMemFuncs *memFuncs)
{
    ekContext *Y = ekDefaultAlloc(sizeof(ekContext));
    Y->allocFunc = ekDefaultAlloc;
    Y->reallocFunc = ekDefaultRealloc;
    Y->freeFunc = ekDefaultFree;
    // LCOV_EXCL_START - I don't care about testing other allocators.
    if(memFuncs)
    {
        if(memFuncs->allocFunc)
        {
            Y->allocFunc = memFuncs->allocFunc;
        }
        if(memFuncs->reallocFunc)
        {
            Y->reallocFunc = memFuncs->reallocFunc;
        }
        if(memFuncs->freeFunc)
        {
            Y->freeFunc = memFuncs->freeFunc;
        }
    }
    // LCOV_EXCL_STOP
    Y->globals = ekMapCreate(Y, YMKT_STRING);

    ekValueTypeRegisterAllBasicTypes(Y);
    ekIntrinsicsRegisterCore(Y);
    ekIntrinsicsRegisterConversions(Y);
    ekIntrinsicsRegisterInheritance(Y);
    ekModuleRegisterAll(Y);
    return Y;
}

// LCOV_EXCL_START - TODO: ektest embedding
void ekContextRecover(ekContext *Y)
{
    if(Y->errorType == YVE_RUNTIME)
    {
        int prevStackCount = 0;

        ekFrame *frame = ekContextPopFrames(Y, YFT_CHUNK, ekTrue);
        if(frame)  // recovery should work on an empty frame stack
        {
            prevStackCount = frame->prevStackCount;
            ekContextPopFrames(Y, YFT_CHUNK, ekFalse);
        }
        ekArrayShrink(Y, &Y->stack, prevStackCount, NULL);
    }
    ekContextClearError(Y);
}
// LCOV_EXCL_STOP

void ekContextSetError(struct ekContext *Y, ekU32 errorType, const char *errorFormat, ...)
{
    va_list args;
    char tempStr[MAX_ERROR_LENGTH + 1];

    ekContextClearError(Y);
    va_start(args, errorFormat);
    vsprintf(tempStr, errorFormat, args);
    va_end(args);

    Y->errorType = errorType;
    Y->error = ekStrdup(Y, tempStr);
}

void ekContextClearError(ekContext *Y)
{
    if(Y->error)
    {
        ekFree(Y->error);
        Y->error = NULL;
    }
    Y->errorType = YVE_NONE;
}

const char *ekContextGetError(ekContext *Y)
{
    return Y->error;
}

void ekContextDestroy(ekContext *Y)
{
    ekMapDestroy(Y, Y->globals, ekValueRemoveRefHashed);
    ekArrayDestroy(Y, &Y->frames, (ekDestroyCB)ekFrameDestroy);
    ekArrayDestroy(Y, &Y->stack, (ekDestroyCB)ekValueRemoveRefHashed);
    ekArrayDestroy(Y, &Y->chunks, (ekDestroyCB)ekChunkDestroy);

    ekArrayDestroy(Y, &Y->freeValues, (ekDestroyCB)ekValueDestroy);

    ekArrayDestroy(Y, &Y->types, (ekDestroyCB)ekValueTypeDestroy);
    ekContextClearError(Y);

    ekFree(Y);
}

static ekValue **ekContextResolve(struct ekContext *Y, const char *name)
{
    int i;
    ekFrame *frame;
    ekValue **valueRef;
    ekMapEntry *hashEntry;

    for(i = ekArraySize(Y, &Y->frames) - 1; i >= 0; i--)
    {
        frame = Y->frames[i];

        // Check the locals
        hashEntry = ekMapGetS(Y, frame->locals, name, ekFalse);
        if(hashEntry) { return (ekValue **)&hashEntry->valuePtr; }

        // Check closure vars
        if(frame->closure && frame->closure->closureVars)
        {
            hashEntry = ekMapGetS(Y, frame->closure->closureVars, name, ekFalse);
            if(hashEntry) { return (ekValue **)&hashEntry->valuePtr; }
        }

        if(frame->type & YFT_FUNC)
        {
            break;
        }
    }

    // check globals
    hashEntry = ekMapGetS(Y, Y->globals, name, ekFalse);
    if(hashEntry) { return (ekValue **)&hashEntry->valuePtr; }

    return NULL;
}

ekFrame *ekContextPushFrame(struct ekContext *Y, ekBlock *block, int argCount, ekU32 frameType, struct ekValue *thisVal, ekValue *closure)
{
    ekFrame *frame;
    int i;

    if(thisVal == NULL)
    {
        thisVal = ekValueNullPtr;
    }

    if(block->argCount != YAV_ALL_ARGS)
    {
        // accomodate the function's arglist by padding/removing stack entries
        if(argCount > block->argCount)
        {
            // Too many arguments passed to this function. Pop some!
            int i;
            for(i = 0; i < (argCount - block->argCount); i++)
            {
                ekValue *v = ekArrayPop(Y, &Y->stack);
                ekValueRemoveRefNote(Y, v, "removing unused args");
            }
        }
        else if(block->argCount > argCount)
        {
            // Too few arguments -- pad with nulls
            int i;
            for(i = 0; i < (block->argCount - argCount); i++)
            {
                ekArrayPush(Y, &Y->stack, &ekValueNull); // No need to refcount here
            }
        }
    }

    frame = ekFrameCreate(Y, frameType, thisVal, block, ekArraySize(Y, &Y->stack), argCount, closure);
    ekArrayPush(Y, &Y->frames, frame);

    return frame;
}

static ekBool ekContextCallCFunction(struct ekContext *Y, ekCFunction func, ekU32 argCount, ekValue *thisVal);
static ekBool ekContextCreateObject(struct ekContext *Y, ekFrame **framePtr, ekValue *isa, int argCount);

static ekBool ekContextCall(struct ekContext *Y, ekFrame **framePtr, ekValue *thisVal, ekValue *callable, int argCount)
{
    // LCOV_EXCL_START - TODO: ektest embedding
    if(!callable)
    {
        ekContextSetError(Y, YVE_RUNTIME, "YOP_CALL: empty stack!");
        return ekFalse;
    }
    if(callable->type == YVT_REF)
    {
        callable = *callable->refVal;
    }
    // LCOV_EXCL_STOP
    if(!ekValueIsCallable(callable))
    {
        ekContextSetError(Y, YVE_RUNTIME, "YOP_CALL: variable not callable");
        return ekFalse;
    }
    if(callable->type == YVT_CFUNCTION)
    {
        if(!ekContextCallCFunction(Y, *callable->cFuncVal, argCount, thisVal))
        {
            return ekFalse; // LCOV_EXCL_LINE - TODO: ektest embedding. A cfunction needs to ruin the stack frame array for this to return false.
        }
    }
    else if(callable->type == YVT_OBJECT)
    {
        return ekContextCreateObject(Y, framePtr, callable, argCount);
    }
    else
    {
        ekValue *closure = (callable->closureVars) ? callable : NULL;
        *framePtr = ekContextPushFrame(Y, callable->blockVal, argCount, YFT_FUNC, thisVal, closure);
    }
    return ekTrue;
}

static ekValue *ekFindFunc(struct ekContext *Y, ekValue *object, const char *name)
{
    ekValue *v = *(ekObjectGetRef(Y, object->objectVal, name, ekFalse));
    if(v == &ekValueNull)
    {
        if(object->objectVal->isa)
        {
            return ekFindFunc(Y, object->objectVal->isa, name);
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

// LCOV_EXCL_START - TODO: ektest embedding
ekBool ekContextCallFuncByName(struct ekContext *Y, ekValue *thisVal, const char *name, int argCount)
{
    ekFrame *frame = NULL;
    ekValue **valueRef = ekContextResolve(Y, name);
    if(!valueRef || !(*valueRef))
    {
        return ekFalse;
    }
    if(ekContextCall(Y, &frame, thisVal, *valueRef, argCount))
    {
        ekContextLoop(Y, ekTrue);
        return ekTrue;
    }
    return ekFalse;
}
// LCOV_EXCL_STOP

static ekBool ekContextCreateObject(struct ekContext *Y, ekFrame **framePtr, ekValue *isa, int argCount)
{
    ekBool ret = ekTrue;
    ekValue *initFunc = ekFindFunc(Y, isa, "init");
    ekValue *newObject = ekValueCreateObject(Y, isa, (initFunc) ? 0 : argCount, ekFalse);

    if(initFunc)
    {
        ret = ekContextCall(Y, framePtr, newObject, initFunc, argCount);
        ekValueRemoveRefNote(Y, newObject, "created object passed into call");
        return ret;
    }

    ekArrayPush(Y, &Y->stack, newObject);
    Y->lastRet = 1; // leaving the new object on the stack (object creation via this function is a CALL)
    return ret;
}

// TODO: this needs to protect against variable masking/shadowing
static ekValue **ekContextRegister(struct ekContext *Y, const char *name, ekValue *value)
{
    ekMapEntry *hashEntry;
    ekValue **valueRef;
    ekFrame *frame = ekArrayTop(Y, &Y->frames);
    if(!frame)
    {
        return NULL;
    }

    ekValueAddRefNote(Y, value, "ekContextRegister");

    if(frame->block == frame->block->chunk->block)
    {
        // If we're in the chunk's "main" function, all variable
        // registration goes into the globals
        hashEntry = ekMapGetS(Y, Y->globals, name, ekTrue);
    }
    else
    {
        hashEntry = ekMapGetS(Y, frame->locals, name, ekTrue);
    }
    hashEntry->valuePtr = value;
    valueRef = (ekValue **)&hashEntry->valuePtr;
    return valueRef;
}

static void ekContextPushRef(struct ekContext *Y, ekValue **valueRef)
{
    ekValue *value = ekValueCreateRef(Y, valueRef);
    ekArrayPush(Y, &Y->stack, value);
}

// TODO: merge this function with PushFrame and _RET
static ekBool ekContextCallCFunction(struct ekContext *Y, ekCFunction func, ekU32 argCount, ekValue *thisVal)
{
    int retCount;
    ekFrame *frame = ekFrameCreate(Y, YFT_FUNC, thisVal, NULL, ekArraySize(Y, &Y->stack), argCount, NULL);
    ekArrayPush(Y, &Y->frames, frame);

    retCount = func(Y, argCount);

    ekArrayPop(Y, &Y->frames); // Removes 'frame' from top of stack
    ekFrameDestroy(Y, frame);
    frame = ekArrayTop(Y, &Y->frames);
    if(!frame)
    {
        return ekFalse;
    }

    // Stash lastRet for any YOP_KEEPs in the pipeline
    Y->lastRet = retCount;
    return ekTrue;
}

void ekContextPopValues(struct ekContext *Y, ekU32 count)
{
    while(count)
    {
        ekValue *p = ekArrayPop(Y, &Y->stack);
        ekValueRemoveRefNote(Y, p, "ekContextPopValues");
        count--;
    }
}

ekValue *ekContextGetValue(struct ekContext *Y, ekU32 howDeep)
{
    if(howDeep >= ekArraySize(Y, &Y->stack))
    {
        return NULL;
    }

    return Y->stack[(ekArraySize(Y, &Y->stack) - 1) - howDeep];
}

static ekContextFrameCleanup(struct ekContext *Y, ekFrame *frame)
{
    if(frame->cleanupCount)
    {
        int i;
        for(i=0; i < frame->cleanupCount; i++)
        {
            int index = ((ekArraySize(Y, &Y->stack) - 1) - Y->lastRet) - i;
            ekValueRemoveRefNote(Y, Y->stack[index], "ekContextFrameCleanup");
            Y->stack[index] = NULL;
        }
        ekArraySquash(Y, &Y->stack);
    }
}

struct ekFrame *ekContextPopFrames(struct ekContext *Y, ekU32 frameTypeToFind, ekBool keepIt)
{
    ekFrame *frame = ekArrayTop(Y, &Y->frames);

    if(frameTypeToFind != YFT_ANY)
    {
        while(frame && !(frame->type & frameTypeToFind))
        {
            ekContextFrameCleanup(Y, frame);
            ekFrameDestroy(Y, frame);
            ekArrayPop(Y, &Y->frames);
            frame = ekArrayTop(Y, &Y->frames);
        };
    }

    if(frame && !keepIt)
    {
        ekContextFrameCleanup(Y, frame);
        ekFrameDestroy(Y, frame);
        ekArrayPop(Y, &Y->frames);
        frame = ekArrayTop(Y, &Y->frames);
    }

    return frame;
}

static ekS32 ekContextPopInts(struct ekContext *Y, int count, int *output)
{
    ekS32 i;
    for(i=0; i<count; i++)
    {
        ekValue *v = ekArrayPop(Y, &Y->stack);
        if(!v)
        {
            return i;
        }
        v = ekValueToInt(Y, v);
        ekAssert(v);
        output[i] = v->intVal;
        ekValueRemoveRefNote(Y, v, "ekContextPopInts");
    }
    return i;
}

ekValue *ekContextThis(ekContext *Y)
{
    int i;
    for(i = ekArraySize(Y, &Y->frames) - 1; i >= 0; i--)
    {
        ekFrame *frame = Y->frames[i];
        if(frame->type & YFT_FUNC)
        {
            ekValueAddRefNote(Y, frame->thisVal, "ekContextThis");
            return frame->thisVal;
        }
    }
    return ekValueNullPtr;
}

ekBool ekContextGetArgs(struct ekContext *Y, int argCount, const char *argFormat, ...)
{
    ekBool required = ekTrue;
    const char *c;
    ekValue *v;
    ekValue **valuePtr;
    ekValue ***leftovers = NULL;
    int argsTaken = 0; // from the ek stack (the amount of incoming varargs solely depends on argFormat)
    va_list args;
    va_start(args, argFormat);

    for(c = argFormat; *c; c++)
    {
        if(*c == '|')
        {
            required = ekFalse;
            continue;
        }

        if(*c == '.')
        {
            leftovers = va_arg(args, ekValue ** *);
            break;
        };

        if(argsTaken == argCount)
        {
            // We have run out of incoming ek arguments!
            // If the current argument is required, we've just failed.
            // If not, what we've gathered is "enough". Pop the args and return success.
            if(!required)
            {
                ekContextPopValues(Y, argCount);
                return ekTrue;
            }
            return ekFalse;
        };

        v = ekContextGetArg(Y, argsTaken, argCount);
        if(!v)
        {
            // this is a very serious failure (argCount doesn't agree with ekContextGetArg)
            ekContextSetError(Y, YVE_RUNTIME, "ekContextGetArgs(): VM stack and argCount disagree!");
            return ekFalse;
        }
        argsTaken++;

        switch(*c)
        {
            default:
            case '?': /* can be anything */                    break;
            case 'n': if(v->type != YVT_NULL) { return ekFalse; } break;
            case 's': if(v->type != YVT_STRING) { return ekFalse; } break;
            case 'i': if(v->type != YVT_INT) { return ekFalse; } break;
            case 'f': if(v->type != YVT_FLOAT) { return ekFalse; } break;
            case 'a': if(v->type != YVT_ARRAY) { return ekFalse; } break;
            case 'm': if(v->type != YVT_OBJECT) { return ekFalse; } break;  // "map"
            case 'o': if(v->type != YVT_OBJECT) { return ekFalse; } break;
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

        valuePtr = va_arg(args, ekValue **);
        *valuePtr = v;
        ekValueAddRefNote(Y, v, "ekContextGetArgs");
    }

    if(leftovers)
    {
        for(; argsTaken < argCount; argsTaken++)
        {
            ekValue *v = ekContextGetArg(Y, argsTaken, argCount);
            ekArrayPush(Y, leftovers, v);
            ekValueAddRefNote(Y, v, "ekContextGetArgs leftovers");
        }
    }

    if(argsTaken != argCount)
    {
        // too many args!
        return ekFalse;
    }

    va_end(args);
    ekContextPopValues(Y, argCount);
    return ekTrue;
}

// TODO: reuse code between ekContextArgsFailure and ekContextSetError
int ekContextArgsFailure(struct ekContext *Y, int argCount, const char *errorFormat, ...)
{
    va_list args;
    char tempStr[MAX_ERROR_LENGTH + 1];

    ekContextClearError(Y);
    va_start(args, errorFormat);
    vsprintf(tempStr, errorFormat, args);
    va_end(args);

    Y->errorType = YVE_RUNTIME;
    Y->error = ekStrdup(Y, tempStr);

    ekContextPopValues(Y, argCount);
    return 0;
}

#ifdef EUREKA_TRACE_EXECUTION
static const char *ekValueDebugString(struct ekContext *Y, ekValue *v)
{
    static char buffer[2048];
    static char valString[2048];

    valString[0] = 0;
    switch(v->type)
    {
        case YVT_INT:
            sprintf(valString, "(%d)", v->intVal);
            break;
        case YVT_FLOAT:
            sprintf(valString, "(%2.2f)", v->floatVal);
            break;
        case YVT_STRING:
            sprintf(valString, "(%s)", ekStringSafePtr(&v->stringVal));
            break;
        case YVT_ARRAY:
            sprintf(valString, "(count: %d)", (int)ekArraySize(Y, &v->arrayVal));
            break;
    }

    sprintf(buffer, "[%02d] %s 0x%p %s", v->refs, ekValueTypeName(Y, v->type), v, valString);
    return buffer;
}

static void ekContextLogState(ekContext *Y)
{
    int i;

    ekTraceExecution(("\n\n\n------------------------------------------\n"));

    if(ekArraySize(Y, &Y->frames) > 0)
    {
        ekFrame *frame = ekArrayTop(Y, &Y->frames);
        ekTraceExecution(("0x%p [cleanup:%d][lastRet:%d] IP: ", frame, frame->cleanupCount, Y->lastRet));
        ekOpsDump(frame->ip, 1);
    }

    ekTraceExecution(("\n"));

    ekTraceExecution(("-- Stack Top --\n"));
    for(i=0; i<ekArraySize(Y, &Y->stack); i++)
    {
        ekValue *v = (ekValue *)Y->stack[ekArraySize(Y, &Y->stack) - 1 - i];
        ekTraceExecution(("%2.2d: %s\n", i, ekValueDebugString(Y, v)));
    }
    ekTraceExecution(("-- Stack Bot --\n"));
}
#endif

void ekContextLoop(struct ekContext *Y, ekBool stopAtPop)
{
    ekFrame *frame = ekArrayTop(Y, &Y->frames);
    ekBool continueLooping = ekTrue;
    ekBool newFrame;
    ekOpcode opcode;
    ekOperand operand;
    ekU32 startingFrameCount = ekArraySize(Y, &Y->frames);

    if(!frame)
    {
        ekContextSetError(Y, YVE_RUNTIME, "ekContextLoop(): No stack frame!");
        return;
    }

    // Main VM loop!
    while(continueLooping && !Y->error)
    {
        if(stopAtPop && (ekArraySize(Y, &Y->frames) < startingFrameCount))
        {
            break;
        }

        newFrame = ekFalse;

        // These are put into temporary variables for future ntohs() cross-platform safety
        opcode  = frame->ip->opcode;
        operand = frame->ip->operand;

#ifdef EUREKA_TRACE_EXECUTION
        ekContextLogState(Y);
#endif
        switch(opcode)
        {
            case YOP_NOP:
                break;

            case YOP_START:
                break;

            case YOP_AND:
            case YOP_OR:
            {
                int i;
                ekValue *performSkipValue = ekArrayTop(Y, &Y->stack);
                ekBool performSkip = ekFalse;
                if(!performSkipValue)
                {
                    ekContextSetError(Y, YVE_RUNTIME, "YOP_SKIP: empty stack!");
                }
                ekValueAddRefNote(Y, performSkipValue, "YOP_AND/YOP_OR skip value staying on top of stack");
                performSkipValue = ekValueToBool(Y, performSkipValue);
                performSkip = (performSkipValue->intVal) ? ekTrue : ekFalse;
                ekValueRemoveRefNote(Y, performSkipValue, "removing skip value in bool form");
                if(opcode == YOP_AND)
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
                    ekValue *p = ekArrayPop(Y, &Y->stack);
                    ekValueRemoveRefNote(Y, p, "YOP_AND/YOP_OR popping skip value");
                }
            }
            break;

            case YOP_PUSHNULL:
            {
                ekArrayPush(Y, &Y->stack, &ekValueNull);
            }
            break;

            case YOP_PUSHTHIS:
            {
                ekArrayPush(Y, &Y->stack, ekContextThis(Y)); // ekContextThis +refs
            }
            break;

            case YOP_PUSHI:
            {
                ekValue *value = ekValueCreateInt(Y, operand);
                ekArrayPush(Y, &Y->stack, value);
            }
            break;

            case YOP_PUSH_KB:
            {
                ekValue *value = ekValueCreateFunction(Y, frame->block->chunk->blocks[operand]);
                ekArrayPush(Y, &Y->stack, value);
            }
            break;

            case YOP_PUSH_KI:
            {
                ekValue *value = ekValueCreateInt(Y, frame->block->chunk->kInts[operand]);
                ekArrayPush(Y, &Y->stack, value);
            }
            break;

            case YOP_PUSH_KF:
            {
                ekValue *value = ekValueCreateFloat(Y, *((ekF32 *)&frame->block->chunk->kFloats[operand]));
                ekArrayPush(Y, &Y->stack, value);
            }
            break;

            case YOP_PUSH_KS:
            {
                ekValue *value;
                if(frame->block->chunk->temporary)
                {
                    value = ekValueCreateString(Y, frame->block->chunk->kStrings[operand]);
                }
                else
                {
                    value = ekValueCreateKString(Y, frame->block->chunk->kStrings[operand]);
                }
                ekArrayPush(Y, &Y->stack, value);
            }
            break;

            case YOP_VARREG_KS:
            {
                ekValue **valueRef = ekContextRegister(Y, frame->block->chunk->kStrings[operand], ekValueNullPtr);
                ekContextPushRef(Y, valueRef);
            }
            break;

            case YOP_VARREF_KS:
            {
                ekValue **valueRef = ekContextResolve(Y, frame->block->chunk->kStrings[operand]);
                if(valueRef)
                {
                    ekContextPushRef(Y, valueRef);
                }
                else
                {
                    ekContextSetError(Y, YVE_RUNTIME, "YOP_GETVAR_KS: no variable named '%s'", frame->block->chunk->kStrings[operand]);
                }
            }
            break;

            case YOP_REFVAL:
            {
                ekValue *value = ekArrayPop(Y, &Y->stack);
                if(!value)
                {
                    ekContextSetError(Y, YVE_RUNTIME, "YOP_REFVAL: empty stack!");
                    break;
                };
                if(value->type != YVT_REF)
                {
                    ekContextSetError(Y, YVE_RUNTIME, "YOP_REFVAL: requires ref on top of stack");
                    break;
                }
                ekArrayPush(Y, &Y->stack, *value->refVal);
                ekValueAddRefNote(Y, *value->refVal, "RefVal value");
                ekValueRemoveRefNote(Y, value, "RefVal ref");
            }
            break;

            case YOP_ADD:
            {
                ekValue *b = ekArrayPop(Y, &Y->stack);
                ekValue *a = ekArrayPop(Y, &Y->stack);
                ekValue *c = ekValueAdd(Y, a, b);
                ekValueRemoveRefNote(Y, b, "add operand 2");
                ekValueRemoveRefNote(Y, a, "add operand 1");
                if(c)
                {
                    ekArrayPush(Y, &Y->stack, c);
                }
                else
                {
                    continueLooping = ekFalse;
                }
            }
            break;

            case YOP_SUB:
            {
                ekValue *b = ekArrayPop(Y, &Y->stack);
                ekValue *a = ekArrayPop(Y, &Y->stack);
                ekValue *c = ekValueSub(Y, a, b);
                if(operand)
                {
                    // Leave entries on the stack. Used in for loops.
                    ekArrayPush(Y, &Y->stack, a);
                    ekArrayPush(Y, &Y->stack, b);
                }
                else
                {
                    ekValueRemoveRefNote(Y, b, "sub operand 2");
                    ekValueRemoveRefNote(Y, a, "sub operand 1");
                }
                if(c)
                {
                    ekArrayPush(Y, &Y->stack, c);
                }
                else
                {
                    continueLooping = ekFalse;
                }
            }
            break;

            case YOP_MUL:
            {
                ekValue *b = ekArrayPop(Y, &Y->stack);
                ekValue *a = ekArrayPop(Y, &Y->stack);
                ekValue *c = ekValueMul(Y, a, b);
                ekValueRemoveRefNote(Y, b, "mul operand 2");
                ekValueRemoveRefNote(Y, a, "mul operand 1");
                if(c)
                {
                    ekArrayPush(Y, &Y->stack, c);
                }
                else
                {
                    continueLooping = ekFalse;
                }
            }
            break;

            case YOP_DIV:
            {
                ekValue *b = ekArrayPop(Y, &Y->stack);
                ekValue *a = ekArrayPop(Y, &Y->stack);
                ekValue *c = ekValueDiv(Y, a, b);
                ekValueRemoveRefNote(Y, b, "div operand 2");
                ekValueRemoveRefNote(Y, a, "div operand 1");
                if(c)
                {
                    ekArrayPush(Y, &Y->stack, c);
                }
                else
                {
                    continueLooping = ekFalse;
                }
            }
            break;

            case YOP_CMP:
            case YOP_EQUALS:
            case YOP_NOTEQUALS:
            case YOP_LESSTHAN:
            case YOP_LESSTHANOREQUAL:
            case YOP_GREATERTHAN:
            case YOP_GREATERTHANOREQUAL:
            {
                ekValue *b = ekArrayPop(Y, &Y->stack);
                ekValue *a = ekArrayPop(Y, &Y->stack);
                ekS32 cmp = ekValueCmp(Y, a, b);
                ekValueRemoveRefNote(Y, b, "cmp operand 2");
                ekValueRemoveRefNote(Y, a, "cmp operand 1");
                if(opcode != YOP_CMP)
                {
                    if((opcode == YOP_EQUALS) || (opcode == YOP_NOTEQUALS))
                    {
                        // Boolean tests
                        cmp = !cmp;
                        if(opcode == YOP_NOTEQUALS)
                        {
                            cmp = !cmp;
                        }
                    }
                    else
                    {
                        // Comparisons-to-bool
                        switch(opcode)
                        {
                            case YOP_LESSTHAN:
                                cmp = (cmp < 0) ? 1 : 0;
                                break;
                            case YOP_LESSTHANOREQUAL:
                                cmp = (cmp <= 0) ? 1 : 0;
                                break;
                            case YOP_GREATERTHAN:
                                cmp = (cmp > 0) ? 1 : 0;
                                break;
                            case YOP_GREATERTHANOREQUAL:
                                cmp = (cmp >= 0) ? 1 : 0;
                                break;
                        }
                    }
                }
                ekArrayPush(Y, &Y->stack, ekValueCreateInt(Y, cmp));
            }
            break;

            case YOP_SETVAR:
            {
                ekValue *ref = ekArrayPop(Y, &Y->stack);
                ekValue *val = ekArrayTop(Y, &Y->stack);
                if(!operand)
                {
                    ekArrayPop(Y, &Y->stack);
                }
                continueLooping = ekValueSetRefVal(Y, ref, val);
                ekValueRemoveRefNote(Y, ref, "SETVAR temporary reference");
                if(!operand)
                {
                    ekValueRemoveRefNote(Y, val, "SETVAR value not needed anymore");
                }
            }
            break;

            case YOP_INHERITS:
            {
                ekValue *l = ekArrayPop(Y, &Y->stack);
                ekValue *r = ekArrayPop(Y, &Y->stack);
                ekBool inherits = ekValueTestInherits(Y, l, r);
                ekValueRemoveRefNote(Y, r, "inherits operand 2");
                ekValueRemoveRefNote(Y, l, "inherits operand 1");

                ekArrayPush(Y, &Y->stack, ekValueCreateInt(Y, inherits ? 1 : 0));
            }
            break;

            case YOP_VARARGS:
            {
                int i;
                int varargCount = frame->argCount - operand;
                ekValue *varargsArray = ekValueCreateArray(Y);

                // Only one of these for loops will actually loop
                for(; varargCount < 0; varargCount++)
                {
                    ekArrayPush(Y, &Y->stack, &ekValueNull);
                }
                for(; varargCount > 0; varargCount--)
                {
                    ekArrayUnshift(Y, &varargsArray->arrayVal, ekArrayPop(Y, &Y->stack));
                }

                ekArrayPush(Y, &Y->stack, varargsArray);
            }
            break;

            case YOP_INDEX:
            {
                ekValue *index = ekArrayPop(Y, &Y->stack);
                ekValue *value = ekArrayPop(Y, &Y->stack);
                if(value && index)
                {
                    int opFlags = operand;
                    ekValue *ret = ekValueIndex(Y, value, index, (opFlags & YOF_LVALUE) ? ekTrue : ekFalse);
                    if(ret)
                    {
                        ekArrayPush(Y, &Y->stack, ret); // +ref implicit in ekValueIndex
                        if(opFlags & YOF_PUSHOBJ)
                        {
                            ekValueAddRefNote(Y, value, "INDEX + YOF_PUSHOBJ");
                            ekArrayPush(Y, &Y->stack, value);
                        }
                        else if(opFlags & YOF_PUSHTHIS)
                        {
                            ekArrayPush(Y, &Y->stack, ekContextThis(Y)); // +ref from ekContextThis
                        }
                    }
                    else
                    {
                        if(Y->errorType == YVE_NONE)
                        {
                            ekContextSetError(Y, YVE_RUNTIME, "YOP_INDEX: Failed attempt to index into type %s", ekValueTypeName(Y, value->type));
                        }
                        continueLooping = ekFalse;
                    }
                }
                else
                {
                    ekContextSetError(Y, YVE_RUNTIME, "YOP_INDEX: empty stack!");
                    continueLooping = ekFalse;
                }
                if(value)
                {
                    ekValueRemoveRefNote(Y, value, "INDEX value done");
                }
                if(index)
                {
                    ekValueRemoveRefNote(Y, index, "INDEX index done");
                }
            }
            break;

            case YOP_DUPE:
            case YOP_MOVE:
            {
                int topIndex = ekArraySize(Y, &Y->stack) - 1;
                int requestedIndex = topIndex - operand;
                if(requestedIndex >= 0)
                {
                    ekValue *val = Y->stack[requestedIndex];
                    if(opcode == YOP_MOVE)
                    {
                        if(operand == 0)
                        {
                            break;    // no sense in moving the top to the top
                        }
                        Y->stack[requestedIndex] = NULL;
                        ekArraySquash(Y, &Y->stack);
                    }
                    else
                    {
                        ekValueAddRefNote(Y, val, "DUPE");
                    }
                    ekArrayPush(Y, &Y->stack, val);
                }
                else
                {
                    ekContextSetError(Y, YVE_RUNTIME, "%s: impossible index", (opcode == YOP_DUPE) ? "YOP_DUPE" : "YOP_MOVE");
                    continueLooping = ekFalse;
                }
            }
            break;

            case YOP_POP:
            {
                int i;
                for(i = 0; i < operand; i++)
                {
                    ekValue *v = ekArrayPop(Y, &Y->stack);
                    ekValueRemoveRefNote(Y, v, "POP");
                }
            }
            break;

            case YOP_CALL:
            {
                int argCount = operand;
                ekFrame *oldFrame = frame;
                ekValue *thisVal = ekArrayPop(Y, &Y->stack);
                ekValue *callable = ekArrayPop(Y, &Y->stack);
                continueLooping = ekContextCall(Y, &frame, thisVal, callable, argCount);
                ekValueRemoveRefNote(Y, thisVal, "CALL this done");
                ekValueRemoveRefNote(Y, callable, "CALL callable done");
                if(frame != oldFrame)
                {
                    newFrame = ekTrue;
                }
            }
            break;

            case YOP_RET:
            {
                int argCount = operand;
                frame = ekContextPopFrames(Y, YFT_FUNC, ekTrue);
                //                if(!argCount && frame && (frame->flags & YFF_INIT) && frame->thisVal)
                //                {
                //                    // We are leaving an init function that does not return any value (which
                //                    // is different from returning null). This bit of magic makes init() functions
                //                    // return 'this' instead of nothing. It avoids the boilerplate 'return this'
                //                    // at the end of every init() in order for it to work at all, but allows for
                //                    // generator init functions.
                //                    argCount = 1;
                //                    ekArrayPush(Y, &Y->stack, frame->thisVal);
                //                }
                frame = ekContextPopFrames(Y, YFT_FUNC, ekFalse);
                if(frame)
                {
                    // Stash lastRet for any YOP_KEEPs in the pipeline
                    Y->lastRet = argCount;
                }
                else
                {
                    // Throw away return values and bail out of loop
                    continueLooping = ekFalse;
                }
            };
            break;

            case YOP_CLEANUP:
            {
                frame->cleanupCount += operand;
                Y->lastRet = 0; // reset this here in case as a 'normal' termination of a for loop won't ever set it, but it is legal to override it with a return
            }
            break;

            case YOP_KEEP:
            {
                int keepCount = operand;
                int offerCount = Y->lastRet;

                if(keepCount < offerCount)
                {
                    int i;
                    for(i = 0; i < (offerCount - keepCount); i++)
                    {
                        ekTraceExecution(("-- cleaning stack entry --\n"));
                        ekValue *v = ekArrayPop(Y, &Y->stack);
                        ekValueRemoveRefNote(Y, v, "KEEP cleaning stack");
                    }
                }
                else if(keepCount > offerCount)
                {
                    int i;
                    for(i = 0; i < (keepCount - offerCount); i++)
                    {
                        ekTraceExecution(("-- padding stack with null --\n"));
                        ekArrayPush(Y, &Y->stack, &ekValueNull);
                    }
                }
                Y->lastRet = 0;
            }
            break;

            case YOP_CLOSE:
            {
                ekValue *v = ekArrayTop(Y, &Y->stack);
                ekValueAddClosureVars(Y, v);
            }
            break;

            case YOP_IF:
            {
                ekBlock *block = NULL;
                ekValue *cond, *ifBody, *elseBody = NULL;
                cond   = ekArrayPop(Y, &Y->stack);
                ifBody = ekArrayPop(Y, &Y->stack);
                if(operand)
                {
                    elseBody = ekArrayPop(Y, &Y->stack);
                }
                // TODO: verify ifBody/elseBody are YVT_BLOCK
                cond = ekValueToBool(Y, cond);
                if(cond->intVal)
                {
                    block = ifBody->blockVal;
                }
                else if(elseBody)
                {
                    block = elseBody->blockVal;
                }

                if(block)
                {
                    frame = ekContextPushFrame(Y, block, 0, YFT_COND, NULL, NULL);
                    if(frame)
                    {
                        newFrame = ekTrue;
                    }
                    else
                    {
                        continueLooping = ekFalse;
                    }
                }

                ekValueRemoveRefNote(Y, cond, "IF cond done");
                ekValueRemoveRefNote(Y, ifBody, "IF ifBody done");
                if(elseBody)
                {
                    ekValueRemoveRefNote(Y, elseBody, "IF elseBody done");
                }
            }
            break;

            case YOP_ENTER:
            {
                ekValue *blockRef = ekArrayPop(Y, &Y->stack);

                if(blockRef)
                {
                    if(blockRef->type == YVT_BLOCK && blockRef->blockVal)
                    {
                        ekU32 frameType = operand;
                        frame = ekContextPushFrame(Y, blockRef->blockVal, 0, frameType, NULL, NULL);
                        if(frame)
                        {
                            newFrame = ekTrue;
                        }
                        else
                        {
                            continueLooping = ekFalse;
                        }
                    }
                    ekValueRemoveRefNote(Y, blockRef, "ENTER: removing block ref");
                }
                else
                {
                    ekContextSetError(Y, YVE_RUNTIME, "hurr");
                    continueLooping = ekFalse;
                }
            }
            break;

            case YOP_LEAVE:
            {
                ekBool performLeave = ekTrue;
                if(operand)
                {
                    ekValue *cond = ekArrayPop(Y, &Y->stack);
                    cond = ekValueToBool(Y, cond);
                    performLeave   = !cond->intVal; // don't leave if expr is true!
                    ekValueRemoveRefNote(Y, cond, "LEAVE cond done");
                }

                if(performLeave)
                {
                    frame = ekContextPopFrames(Y, YFT_ANY, ekFalse);
                    continueLooping = (frame) ? ekTrue : ekFalse;
                }
            }
            break;

            case YOP_BREAK:
            {
                // a C-style break. Find the innermost loop and kill it.
                frame = ekContextPopFrames(Y, YFT_LOOP, ekFalse);
                continueLooping = (frame) ? ekTrue : ekFalse;
            }
            break;

            case YOP_CONTINUE:
            {
                // a C-style continue. Find the innermost loop and reset it.
                frame = ekContextPopFrames(Y, YFT_LOOP, ekTrue);
                if(frame)
                {
                    ekFrameReset(Y, frame, ekTrue);
                    newFrame = ekTrue;
                }
                else
                {
                    continueLooping = ekFalse;
                }
            }
            break;

            case YOP_NOT:
            {
                ekValue *not;
                ekValue *value = ekArrayPop(Y, &Y->stack);
                if(!value)
                {
                    ekContextSetError(Y, YVE_RUNTIME, "YOP_NOT: empty stack!");
                    continueLooping = ekFalse;
                    break;
                };
                value = ekValueToBool(Y, value);
                not = ekValueCreateInt(Y, !value->intVal); // Double temporary?
                ekArrayPush(Y, &Y->stack, not);
                ekValueRemoveRefNote(Y, value, "NOT value done");
            }
            break;

            case YOP_SHIFTLEFT:
            case YOP_SHIFTRIGHT:
            case YOP_BITWISE_NOT:
            case YOP_BITWISE_XOR:
            case YOP_BITWISE_AND:
            case YOP_BITWISE_OR:
            {
                int i[2];
                int ret;
                int argsNeeded = (opcode == YOP_BITWISE_NOT) ? 1 : 2;
                if(ekContextPopInts(Y, argsNeeded, i) != argsNeeded)
                {
                    ekContextSetError(Y, YVE_RUNTIME, "Bitwise operations require integer friendly arguments");
                    continueLooping = ekFalse;
                    break;
                }

                switch(opcode)
                {
                    case YOP_SHIFTLEFT:   ret =  i[0] << i[1]; break;
                    case YOP_SHIFTRIGHT:  ret =  i[0] >> i[1]; break;
                    case YOP_BITWISE_NOT: ret = ~i[0];         break;
                    case YOP_BITWISE_XOR: ret =  i[0] ^  i[1]; break;
                    case YOP_BITWISE_AND: ret =  i[0] &  i[1]; break;
                    case YOP_BITWISE_OR:  ret =  i[0] |  i[1]; break;
                }
                ekArrayPush(Y, &Y->stack, ekValueCreateInt(Y, ret));
            }
            break;

            case YOP_FORMAT:
            {
                ekValue *format = ekArrayPop(Y, &Y->stack);
                ekValue *val;
                if(!format)
                {
                    ekContextSetError(Y, YVE_RUNTIME, "YOP_FORMAT: empty stack!");
                    continueLooping = ekFalse;
                    break;
                };
                val = ekValueStringFormat(Y, format, operand);
                if(!val)
                {
                    ekContextSetError(Y, YVE_RUNTIME, "YOP_FORMAT: bad format");
                    continueLooping = ekFalse;
                    break;
                };
                ekArrayPush(Y, &Y->stack, val);
            }
            break;

            case YOP_NTH:
            {
                ekValue *val = ekArrayPop(Y, &Y->stack);

                if(val->type == YVT_ARRAY)
                {
                    ekValue *nth = ekArrayPop(Y, &Y->stack);
                    if(nth->intVal >= 0 && nth->intVal < ekArraySize(Y, &val->arrayVal))
                    {
                        ekValue *indexedValue = val->arrayVal[nth->intVal];
                        ekValueAddRefNote(Y, indexedValue, "NTH indexed value");
                        ekArrayPush(Y, &Y->stack, indexedValue);
                        Y->lastRet = 1;
                    }
                    else
                    {
                        ekContextSetError(Y, YVE_RUNTIME, "YOP_NTH: index out of range");
                        continueLooping = ekFalse;
                    }
                    ekValueRemoveRefNote(Y, nth, "NTH nth done");
                }
                else if(val->type == YVT_OBJECT)
                {
                    ekFrame *oldFrame = frame;
                    ekValue *getFunc = ekFindFunc(Y, val, "get");
                    if(getFunc)
                    {
                        continueLooping = ekContextCall(Y, &frame, val, getFunc, 1 /* the index */);
                        if(frame != oldFrame)
                        {
                            newFrame = ekTrue;
                        }
                    }
                    else
                    {
                        ekContextSetError(Y, YVE_RUNTIME, "YVT_NTH: iterable does not have a get() function");
                        continueLooping = ekFalse;
                    }
                }
                else
                {
                    ekContextSetError(Y, YVE_RUNTIME, "YOP_NTH: Invalid value type %d", val->type);
                    continueLooping = ekFalse;
                }
                ekValueRemoveRefNote(Y, val, "NTH val done");
            }
            break;

            case YOP_COUNT:
            {
                ekValue *val = ekArrayPop(Y, &Y->stack);
                if(val->type == YVT_ARRAY)
                {
                    ekValue *count = ekValueCreateInt(Y, ekArraySize(Y, &val->arrayVal));
                    ekArrayPush(Y, &Y->stack, count);
                    Y->lastRet = 1;
                }
                else if(val->type == YVT_OBJECT)
                {
                    ekFrame *oldFrame = frame;
                    ekValue *countFunc = ekFindFunc(Y, val, "count");
                    if(countFunc)
                    {
                        continueLooping = ekContextCall(Y, &frame, val, countFunc, 0);
                        if(frame != oldFrame)
                        {
                            newFrame = ekTrue;
                        }
                    }
                    else
                    {
                        ekContextSetError(Y, YVE_RUNTIME, "YVT_COUNT: iterable does not have a count() function");
                        continueLooping = ekFalse;
                    }
                }
                else
                {
                    ekContextSetError(Y, YVE_RUNTIME, "YVT_COUNT: Invalid value type %d", val->type);
                    continueLooping = ekFalse;
                }
                ekValueRemoveRefNote(Y, val, "COUNT val done");
            }
            break;

            default:
                ekContextSetError(Y, YVE_RUNTIME, "Unknown VM Opcode: %d", opcode);
                continueLooping = ekFalse;
                break;
        }

        if(continueLooping && !newFrame)
        {
            frame->ip++;
        }
    }
}
