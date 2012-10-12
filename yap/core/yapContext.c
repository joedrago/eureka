// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapContext.h"

#include "yapBlock.h"
#include "yapChunk.h"
#include "yapCompiler.h"
#include "yapHash.h"
#include "yapObject.h"
#include "yapFrame.h"
#include "yapOp.h"
#include "yapValue.h"

#include "yapmAll.h"
#include "yapiCore.h"
#include "yapiConversions.h"
#include "yapiInheritance.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_ERROR_LENGTH 1023

void yapContextRegisterGlobal(struct yapContext *Y, const char *name, yapValue *value)
{
    yapHashSet(Y, Y->globals, name, value);
}

void yapContextRegisterGlobalFunction(struct yapContext *Y, const char *name, yapCFunction func)
{
    yapContextRegisterGlobal(Y, name, yapValueCreateCFunction(Y, func));
}

static yBool yapChunkCanBeTemporary(yapChunk *chunk)
{
    // A simple test ... if there aren't any functions in this chunk and all ktable lookups
    // are duped (chunk flagged as temporary), than the chunk can safely go away in between
    // calls to yapContextLoop (assuming zero yapFrames on the stack).
    return !chunk->hasFuncs;
}

void yapContextEval(struct yapContext *Y, const char *text, yU32 evalOpts)
{
    yapChunk *chunk;
    yapCompiler *compiler;

    compiler = yapCompilerCreate(Y);
    yapCompile(compiler, text, YCO_DEFAULT);

    if(compiler->errors.count)
    {
        int i;
        int total = 0;
        for(i = 0; i < compiler->errors.count; i++)
        {
            char *error = (char *)compiler->errors.data[i];
            total += strlen(error) + 3; // "* " + newline
        }
        if(total > 0)
        {
            char *s = (char *)yapAlloc(total+1);
            for(i = 0; i < compiler->errors.count; i++)
            {
                char *error = (char *)compiler->errors.data[i];
                strcat(s, "* ");
                strcat(s, error);
                strcat(s, "\n");
            }
            yapContextSetError(Y, YVE_COMPILE, s);
            yapFree(s);
        }
    }

    // take ownership of the chunk
    chunk = compiler->chunk;
    compiler->chunk = NULL;

    yapCompilerDestroy(compiler);

    if(chunk)
    {
        if(chunk->block)
        {
            chunk->temporary = yapChunkCanBeTemporary(chunk);

            if(evalOpts & YEO_DUMP)
            {
#ifdef YAP_ENABLE_EXT_DISASM
                yapChunkDump(chunk);
#else
                yapAssert(0 && "Requesting disasm dump when disasm support is disabled");
#endif
            }

#ifdef YAP_TRACE_EXECUTION
            yapTraceExecution(("--- begin chunk execution ---\n"));
#endif
            // Execute the chunk's block
            yapContextPushFrame(Y, chunk->block, 0, YFT_FUNC|YFT_CHUNK, yapValueNullPtr, NULL);
            yapContextLoop(Y, yTrue);

#ifdef YAP_TRACE_EXECUTION
            yapTraceExecution(("---  end  chunk execution ---\n"));
#endif
            if(!chunk->temporary)
            {
                yapArrayPush(Y, &Y->chunks, chunk);
                chunk = NULL; // forget the ptr
            }
        }

        if(chunk)
        {
            yapChunkDestroy(Y, chunk);
        }
    }
}

yapContext *yapContextCreate(yapMemFuncs *memFuncs)
{
    yapContext *Y = yapDefaultAlloc(sizeof(yapContext));
    Y->allocFunc = yapDefaultAlloc;
    Y->reallocFunc = yapDefaultRealloc;
    Y->freeFunc = yapDefaultFree;
    if(memFuncs)
    {
        if(memFuncs->allocFunc)
            Y->allocFunc = memFuncs->allocFunc;
        if(memFuncs->reallocFunc)
            Y->reallocFunc = memFuncs->reallocFunc;
        if(memFuncs->freeFunc)
            Y->freeFunc = memFuncs->freeFunc;
    }
    Y->globals = yapHashCreate(Y, 0);

    yapValueTypeRegisterAllBasicTypes(Y);
    yapIntrinsicsRegisterCore(Y);
    yapIntrinsicsRegisterConversions(Y);
    yapIntrinsicsRegisterInheritance(Y);
    yapModuleRegisterAll(Y);
    return Y;
}

void yapContextRecover(yapContext *Y)
{
    if(Y->errorType == YVE_RUNTIME)
    {
        int prevStackCount = 0;

        yapFrame *frame = yapContextPopFrames(Y, YFT_CHUNK, yTrue);
        if(frame) // recovery should work on an empty frame stack
        {
            prevStackCount = frame->prevStackCount;
            yapContextPopFrames(Y, YFT_CHUNK, yFalse);
        }
        yapArrayShrink(Y, &Y->stack, prevStackCount, NULL);

        yapContextGC(Y);
    }
    yapContextClearError(Y);
}

void yapContextSetError(struct yapContext *Y, yU32 errorType, const char *errorFormat, ...)
{
    va_list args;
    char tempStr[MAX_ERROR_LENGTH + 1];

    yapContextClearError(Y);
    va_start(args, errorFormat);
    vsprintf(tempStr, errorFormat, args);
    va_end(args);

    Y->errorType = errorType;
    Y->error = yapStrdup(Y, tempStr);
}

void yapContextClearError(yapContext *Y)
{
    if(Y->error)
    {
        yapFree(Y->error);
        Y->error = NULL;
    }
    Y->errorType = YVE_NONE;
}

const char * yapContextGetError(yapContext *Y)
{
    return Y->error;
}

void yapContextDestroy(yapContext *Y)
{
    yapHashDestroy(Y, Y->globals, NULL);
    yapArrayClear(Y, &Y->frames, (yapDestroyCB)yapFrameDestroy);
    yapArrayClear(Y, &Y->stack, NULL);
    yapArrayClear(Y, &Y->chunks, (yapDestroyCB)yapChunkDestroy);

    yapArrayClear(Y, &Y->usedValues, (yapDestroyCB)yapValueDestroy);
    yapArrayClear(Y, &Y->freeValues, (yapDestroyCB)yapValueDestroy);

    yapArrayClear(Y, &Y->types, (yapDestroyCB)yapValueTypeDestroy);
    yapContextClearError(Y);

    yapFree(Y);
}

static yapValue **yapContextResolve(struct yapContext *Y, const char *name)
{
    int i;
    yapFrame *frame;
    yapValue **valueRef;

    for(i = Y->frames.count - 1; i >= 0; i--)
    {
        frame = (yapFrame *)Y->frames.data[i];

        // Check the locals
        valueRef = (yapValue **)yapHashLookup(Y, frame->locals, name, yFalse);
        if(valueRef) return valueRef;

        if(frame->closure && frame->closure->closureVars)
        {
            valueRef = (yapValue **)yapHashLookup(Y, frame->closure->closureVars, name, yFalse);
            if(valueRef) return valueRef;
        }

        if(frame->type == YFT_FUNC)
            break;
    }

    // check globals
    valueRef = (yapValue **)yapHashLookup(Y, Y->globals, name, yFalse);
    if(valueRef) return valueRef;

    return NULL;
}

yapFrame *yapContextPushFrame(struct yapContext *Y, yapBlock *block, int argCount, yU32 frameType, struct yapValue *thisVal, yapValue *closure)
{
    yapFrame *frame;
    int i;

    if(thisVal == NULL)
        thisVal = yapValueNullPtr;

    if(block->argCount != YAV_ALL_ARGS)
    {
        // accomodate the function's arglist by padding/removing stack entries
        if(argCount > block->argCount)
        {
            // Too many arguments passed to this function. Pop some!
            int i;
            for(i = 0; i < (argCount - block->argCount); i++)
                yapArrayPop(Y, &Y->stack);
        }
        else if(block->argCount > argCount)
        {
            // Too few arguments -- pad with nulls
            int i;
            for(i = 0; i < (block->argCount - argCount); i++)
                yapArrayPush(Y, &Y->stack, &yapValueNull);
        }
    }

    frame = yapFrameCreate(Y, frameType, thisVal, block, Y->stack.count, argCount, closure);
    yapArrayPush(Y, &Y->frames, frame);

    return frame;
}

static yBool yapContextCallCFunction(struct yapContext *Y, yapCFunction func, yU32 argCount, yapValue *thisVal);
static yBool yapContextCreateObject(struct yapContext *Y, yapFrame **framePtr, yapValue *isa, int argCount);

static yBool yapContextCall(struct yapContext *Y, yapFrame **framePtr, yapValue *thisVal, yapValue *callable, int argCount)
{
    if(!callable)
    {
        yapContextSetError(Y, YVE_RUNTIME, "YOP_CALL: empty stack!");
        return yFalse;
    }
    if(callable->type == YVT_REF)
    {
        callable = *callable->refVal;
    }
    if(!yapValueIsCallable(callable))
    {
        yapContextSetError(Y, YVE_RUNTIME, "YOP_CALL: variable not callable");
        return yFalse;
    }
    if(callable->type == YVT_CFUNCTION)
    {
        if(!yapContextCallCFunction(Y, *callable->cFuncVal, argCount, thisVal))
            return yFalse;
    }
    else if(callable->type == YVT_OBJECT)
    {
        return yapContextCreateObject(Y, framePtr, callable, argCount);
    }
    else
    {
        yapValue *closure = (callable->closureVars) ? callable : NULL;
        *framePtr = yapContextPushFrame(Y, callable->blockVal, argCount, YFT_FUNC, thisVal, closure);
    }
    return yTrue;
}

static yapValue *yapFindFunc(struct yapContext *Y, yapValue *object, const char *name)
{
    yapValue *v = *(yapObjectGetRef(Y, object->objectVal, name, yFalse));
    if(v == &yapValueNull)
    {
        if(object->objectVal->isa)
            return yapFindFunc(Y, object->objectVal->isa, name);
        return NULL;
    }
    else
    {
        if(!yapValueIsCallable(v))
            return NULL;
    }
    return v;
}

yBool yapContextCallFuncByName(struct yapContext *Y, yapValue *thisVal, const char *name, int argCount)
{
    yapFrame *frame = NULL;
    yapValue **valueRef = yapContextResolve(Y, name);
    if(!valueRef || !(*valueRef))
        return yFalse;
    if(yapContextCall(Y, &frame, thisVal, *valueRef, argCount))
    {
        yapContextLoop(Y, yTrue);
        return yTrue;
    }
    return yFalse;
}

static yBool yapContextCreateObject(struct yapContext *Y, yapFrame **framePtr, yapValue *isa, int argCount)
{
    yBool ret = yTrue;
    yapValue *initFunc = yapFindFunc(Y, isa, "init");
    yapValue *newObject = yapValueCreateObject(Y, isa, (initFunc) ? 0 : argCount, yFalse);

    if(initFunc)
        return yapContextCall(Y, framePtr, newObject, initFunc, argCount);

    yapArrayPush(Y, &Y->stack, newObject);
    Y->lastRet = 1; // leaving the new object on the stack (object creation via this function is a CALL)
    return ret;
}

// TODO: this needs to protect against variable masking/shadowing
static yapValue **yapContextRegister(struct yapContext *Y, const char *name, yapValue *value)
{
    yapValue **valueRef;
    yapFrame *frame = yapArrayTop(Y, &Y->frames);
    if(!frame)
        return NULL;

    if(frame->block == frame->block->chunk->block)
    {
        // If we're in the chunk's "main" function, all variable
        // registration goes into the globals
        valueRef = (yapValue**)yapHashSet(Y, Y->globals, name, value);
    }
    else
    {
        valueRef = (yapValue**)yapHashSet(Y, frame->locals, name, value);
    }
    return valueRef;
}

static void yapContextPushRef(struct yapContext *Y, yapValue **valueRef)
{
    yapValue *value = yapValueCreateRef(Y, valueRef);
    yapArrayPush(Y, &Y->stack, value);
}

// TODO: merge this function with PushFrame and _RET
static yBool yapContextCallCFunction(struct yapContext *Y, yapCFunction func, yU32 argCount, yapValue *thisVal)
{
    int retCount;
    yapFrame *frame = yapFrameCreate(Y, YFT_FUNC, thisVal, NULL, Y->stack.count, argCount, NULL);
    yapArrayPush(Y, &Y->frames, frame);

    retCount = func(Y, argCount);

    yapArrayPop(Y, &Y->frames); // Removes 'frame' from top of stack
    yapFrameDestroy(Y, frame);
    frame = yapArrayTop(Y, &Y->frames);
    if(!frame)
        return yFalse;

    // Stash lastRet for any YOP_KEEPs in the pipeline
    Y->lastRet = retCount;
    return yTrue;
}

void yapContextPopValues(struct yapContext *Y, yU32 count)
{
    while(count)
    {
        yapArrayPop(Y, &Y->stack);
        count--;
    }
}

yapValue *yapContextGetValue(struct yapContext *Y, yU32 howDeep)
{
    if(howDeep >= Y->stack.count)
        return NULL;

    return Y->stack.data[(Y->stack.count - 1) - howDeep];
}

static yapContextFrameCleanup(struct yapContext *Y, yapFrame *frame)
{
    if(frame->cleanupCount)
    {
        int i;
        for(i=0; i < frame->cleanupCount; i++)
        {
            int index = ((Y->stack.count - 1) - Y->lastRet) - i;
            Y->stack.data[index] = NULL;
        }
        yapArraySquash(Y, &Y->stack);
    }
}

struct yapFrame *yapContextPopFrames(struct yapContext *Y, yU32 frameTypeToFind, yBool keepIt)
{
    yapFrame *frame = yapArrayTop(Y, &Y->frames);

    if(frameTypeToFind != YFT_ANY)
    {
        while(frame && !(frame->type & frameTypeToFind))
        {
            yapContextFrameCleanup(Y, frame);
            yapFrameDestroy(Y, frame);
            yapArrayPop(Y, &Y->frames);
            frame = yapArrayTop(Y, &Y->frames);
        };
    }

    if(frame && !keepIt)
    {
        yapContextFrameCleanup(Y, frame);
        yapFrameDestroy(Y, frame);
        yapArrayPop(Y, &Y->frames);
        frame = yapArrayTop(Y, &Y->frames);
    }

    return frame;
}

static yS32 yapContextPopInts(struct yapContext *Y, int count, int *output)
{
    yS32 i;
    for(i=0; i<count; i++)
    {
        yapValue *v = yapArrayPop(Y, &Y->stack);
        if(!v)
            return i;
        v = yapValueToInt(Y, v);
        if(!v)
            return i;
        output[i] = v->intVal;
    }
    return i;
}

yapValue * yapContextThis(yapContext *Y)
{
    int i;
    for(i = Y->frames.count - 1; i >= 0; i--)
    {
        yapFrame *frame = (yapFrame *)Y->frames.data[i];
        if(frame->type & YFT_FUNC)
            return frame->thisVal;
    }
    return yapValueNullPtr;
}

yBool yapContextGetArgs(struct yapContext *Y, int argCount, const char *argFormat, ...)
{
    yBool required = yTrue;
    const char *c;
    yapValue *v;
    yapValue **valuePtr;
    yapArray *leftovers = NULL;
    int argsTaken = 0; // from the yap stack (the amount of incoming varargs solely depends on argFormat)
    va_list args;
    va_start(args, argFormat);

    for(c = argFormat; *c; c++)
    {
        if(*c == '|')
        {
            required = yFalse;
            continue;
        }

        if(*c == '.')
        {
            leftovers = va_arg(args, yapArray*);
            break;
        };

        if(argsTaken == argCount)
        {
            // We have run out of incoming yap arguments!
            // If the current argument is required, we've just failed.
            // If not, what we've gathered is "enough". Pop the args and return success.
            if(!required)
            {
                yapContextPopValues(Y, argCount);
                return yTrue;
            }
            return yFalse;
        };

        v = yapContextGetArg(Y, argsTaken, argCount);
        if(!v)
        {
            // this is a very serious failure (argCount doesn't agree with yapContextGetArg)
            yapContextSetError(Y, YVE_RUNTIME, "yapContextGetArgs(): VM stack and argCount disagree!");
            return yFalse;
        }
        argsTaken++;

        switch(*c)
        {
        default:
        case '?': /* can be anything */                    break;
        case 'n': if(v->type != YVT_NULL)   return yFalse; break;
        case 's': if(v->type != YVT_STRING) return yFalse; break;
        case 'i': if(v->type != YVT_INT)    return yFalse; break;
        case 'f': if(v->type != YVT_FLOAT)  return yFalse; break;
        case 'a': if(v->type != YVT_ARRAY)  return yFalse; break;
        case 'd': if(v->type != YVT_OBJECT) return yFalse; break; // "dict"
        case 'o': if(v->type != YVT_OBJECT) return yFalse; break;
        case 'c':
            {
                // "callable" - function, object, etc
                if(!yapValueIsCallable(v))
                    return yFalse;
            }
            break;
        };

        valuePtr = va_arg(args, yapValue**);
        *valuePtr = v;
    }

    if(leftovers)
    {
        for( ; argsTaken < argCount; argsTaken++)
        {
            yapArrayPush(Y, leftovers, yapContextGetArg(Y, argsTaken, argCount));
        }
    }

    if(argsTaken != argCount)
    {
        // too many args!
        return yFalse;
    }

    va_end(args);
    yapContextPopValues(Y, argCount);
    return yTrue;
}

// TODO: reuse code between yapContextArgsFailure and yapContextSetError
int yapContextArgsFailure(struct yapContext *Y, int argCount, const char *errorFormat, ...)
{
    va_list args;
    char tempStr[MAX_ERROR_LENGTH + 1];

    yapContextClearError(Y);
    va_start(args, errorFormat);
    vsprintf(tempStr, errorFormat, args);
    va_end(args);

    Y->errorType = YVE_RUNTIME;
    Y->error = yapStrdup(Y, tempStr);

    yapContextPopValues(Y, argCount);
    return 0;
}

#ifdef YAP_TRACE_EXECUTION
static const char *yapValueDebugString(struct yapContext *Y, yapValue *v)
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
        sprintf(valString, "(%s)", yapStringSafePtr(&v->stringVal));
        break;
    case YVT_ARRAY:
        sprintf(valString, "(count: %d)", v->arrayVal->count);
        break;
    }

    sprintf(buffer, "%s 0x%p %s", yapValueTypeName(Y, v->type), v, valString);
    return buffer;
}

static void yapContextLogState(yapContext *Y)
{
    int i;

    yapTraceExecution(("\n\n\n------------------------------------------\n"));

    if(Y->frames.count > 0)
    {
        yapFrame *frame = yapArrayTop(Y, &Y->frames);
        yapTraceExecution(("0x%p [cleanup:%d][lastRet:%d] IP: ", frame, frame->cleanupCount, Y->lastRet));
        yapOpsDump(frame->ip, 1);
    }

    yapTraceExecution(("\n"));

    yapTraceExecution(("-- Stack Top --\n"));
    for(i=0; i<Y->stack.count; i++)
    {
        yapValue *v = (yapValue*)Y->stack.data[Y->stack.count - 1 - i];
        yapTraceExecution(("%2.2d: %s\n", i, yapValueDebugString(Y, v)));
    }
    yapTraceExecution(("-- Stack Bot --\n"));
}
#endif

void yapContextLoop(struct yapContext *Y, yBool stopAtPop)
{
    yapFrame *frame = yapArrayTop(Y, &Y->frames);
    yBool continueLooping = yTrue;
    yBool newFrame;
    yU16 opcode;
    yU16 operand;
    yU32 startingFrameCount = Y->frames.count;

    if(!frame)
    {
        yapContextSetError(Y, YVE_RUNTIME, "yapContextLoop(): No stack frame!");
        return;
    }

    // Main VM loop!
    while(continueLooping && !Y->error)
    {
        if(stopAtPop && (Y->frames.count < startingFrameCount))
            break;

        newFrame = yFalse;

        // These are put into temporary variables for future ntohs() cross-platform safety
        opcode  = frame->ip->opcode;
        operand = frame->ip->operand;

#ifdef YAP_TRACE_EXECUTION
        yapContextLogState(Y);
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
            yapValue *performSkipValue = yapArrayTop(Y, &Y->stack);
            yBool performSkip = yFalse;
            if(!performSkipValue)
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_SKIP: empty stack!");
                continueLooping = yFalse;
            }
            performSkipValue = yapValueToBool(Y, performSkipValue);
            performSkip = (performSkipValue->intVal) ? yTrue : yFalse;
            if(opcode == YOP_AND)
                performSkip = !performSkip;
            if(performSkip)
            {
                for(i = 0; i < operand; i++)
                    frame->ip++;
            }
            else
            {
                yapArrayPop(Y, &Y->stack);
            }
        }
        break;

        case YOP_PUSHNULL:
        {
            yapArrayPush(Y, &Y->stack, &yapValueNull);
        }
        break;

        case YOP_PUSHTHIS:
        {
            yapArrayPush(Y, &Y->stack, yapContextThis(Y));
        }
        break;

        case YOP_PUSHI:
        {
            yapValue *value = yapValueCreateInt(Y, operand);
            yapArrayPush(Y, &Y->stack, value);
        }
        break;

        case YOP_PUSH_KB:
        {
            yapValue *value = yapValueCreateFunction(Y, frame->block->chunk->blocks.data[operand]);
            yapArrayPush(Y, &Y->stack, value);
        }
        break;

        case YOP_PUSH_KI:
        {
            yapValue *value = yapValueCreateInt(Y, frame->block->chunk->kInts.data[operand]);
            yapArrayPush(Y, &Y->stack, value);
        }
        break;

        case YOP_PUSH_KF:
        {
            yapValue *value = yapValueCreateFloat(Y, *((yF32*)&frame->block->chunk->kFloats.data[operand]));
            yapArrayPush(Y, &Y->stack, value);
        }
        break;

        case YOP_PUSH_KS:
        {
            yapValue *value;
            if(frame->block->chunk->temporary)
                value = yapValueCreateString(Y, frame->block->chunk->kStrings.data[operand]);
            else
                value = yapValueCreateKString(Y, frame->block->chunk->kStrings.data[operand]);

            yapArrayPush(Y, &Y->stack, value);
        }
        break;

        case YOP_VARREG_KS:
        {
            yapValue **valueRef = yapContextRegister(Y, frame->block->chunk->kStrings.data[operand], yapValueNullPtr);
            yapContextPushRef(Y, valueRef);
        }
        break;

        case YOP_VARREF_KS:
        {
            yapValue **valueRef = yapContextResolve(Y, frame->block->chunk->kStrings.data[operand]);
            if(valueRef)
            {
                yapContextPushRef(Y, valueRef);
            }
            else
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_GETVAR_KS: no variable named '%s'", frame->block->chunk->kStrings.data[operand]);
                continueLooping = yFalse;
            }
        }
        break;

        case YOP_REFVAL:
        {
            yapValue *value = yapArrayPop(Y, &Y->stack);
            if(!value)
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_REFVAL: empty stack!");
                continueLooping = yFalse;
                break;
            };
            if(value->type != YVT_REF)
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_REFVAL: requires ref on top of stack");
                continueLooping = yFalse;
                break;
            }
            yapArrayPush(Y, &Y->stack, *value->refVal);
        }
        break;

        case YOP_ADD:
        {
            yapValue *b = yapArrayPop(Y, &Y->stack);
            yapValue *a = yapArrayPop(Y, &Y->stack);
            a = yapValueAdd(Y, a, b);
            if(a)
                yapArrayPush(Y, &Y->stack, a);
            else
                continueLooping = yFalse;
        }
        break;

        case YOP_SUB:
        {
            yapValue *b = yapArrayPop(Y, &Y->stack);
            yapValue *a = yapArrayPop(Y, &Y->stack);
            yapValue *c = yapValueSub(Y, a, b);
            if(operand)
            {
                // Leave entries on the stack. Used in for loops.
                yapArrayPush(Y, &Y->stack, a);
                yapArrayPush(Y, &Y->stack, b);
            }
            if(c)
                yapArrayPush(Y, &Y->stack, c);
            else
                continueLooping = yFalse;
        }
        break;

        case YOP_MUL:
        {
            yapValue *b = yapArrayPop(Y, &Y->stack);
            yapValue *a = yapArrayPop(Y, &Y->stack);
            a = yapValueMul(Y, a, b);
            if(a)
                yapArrayPush(Y, &Y->stack, a);
            else
                continueLooping = yFalse;
        }
        break;

        case YOP_DIV:
        {
            yapValue *b = yapArrayPop(Y, &Y->stack);
            yapValue *a = yapArrayPop(Y, &Y->stack);
            a = yapValueDiv(Y, a, b);
            if(a)
                yapArrayPush(Y, &Y->stack, a);
            else
                continueLooping = yFalse;
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
            yapValue *b = yapArrayPop(Y, &Y->stack);
            yapValue *a = yapArrayPop(Y, &Y->stack);
            yS32 cmp = yapValueCmp(Y, a, b);
            if(opcode != YOP_CMP)
            {
                if((opcode == YOP_EQUALS) || (opcode == YOP_NOTEQUALS))
                {
                    // Boolean tests
                    cmp = !cmp;
                    if(opcode == YOP_NOTEQUALS)
                        cmp = !cmp;
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
            yapArrayPush(Y, &Y->stack, yapValueCreateInt(Y, cmp));
        }
        break;

        case YOP_SETVAR:
        {
            yapValue *ref = yapArrayPop(Y, &Y->stack);
            yapValue *val = yapArrayTop(Y, &Y->stack);
            if(!operand)
                yapArrayPop(Y, &Y->stack);
            continueLooping = yapValueSetRefVal(Y, ref, val);
        }
        break;

        case YOP_INHERITS:
        {
            yapValue *l = yapArrayPop(Y, &Y->stack);
            yapValue *r = yapArrayPop(Y, &Y->stack);
            yBool inherits = yapValueTestInherits(Y, l, r);

            yapArrayPush(Y, &Y->stack, yapValueCreateInt(Y, inherits ? 1 : 0));
        }
        break;

        case YOP_VARARGS:
        {
            int i;
            int varargCount = frame->argCount - operand;
            yapValue *varargsArray = yapValueCreateArray(Y);

            // Only one of these for loops will actually loop
            for(; varargCount < 0; varargCount++)
                yapArrayPush(Y, &Y->stack, &yapValueNull);
            for(; varargCount > 0; varargCount--)
                yapArrayUnshift(Y, varargsArray->arrayVal, yapArrayPop(Y, &Y->stack));

            yapArrayPush(Y, &Y->stack, varargsArray);
        }
        break;

        case YOP_INDEX:
        {
            yapValue *index = yapArrayPop(Y, &Y->stack);
            yapValue *value = yapArrayPop(Y, &Y->stack);
            if(value && index)
            {
                int opFlags = operand;
                yapValue *ret = yapValueIndex(Y, value, index, (opFlags & YOF_LVALUE) ? yTrue : yFalse);
                if(ret)
                {
                    yapArrayPush(Y, &Y->stack, ret);
                    if(opFlags & YOF_PUSHOBJ)
                        yapArrayPush(Y, &Y->stack, value);
                    else if(opFlags & YOF_PUSHTHIS)
                        yapArrayPush(Y, &Y->stack, yapContextThis(Y));
                }
                else
                {
                    if(Y->errorType == YVE_NONE)
                        yapContextSetError(Y, YVE_RUNTIME, "YOP_INDEX: Failed attempt to index into type %s", yapValueTypeName(Y, value->type));
                    continueLooping = yFalse;
                }
            }
            else
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_INDEX: empty stack!");
                continueLooping = yFalse;
            }
        }
        break;

        case YOP_DUPE:
        case YOP_MOVE:
        {
            int topIndex = Y->stack.count - 1;
            int requestedIndex = topIndex - operand;
            if(requestedIndex >= 0)
            {
                yapValue *val = Y->stack.data[requestedIndex];
                if(opcode == YOP_MOVE)
                {
                    if(operand == 0)
                        break; // no sense in moving the top to the top
                    Y->stack.data[requestedIndex] = NULL;
                    yapArraySquash(Y, &Y->stack);
                }
                yapArrayPush(Y, &Y->stack, val);
            }
            else
            {
                yapContextSetError(Y, YVE_RUNTIME, "%s: impossible index", (opcode == YOP_DUPE) ? "YOP_DUPE" : "YOP_MOVE");
                continueLooping = yFalse;
            }
        }
        break;

        case YOP_POP:
        {
            int i;
            for(i = 0; i < operand; i++)
                yapArrayPop(Y, &Y->stack);
        }
        break;

        case YOP_CALL:
        {
            int argCount = operand;
            yapFrame *oldFrame = frame;
            yapValue *thisVal = yapArrayPop(Y, &Y->stack);
            yapValue *callable = yapArrayPop(Y, &Y->stack);
            continueLooping = yapContextCall(Y, &frame, thisVal, callable, argCount);
            if(frame != oldFrame)
                newFrame = yTrue;
        }
        break;

        case YOP_RET:
        {
            int argCount = operand;
            frame = yapContextPopFrames(Y, YFT_FUNC, yTrue);
            //                if(!argCount && frame && (frame->flags & YFF_INIT) && frame->thisVal)
            //                {
            //                    // We are leaving an init function that does not return any value (which
            //                    // is different from returning null). This bit of magic makes init() functions
            //                    // return 'this' instead of nothing. It avoids the boilerplate 'return this'
            //                    // at the end of every init() in order for it to work at all, but allows for
            //                    // generator init functions.
            //                    argCount = 1;
            //                    yapArrayPush(Y, &Y->stack, frame->thisVal);
            //                }
            frame = yapContextPopFrames(Y, YFT_FUNC, yFalse);
            if(frame)
            {
                // Stash lastRet for any YOP_KEEPs in the pipeline
                Y->lastRet = argCount;
            }
            else
            {
                // Throw away return values and bail out of loop
                continueLooping = yFalse;
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
                    yapTraceExecution(("-- cleaning stack entry --\n"));
                    yapArrayPop(Y, &Y->stack);
                }
            }
            else if(keepCount > offerCount)
            {
                int i;
                for(i = 0; i < (keepCount - offerCount); i++)
                {
                    yapTraceExecution(("-- padding stack with null --\n"));
                    yapArrayPush(Y, &Y->stack, &yapValueNull);
                }
            }
            Y->lastRet = 0;
        }
        break;

        case YOP_CLOSE:
        {
            yapValue *v = yapArrayTop(Y, &Y->stack);
            yapValueAddClosureVars(Y, v);
        }
        break;

        case YOP_IF:
        {
            yapBlock *block = NULL;
            yapValue *cond, *ifBody, *elseBody = NULL;
            cond   = yapArrayPop(Y, &Y->stack);
            ifBody = yapArrayPop(Y, &Y->stack);
            if(operand)
                elseBody = yapArrayPop(Y, &Y->stack);
            // TODO: verify ifBody/elseBody are YVT_BLOCK
            cond = yapValueToBool(Y, cond);
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
                frame = yapContextPushFrame(Y, block, 0, YFT_COND, NULL, NULL);
                if(frame)
                    newFrame = yTrue;
                else
                    continueLooping = yFalse;
            }
        }
        break;

        case YOP_ENTER:
        {
            yapValue *blockRef = yapArrayPop(Y, &Y->stack);

            if(blockRef && blockRef->type == YVT_BLOCK && blockRef->blockVal)
            {
                yU32 frameType = operand;
                frame = yapContextPushFrame(Y, blockRef->blockVal, 0, frameType, NULL, NULL);
                if(frame)
                    newFrame = yTrue;
                else
                    continueLooping = yFalse;
            }
            else
            {
                yapContextSetError(Y, YVE_RUNTIME, "hurr");
                continueLooping = yFalse;
            }
        }
        break;

        case YOP_LEAVE:
        {
            yBool performLeave = yTrue;
            if(operand)
            {
                yapValue *cond = yapArrayPop(Y, &Y->stack);
                cond = yapValueToBool(Y, cond);
                performLeave   = !cond->intVal; // don't leave if expr is true!
            }

            if(performLeave)
            {
                frame = yapContextPopFrames(Y, YFT_ANY, yFalse);
                continueLooping = (frame) ? yTrue : yFalse;
            }
        }
        break;

        case YOP_BREAK:
        {
            // a C-style break. Find the innermost loop and kill it.
            frame = yapContextPopFrames(Y, YFT_LOOP, yFalse);
            continueLooping = (frame) ? yTrue : yFalse;
        }
        break;

        case YOP_CONTINUE:
        {
            // a C-style continue. Find the innermost loop and reset it.
            frame = yapContextPopFrames(Y, YFT_LOOP, yTrue);
            if(frame)
            {
                yapFrameReset(Y, frame, yTrue);
                newFrame = yTrue;
            }
            else
            {
                continueLooping = yFalse;
            }
        }
        break;

        case YOP_NOT:
        {
            yapValue *value = yapArrayPop(Y, &Y->stack);
            if(!value)
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_NOT: empty stack!");
                continueLooping = yFalse;
                break;
            };
            value = yapValueToBool(Y, value);
            value = yapValueCreateInt(Y, !value->intVal); // Double temporary?
            yapArrayPush(Y, &Y->stack, value);
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
            if(yapContextPopInts(Y, argsNeeded, i) != argsNeeded)
            {
                yapContextSetError(Y, YVE_RUNTIME, "Bitwise operations require integer friendly arguments");
                continueLooping = yFalse;
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
            yapArrayPush(Y, &Y->stack, yapValueCreateInt(Y, ret));
        }
        break;

        case YOP_FORMAT:
        {
            yapValue *format = yapArrayPop(Y, &Y->stack);
            yapValue *val;
            if(!format)
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_FORMAT: empty stack!");
                continueLooping = yFalse;
                break;
            };
            val = yapValueStringFormat(Y, format, operand);
            if(!val)
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_FORMAT: bad format");
                continueLooping = yFalse;
                break;
            };
            yapArrayPush(Y, &Y->stack, val);
        }
        break;

        case YOP_NTH:
        {
            yapValue *val = yapArrayPop(Y, &Y->stack);

            if(val->type == YVT_ARRAY)
            {
                yapValue *nth = yapArrayPop(Y, &Y->stack);
                if(nth->intVal >= 0 && nth->intVal < val->arrayVal->count)
                {
                    yapArrayPush(Y, &Y->stack, val->arrayVal->data[nth->intVal]);
                    Y->lastRet = 1;
                }
                else
                {
                    yapContextSetError(Y, YVE_RUNTIME, "YOP_NTH: index out of range");
                    continueLooping = yFalse;
                    break;
                }
            }
            else if(val->type == YVT_OBJECT)
            {
                yapFrame *oldFrame = frame;
                yapValue *getFunc = yapFindFunc(Y, val, "get");
                if(!getFunc)
                {
                    yapContextSetError(Y, YVE_RUNTIME, "YVT_NTH: iterable does not have a get() function");
                    continueLooping = yFalse;
                    break;
                }
                continueLooping = yapContextCall(Y, &frame, val, getFunc, 1 /* the index */);
                if(frame != oldFrame)
                    newFrame = yTrue;
            }
            else
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_NTH: Invalid value type %d", val->type);
                continueLooping = yFalse;
                break;
            }
        }
        break;

        case YOP_COUNT:
        {
            yapValue *val = yapArrayPop(Y, &Y->stack);
            if(val->type == YVT_ARRAY)
            {
                yapValue *count = yapValueCreateInt(Y, val->arrayVal->count);
                yapArrayPush(Y, &Y->stack, count);
                Y->lastRet = 1;
            }
            else if(val->type == YVT_OBJECT)
            {
                yapFrame *oldFrame = frame;
                yapValue *countFunc = yapFindFunc(Y, val, "count");
                if(!countFunc)
                {
                    yapContextSetError(Y, YVE_RUNTIME, "YVT_COUNT: iterable does not have a count() function");
                    continueLooping = yFalse;
                    break;
                }
                continueLooping = yapContextCall(Y, &frame, val, countFunc, 0);
                if(frame != oldFrame)
                    newFrame = yTrue;
            }
            else
            {
                yapContextSetError(Y, YVE_RUNTIME, "YVT_COUNT: Invalid value type %d", val->type);
                continueLooping = yFalse;
                break;
            }
        }
        break;

        default:
            yapContextSetError(Y, YVE_RUNTIME, "Unknown VM Opcode: %d", opcode);
            continueLooping = yFalse;
            break;
        }

        if(continueLooping && !newFrame)
            frame->ip++;

        yapContextGC(Y); // TODO: Only do this when (new var) Y->gc == 0, then reset
    }

    yapContextGC(Y);
}

static void yapHashEntryValueMark(struct yapContext *Y, yapHashEntry *entry)
{
    yapValueMark(Y, entry->value);
}

void yapContextGC(struct yapContext *Y)
{
    int i, j;

    // -----------------------------------------------------------------------
    // walk all used values and clear their GC flags

    for(i = 0; i < Y->usedValues.count; i++)
    {
        yapValue *value = (yapValue *)Y->usedValues.data[i];
        value->used = yFalse;
    }

    // -----------------------------------------------------------------------
    // mark all values used by things the VM still cares about

    yapHashIterate(Y, Y->globals, (yapIterateCB)yapHashEntryValueMark);

    for(i = 0; i < Y->frames.count; i++)
    {
        yapFrame *frame = (yapFrame *)Y->frames.data[i];
        if(frame->thisVal)
            yapValueMark(Y, frame->thisVal);
        if(frame->closure)
            yapValueMark(Y, frame->closure);
        yapHashIterate(Y, frame->locals, (yapIterateCB)yapHashEntryValueMark);
    }

    for(i = 0; i < Y->stack.count; i++)
    {
        yapValue *value = (yapValue *)Y->stack.data[i];
        yapValueMark(Y, value);
    }

    // -----------------------------------------------------------------------
    // sweep!

    for(i = 0; i < Y->usedValues.count; i++)
    {
        yapValue *value = (yapValue *)Y->usedValues.data[i];
        if(!value->used)
        {
            yapValueDestroy(Y, value);
            Y->usedValues.data[i] = NULL; // for future squashing
        }
    }
    yapArraySquash(Y, &Y->usedValues);
}
