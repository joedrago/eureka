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
#include "ekIntrinsics.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_ERROR_LENGTH 1023

void ekContextRegisterGlobal(struct ekContext *E, const char *name, ekValue *value)
{
    ekMapGetS2P(E, E->globals, name) = value;
    ekValueAddRefNote(E, value, "ekContextRegisterGlobal");
}

void ekContextRegisterGlobalFunction(struct ekContext *E, const char *name, ekCFunction func)
{
    ekValue *p = ekValueCreateCFunction(E, func);
    ekContextRegisterGlobal(E, name, p);
    ekValueRemoveRefNote(E, p, "granted ownership to globals table");
}

static ekBool ekChunkCanBeTemporary(ekChunk *chunk)
{
    // A simple test ... if there aren't any functions in this chunk and all ktable lookups
    // are duped (chunk flagged as temporary), than the chunk can safely go away in between
    // calls to ekContextLoop (assuming zero ekFrames on the stack).
    return !chunk->hasFuncs;
}

void ekContextEval(struct ekContext *E, const char *text, ekU32 evalOpts)
{
    ekChunk *chunk;
    ekCompiler *compiler;
    ekU32 compileFlags = ECO_DEFAULT;

    if(evalOpts & EEO_OPTIMIZE)
    {
        compileFlags |= ECO_OPTIMIZE;
    }

    compiler = ekCompilerCreate(E);
    ekCompile(compiler, text, compileFlags);

    if(ekArraySize(E, &compiler->errors))
    {
        int i;
        int total = 0;
        for(i = 0; i < ekArraySize(E, &compiler->errors); i++)
        {
            char *error = (char *)compiler->errors[i];
            total += strlen(error) + 3; // "* " + newline
        }
        if(total > 0)
        {
            char *s = (char *)ekAlloc(total+1);
            for(i = 0; i < ekArraySize(E, &compiler->errors); i++)
            {
                char *error = compiler->errors[i];
                strcat(s, "* ");
                strcat(s, error);
                strcat(s, "\n");
            }
            ekContextSetError(E, EVE_COMPILE, s);
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

            if(evalOpts & EEO_DUMP)
            {
#ifdef EUREKA_ENABLE_EXT_DISASM
                ekChunkDump(E, chunk);
#else
                ekAssert(0 && "Requesting disasm dump when disasm support is disabled");
#endif
            }

#ifdef EUREKA_TRACE_EXECUTION
            ekTraceExecution(("--- begin chunk execution ---\n"));
#endif
            // Execute the chunk's block
            ekContextPushFrame(E, chunk->block, 0, EFT_FUNC|EFT_CHUNK, ekValueNullPtr, NULL);
            ekContextLoop(E, ekTrue);

#ifdef EUREKA_TRACE_EXECUTION
            ekTraceExecution(("---  end  chunk execution ---\n"));
#endif
            if(!chunk->temporary)
            {
                ekArrayPush(E, &E->chunks, chunk);
                chunk = NULL; // forget the ptr
            }
        }

        if(chunk)
        {
            ekChunkDestroy(E, chunk);
        }
    }
}

ekContext *ekContextCreate(ekMemFuncs *memFuncs)
{
    ekContext *E = ekDefaultAlloc(sizeof(ekContext));
    E->allocFunc = ekDefaultAlloc;
    E->reallocFunc = ekDefaultRealloc;
    E->freeFunc = ekDefaultFree;
    // LCOV_EXCL_START - I don't care about testing other allocators.
    if(memFuncs)
    {
        if(memFuncs->allocFunc)
        {
            E->allocFunc = memFuncs->allocFunc;
        }
        if(memFuncs->reallocFunc)
        {
            E->reallocFunc = memFuncs->reallocFunc;
        }
        if(memFuncs->freeFunc)
        {
            E->freeFunc = memFuncs->freeFunc;
        }
    }
    // LCOV_EXCL_STOP
    E->globals = ekMapCreate(E, EMKT_STRING);

    ekValueTypeRegisterAllBasicTypes(E);
    ekIntrinsicsRegister(E);
    ekModuleRegisterAll(E);
    return E;
}

// LCOV_EXCL_START - TODO: ektest embedding
void ekContextRecover(ekContext *E)
{
    if(E->errorType == EVE_RUNTIME)
    {
        int prevStackCount = 0;

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

void ekContextDestroy(ekContext *E)
{
    ekMapDestroy(E, E->globals, ekValueRemoveRefHashed);
    ekArrayDestroy(E, &E->frames, (ekDestroyCB)ekFrameDestroy);
    ekArrayDestroy(E, &E->stack, (ekDestroyCB)ekValueRemoveRefHashed);
    ekArrayDestroy(E, &E->chunks, (ekDestroyCB)ekChunkDestroy);

    ekArrayDestroy(E, &E->freeValues, (ekDestroyCB)ekValueDestroy);

    ekArrayDestroy(E, &E->types, (ekDestroyCB)ekValueTypeDestroy);
    ekContextClearError(E);

    ekFree(E);
}

static ekValue **ekContextResolve(struct ekContext *E, const char *name)
{
    int i;
    ekFrame *frame;
    ekValue **valueRef;
    ekMapEntry *hashEntry;

    for(i = ekArraySize(E, &E->frames) - 1; i >= 0; i--)
    {
        frame = E->frames[i];

        // Check the locals
        hashEntry = ekMapGetS(E, frame->locals, name, ekFalse);
        if(hashEntry) { return (ekValue **)&hashEntry->valuePtr; }

        // Check closure vars
        if(frame->closure && frame->closure->closureVars)
        {
            hashEntry = ekMapGetS(E, frame->closure->closureVars, name, ekFalse);
            if(hashEntry) { return (ekValue **)&hashEntry->valuePtr; }
        }

        if(frame->type & EFT_FUNC)
        {
            break;
        }
    }

    // check globals
    hashEntry = ekMapGetS(E, E->globals, name, ekFalse);
    if(hashEntry) { return (ekValue **)&hashEntry->valuePtr; }

    return NULL;
}

ekFrame *ekContextPushFrame(struct ekContext *E, ekBlock *block, int argCount, ekU32 frameType, struct ekValue *thisVal, ekValue *closure)
{
    ekFrame *frame;
    int i;

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
            int i;
            for(i = 0; i < (argCount - block->argCount); i++)
            {
                ekValue *v = ekArrayPop(E, &E->stack);
                ekValueRemoveRefNote(E, v, "removing unused args");
            }
        }
        else if(block->argCount > argCount)
        {
            // Too few arguments -- pad with nulls
            int i;
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

static ekBool ekContextCallCFunction(struct ekContext *E, ekCFunction func, ekU32 argCount, ekValue *thisVal);
static ekBool ekContextCreateObject(struct ekContext *E, ekFrame **framePtr, ekValue *isa, int argCount);

static ekBool ekContextCall(struct ekContext *E, ekFrame **framePtr, ekValue *thisVal, ekValue *callable, int argCount)
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
        if(!ekContextCallCFunction(E, *callable->cFuncVal, argCount, thisVal))
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

static ekValue *ekFindFunc(struct ekContext *E, ekValue *object, const char *name)
{
    ekValue *v = *(ekObjectGetRef(E, object->objectVal, name, ekFalse));
    if(v == &ekValueNull)
    {
        if(object->objectVal->isa)
        {
            return ekFindFunc(E, object->objectVal->isa, name);
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
ekBool ekContextCallFuncByName(struct ekContext *E, ekValue *thisVal, const char *name, int argCount)
{
    ekFrame *frame = NULL;
    ekValue **valueRef = ekContextResolve(E, name);
    if(!valueRef || !(*valueRef))
    {
        return ekFalse;
    }
    if(ekContextCall(E, &frame, thisVal, *valueRef, argCount))
    {
        ekContextLoop(E, ekTrue);
        return ekTrue;
    }
    return ekFalse;
}
// LCOV_EXCL_STOP

static ekBool ekContextCreateObject(struct ekContext *E, ekFrame **framePtr, ekValue *isa, int argCount)
{
    ekBool ret = ekTrue;
    ekValue *initFunc = ekFindFunc(E, isa, "init");
    ekValue *newObject = ekValueCreateObject(E, isa, (initFunc) ? 0 : argCount, ekFalse);

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

// TODO: this needs to protect against variable masking/shadowing
static ekValue **ekContextRegister(struct ekContext *E, const char *name, ekValue *value)
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
    return valueRef;
}

static void ekContextPushRef(struct ekContext *E, ekValue **valueRef)
{
    ekValue *value = ekValueCreateRef(E, valueRef);
    ekArrayPush(E, &E->stack, value);
}

// TODO: merge this function with PushFrame and _RET
static ekBool ekContextCallCFunction(struct ekContext *E, ekCFunction func, ekU32 argCount, ekValue *thisVal)
{
    int retCount;
    ekFrame *frame = ekFrameCreate(E, EFT_FUNC, thisVal, NULL, ekArraySize(E, &E->stack), argCount, NULL);
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

// LCOV_EXCL_START - I don't care about testing this yet.
static ekContextFrameCleanup(struct ekContext *E, ekFrame *frame)
{
    if(frame->cleanupCount)
    {
        int i;
        for(i=0; i < frame->cleanupCount; i++)
        {
            int index = ((ekArraySize(E, &E->stack) - 1) - E->lastRet) - i;
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

static ekS32 ekContextPopInts(struct ekContext *E, int count, int *output)
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
    int i;
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

ekBool ekContextGetArgs(struct ekContext *E, int argCount, const char *argFormat, ...)
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

        switch(*c)
        {
            default:
            case '?': /* can be anything */                    break;
            case 'n': if(v->type != EVT_NULL) { return ekFalse; } break;
            case 's': if(v->type != EVT_STRING) { return ekFalse; } break;
            case 'i': if(v->type != EVT_INT) { return ekFalse; } break;
            case 'f': if(v->type != EVT_FLOAT) { return ekFalse; } break;
            case 'a': if(v->type != EVT_ARRAY) { return ekFalse; } break;
            case 'm': if(v->type != EVT_OBJECT) { return ekFalse; } break;  // "map"
            case 'o': if(v->type != EVT_OBJECT) { return ekFalse; } break;
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
int ekContextArgsFailure(struct ekContext *E, int argCount, const char *errorFormat, ...)
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
            sprintf(valString, "(count: %d)", (int)ekArraySize(E, &v->arrayVal));
            break;
    }

    sprintf(buffer, "[%02d] %s 0x%p %s", v->refs, ekValueTypeName(E, v->type), v, valString);
    return buffer;
}

static void ekContextLogState(ekContext *E)
{
    int i;

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

void ekContextLoop(struct ekContext *E, ekBool stopAtPop)
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
                int i;
                ekValue *performSkipValue = ekArrayTop(E, &E->stack);
                ekBool performSkip = ekFalse;
                if(!performSkipValue)
                {
                    ekContextSetError(E, EVE_RUNTIME, "EOP_SKIP: empty stack!");
                }
                ekValueAddRefNote(E, performSkipValue, "EOP_AND/EOP_OR skip value staying on top of stack");
                performSkipValue = ekValueToBool(E, performSkipValue);
                performSkip = (performSkipValue->intVal) ? ekTrue : ekFalse;
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

            case EOP_VARREG_KS:
            {
                ekValue **valueRef = ekContextRegister(E, frame->block->chunk->kStrings[operand], ekValueNullPtr);
                ekContextPushRef(E, valueRef);
            }
            break;

            case EOP_VARREF_KS:
            {
                ekValue **valueRef = ekContextResolve(E, frame->block->chunk->kStrings[operand]);
                if(valueRef)
                {
                    ekContextPushRef(E, valueRef);
                }
                else
                {
                    ekContextSetError(E, EVE_RUNTIME, "EOP_GETVAR_KS: no variable named '%s'", frame->block->chunk->kStrings[operand]);
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

            case EOP_SETVAR:
            {
                ekValue *ref = ekArrayPop(E, &E->stack);
                ekValue *val = ekArrayTop(E, &E->stack);
                if(!operand)
                {
                    ekArrayPop(E, &E->stack);
                }
                continueLooping = ekValueSetRefVal(E, ref, val);
                ekValueRemoveRefNote(E, ref, "SETVAR temporary reference");
                if(!operand)
                {
                    ekValueRemoveRefNote(E, val, "SETVAR value not needed anymore");
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
                int i;
                int varargCount = frame->argCount - operand;
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
                    int opFlags = operand;
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
                int topIndex = ekArraySize(E, &E->stack) - 1;
                int requestedIndex = topIndex - operand;
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
                int i;
                for(i = 0; i < operand; i++)
                {
                    ekValue *v = ekArrayPop(E, &E->stack);
                    ekValueRemoveRefNote(E, v, "POP");
                }
            }
            break;

            case EOP_CALL:
            {
                int argCount = operand;
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
                int argCount = operand;
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
                int keepCount = operand;
                int offerCount = E->lastRet;

                if(keepCount < offerCount)
                {
                    int i;
                    for(i = 0; i < (offerCount - keepCount); i++)
                    {
                        ekTraceExecution(("-- cleaning stack entry --\n"));
                        ekValue *v = ekArrayPop(E, &E->stack);
                        ekValueRemoveRefNote(E, v, "KEEP cleaning stack");
                    }
                }
                else if(keepCount > offerCount)
                {
                    int i;
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
                if(operand)
                {
                    ekValue *cond = ekArrayPop(E, &E->stack);
                    cond = ekValueToBool(E, cond);
                    performLeave   = !cond->intVal; // don't leave if expr is true!
                    ekValueRemoveRefNote(E, cond, "LEAVE cond done");
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
                not = ekValueCreateInt(E, !value->intVal); // Double temporary?
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
                int i[2];
                int ret;
                int argsNeeded = (opcode == EOP_BITWISE_NOT) ? 1 : 2;
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
                int i;
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

            case EOP_NTH:
            {
                ekValue *val = ekArrayPop(E, &E->stack);

                if(val->type == EVT_ARRAY)
                {
                    ekValue *nth = ekArrayPop(E, &E->stack);
                    if(nth->intVal >= 0 && nth->intVal < ekArraySize(E, &val->arrayVal))
                    {
                        ekValue *indexedValue = val->arrayVal[nth->intVal];
                        ekValueAddRefNote(E, indexedValue, "NTH indexed value");
                        ekArrayPush(E, &E->stack, indexedValue);
                        E->lastRet = 1;
                    }
                    else
                    {
                        ekContextSetError(E, EVE_RUNTIME, "EOP_NTH: index out of range");
                        continueLooping = ekFalse;
                    }
                    ekValueRemoveRefNote(E, nth, "NTH nth done");
                }
                else if(val->type == EVT_OBJECT)
                {
                    ekFrame *oldFrame = frame;
                    ekValue *getFunc = ekFindFunc(E, val, "get");
                    if(getFunc)
                    {
                        continueLooping = ekContextCall(E, &frame, val, getFunc, 1 /* the index */);
                        if(frame != oldFrame)
                        {
                            newFrame = ekTrue;
                        }
                    }
                    else
                    {
                        ekContextSetError(E, EVE_RUNTIME, "EVT_NTH: iterable does not have a get() function");
                        continueLooping = ekFalse;
                    }
                }
                else
                {
                    ekContextSetError(E, EVE_RUNTIME, "EOP_NTH: Invalid value type %d", val->type);
                    continueLooping = ekFalse;
                }
                ekValueRemoveRefNote(E, val, "NTH val done");
            }
            break;

            case EOP_COUNT:
            {
                ekValue *val = ekArrayPop(E, &E->stack);
                if(val->type == EVT_ARRAY)
                {
                    ekValue *count = ekValueCreateInt(E, ekArraySize(E, &val->arrayVal));
                    ekArrayPush(E, &E->stack, count);
                    E->lastRet = 1;
                }
                else if(val->type == EVT_OBJECT)
                {
                    ekFrame *oldFrame = frame;
                    ekValue *countFunc = ekFindFunc(E, val, "count");
                    if(countFunc)
                    {
                        continueLooping = ekContextCall(E, &frame, val, countFunc, 0);
                        if(frame != oldFrame)
                        {
                            newFrame = ekTrue;
                        }
                    }
                    else
                    {
                        ekContextSetError(E, EVE_RUNTIME, "EVT_COUNT: iterable does not have a count() function");
                        continueLooping = ekFalse;
                    }
                }
                else
                {
                    ekContextSetError(E, EVE_RUNTIME, "EVT_COUNT: Invalid value type %d", val->type);
                    continueLooping = ekFalse;
                }
                ekValueRemoveRefNote(E, val, "COUNT val done");
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
