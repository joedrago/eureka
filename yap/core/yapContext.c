// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapContext.h"

#include "yapBlock.h"
#include "yapCompiler.h"
#include "yapObject.h"
#include "yapFrame.h"
#include "yapChunk.h"
#include "yapOp.h"
#include "yapValue.h"
#include "yapVariable.h"

#include "yapmAll.h"
#include "yapiArray.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_ERROR_LENGTH 1023

void yapContextRegisterGlobal(yapContext *Y, const char *name, yapValue *value)
{
    yapVariable *variable = yapVariableCreate(Y, value);
    yapHashSet(Y->globals, name, variable);
}

void yapContextRegisterGlobalFunction(yapContext *Y, const char *name, yapCFunction func)
{
    yapContextRegisterGlobal(Y, name, yapValueSetCFunction(Y, yapValueAcquire(Y), func));
}

static yBool yapChunkCanBeTemporary(yapChunk *chunk)
{
    // A simple test ... if there aren't any functions in this chunk and all ktable lookups
    // are duped (chunk flagged as temporary), than the chunk can safely go away in between
    // calls to yapContextLoop (assuming zero yapFrames on the stack).
    return !chunk->hasFuncs;
}

void yapContextEval(yapContext *Y, const char *text, yU32 evalOpts)
{
    yapChunk *chunk;
    yapCompiler *compiler;

#ifdef YAP_ENABLE_MEMORY_STATS
    yapMemoryStatsReset();
#endif

    compiler = yapCompilerCreate();
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

#ifdef YAP_TRACE_OPS
            printf("--- begin chunk execution ---\n");
#endif
            // Execute the chunk's block
            yapContextPushFrame(Y, chunk->block, 0, YFT_FUNC|YFT_CHUNK, yapValueNullPtr);
            yapContextLoop(Y, yTrue);

#ifdef YAP_TRACE_OPS
            printf("---  end  chunk execution ---\n");
#endif
            if(!chunk->temporary)
            {
                yapArrayPush(&Y->chunks, chunk);
                chunk = NULL; // forget the ptr
            }
        }

        if(chunk)
        {
            yapChunkDestroy(chunk);
        }
    }

#ifdef YAP_ENABLE_MEMORY_STATS
    yapMemoryStatsPrint("yapContextEval: ");
#endif
}

yapContext *yapContextCreate(void)
{
    yapContext *Y = yapAlloc(sizeof(yapContext));
    Y->globals = yapHashCreate(0);
    yapValueTypeRegisterAllBasicTypes(Y);
    yapIntrinsicsRegister(Y);
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
        yapArrayShrink(&Y->stack, prevStackCount, NULL);

        yapContextGC(Y);
    }
    yapContextClearError(Y);
}

void yapContextSetError(yapContext *Y, yU32 errorType, const char *errorFormat, ...)
{
    va_list args;
    char tempStr[MAX_ERROR_LENGTH + 1];

    yapContextClearError(Y);
    va_start(args, errorFormat);
    vsprintf(tempStr, errorFormat, args);
    va_end(args);

    Y->errorType = errorType;
    Y->error = yapStrdup(tempStr);
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
    yapHashDestroy(Y->globals, NULL);
    yapArrayClear(&Y->frames, (yapDestroyCB)yapFrameDestroy);
    yapArrayClear(&Y->stack, NULL);
    yapArrayClear(&Y->chunks, (yapDestroyCB)yapChunkDestroy);

    yapArrayClear(&Y->usedVariables, (yapDestroyCB)yapVariableDestroy);
    yapArrayClearP1(&Y->usedValues, (yapDestroyCB1)yapValueDestroy, Y);
    yapArrayClearP1(&Y->freeValues, (yapDestroyCB1)yapValueDestroy, Y);

    yapArrayClear(&Y->types, (yapDestroyCB)yapValueTypeDestroy);
    yapContextClearError(Y);

    yapFree(Y);
}

static yapVariable *yapContextResolveVariable(yapContext *Y, const char *name)
{
    int i;
    yapFrame *frame;
    yapVariable **variableRef;

    for(i = Y->frames.count - 1; i >= 0; i--)
    {
        frame = (yapFrame *)Y->frames.data[i];

        // Check the local variables
        variableRef = (yapVariable **)yapHashLookup(frame->locals, name, yFalse);
        if(variableRef) return *variableRef;

        if(frame->type == YFT_FUNC)
            break;
    }

    // check global vars
    variableRef = (yapVariable **)yapHashLookup(Y->globals, name, yFalse);
    if(variableRef) return *variableRef;

    return NULL;
}

yapFrame *yapContextPushFrame(yapContext *Y, yapBlock *block, int argCount, yU32 frameType, struct yapValue *thisVal)
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
                yapArrayPop(&Y->stack);
        }
        else if(block->argCount > argCount)
        {
            // Too few arguments -- pad with nulls
            int i;
            for(i = 0; i < (block->argCount - argCount); i++)
                yapArrayPush(&Y->stack, &yapValueNull);
        }
    }

    frame = yapFrameCreate(frameType, thisVal, block, Y->stack.count, argCount);
    yapArrayPush(&Y->frames, frame);

    return frame;
}

static yBool yapContextCallCFunction(yapContext *Y, yapCFunction func, yU32 argCount, yapValue *thisVal);
static yBool yapContextCreateObject(yapContext *Y, yapFrame **framePtr, yapValue *isa, int argCount);

static yBool yapContextCall(yapContext *Y, yapFrame **framePtr, yapValue *thisVal, yapValue *callable, int argCount)
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
        *framePtr = yapContextPushFrame(Y, callable->blockVal, argCount, YFT_FUNC, thisVal);
        if(*framePtr && callable->closureVars)
        {
            // Hand over all closure variables into the new local scope
            int i;
            for(i=0; i<callable->closureVars->count; i++)
            {
                yapClosureVariable *cv = (yapClosureVariable *)callable->closureVars->data[i];
                yapHashSet((*framePtr)->locals, cv->name, cv->variable);
            }
        }
    }
    return yTrue;
}

static yapValue *yapFindFunc(yapContext *Y, yapValue *object, const char *name)
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

yBool yapContextCallFuncByName(yapContext *Y, yapValue *thisVal, const char *name, int argCount)
{
    yapFrame *frame = NULL;
    yapVariable *variable = yapContextResolveVariable(Y, name);
    if(!variable)
        return yFalse;
    if(yapContextCall(Y, &frame, thisVal, variable->value, argCount))
    {
        yapContextLoop(Y, yTrue);
        return yTrue;
    }
    return yFalse;
}

static yBool yapContextCreateObject(yapContext *Y, yapFrame **framePtr, yapValue *isa, int argCount)
{
    yBool ret = yTrue;
    yapValue *initFunc = yapFindFunc(Y, isa, "init");
    yapValue *newObject = yapValueObjectCreate(Y, isa, (initFunc) ? 0 : argCount);

    if(initFunc)
        return yapContextCall(Y, framePtr, newObject, initFunc, argCount);

    yapArrayPush(&Y->stack, newObject);
    return ret;
}

// TODO: this needs to protect against variable masking/shadowing
static yapVariable *yapContextRegisterVariable(yapContext *Y, const char *name, yapValue *value)
{
    yapVariable *variable;
    yapFrame *frame = yapArrayTop(&Y->frames);
    if(!frame)
        return NULL;

    variable = yapVariableCreate(Y, value);
    if(frame->block == frame->block->chunk->block)
    {
        // If we're in the chunk's "main" function, all variable
        // registration goes into the globals
        yapHashSet(Y->globals, name, variable);
    }
    else
    {
        yapHashSet(frame->locals, name, variable);
    }
    return variable;
}

static void yapContextPushRef(yapContext *Y, yapVariable *variable)
{
    yapValue *value = yapValueSetRef(Y, yapValueAcquire(Y), &variable->value);
    yapArrayPush(&Y->stack, value);
}

// TODO: merge this function with PushFrame and _RET
static yBool yapContextCallCFunction(yapContext *Y, yapCFunction func, yU32 argCount, yapValue *thisVal)
{
    int retCount;
    yapFrame *frame = yapFrameCreate(YFT_FUNC, thisVal, NULL, Y->stack.count, argCount);
    yapArrayPush(&Y->frames, frame);

    retCount = func(Y, argCount);

    yapArrayPop(&Y->frames); // Removes 'frame' from top of stack
    yapFrameDestroy(frame);
    frame = yapArrayTop(&Y->frames);
    if(!frame)
        return yFalse;

    // Stash lastRet for any YOP_KEEPs in the pipeline
    Y->lastRet = retCount;
    return yTrue;
}

void yapContextPopValues(yapContext *Y, yU32 count)
{
    while(count)
    {
        yapArrayPop(&Y->stack);
        count--;
    }
}

yapValue *yapContextGetValue(yapContext *Y, yU32 howDeep)
{
    if(howDeep >= Y->stack.count)
        return NULL;

    return Y->stack.data[(Y->stack.count - 1) - howDeep];
}

static yapContextFrameCleanup(yapContext *Y, yapFrame *frame)
{
    if(frame->cleanupCount)
    {
        int i;
        for(i=0; i < frame->cleanupCount; i++)
        {
            int index = ((Y->stack.count - 1) - Y->lastRet) - i;
            Y->stack.data[index] = NULL;
        }
        yapArraySquash(&Y->stack);
    }
}

struct yapFrame *yapContextPopFrames(yapContext *Y, yU32 frameTypeToFind, yBool keepIt)
{
    yapFrame *frame = yapArrayTop(&Y->frames);

    if(frameTypeToFind != YFT_ANY)
    {
        while(frame && !(frame->type & frameTypeToFind))
        {
            yapContextFrameCleanup(Y, frame);
            yapFrameDestroy(frame);
            yapArrayPop(&Y->frames);
            frame = yapArrayTop(&Y->frames);
        };
    }

    if(frame && !keepIt)
    {
        yapContextFrameCleanup(Y, frame);
        yapFrameDestroy(frame);
        yapArrayPop(&Y->frames);
        frame = yapArrayTop(&Y->frames);
    }

    return frame;
}

static yS32 yapContextPopInts(yapContext *Y, int count, int *output)
{
    yS32 i;
    for(i=0; i<count; i++)
    {
        yapValue *v = yapArrayPop(&Y->stack);
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

yBool yapContextGetArgs(yapContext *Y, int argCount, const char *argFormat, ...)
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
            yapArrayPush(leftovers, yapContextGetArg(Y, argsTaken, argCount));
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
int yapContextArgsFailure(yapContext *Y, int argCount, const char *errorFormat, ...)
{
    va_list args;
    char tempStr[MAX_ERROR_LENGTH + 1];

    yapContextClearError(Y);
    va_start(args, errorFormat);
    vsprintf(tempStr, errorFormat, args);
    va_end(args);

    Y->errorType = YVE_RUNTIME;
    Y->error = yapStrdup(tempStr);

    yapContextPopValues(Y, argCount);
    return 0;
}

#ifdef YAP_TRACE_OPS
static const char *yapValueDebugString(yapContext *Y, yapValue *v)
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

    printf("\n\n\n------------------------------------------\n");

    if(Y->frames.count > 0)
    {
        yapFrame *frame = yapArrayTop(&Y->frames);
        printf("0x%p [cleanup:%d][lastRet:%d] IP: ", frame, frame->cleanupCount, Y->lastRet);
        yapOpsDump(frame->ip, 1);
    }

    printf("\n");
    printf("-- Stack --\n");

    for(i=0; i<Y->stack.count; i++)
    {
        yapValue *v = (yapValue*)Y->stack.data[Y->stack.count - 1 - i];
        printf("%2.2d: %s\n", i, yapValueDebugString(Y, v));
    }
}
#endif

void yapContextLoop(yapContext *Y, yBool stopAtPop)
{
    yapFrame *frame = yapArrayTop(&Y->frames);
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

#ifdef YAP_TRACE_OPS
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
            yapValue *performSkipValue = yapArrayTop(&Y->stack);
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
                yapArrayPop(&Y->stack);
            }
        }
        break;

        case YOP_PUSHNULL:
        {
            yapArrayPush(&Y->stack, &yapValueNull);
        }
        break;

        case YOP_PUSHTHIS:
        {
            yapArrayPush(&Y->stack, yapContextThis(Y));
        }
        break;

        case YOP_PUSHI:
        {
            yapValue *value = yapValueSetInt(Y, yapValueAcquire(Y), operand);
            yapArrayPush(&Y->stack, value);
        }
        break;

        case YOP_PUSH_KB:
        {
            yapValue *value = yapValueSetFunction(Y, yapValueAcquire(Y), frame->block->chunk->blocks.data[operand]);
            yapArrayPush(&Y->stack, value);
        }
        break;

        case YOP_PUSH_KI:
        {
            yapValue *value = yapValueSetInt(Y, yapValueAcquire(Y), frame->block->chunk->kInts.data[operand]);
            yapArrayPush(&Y->stack, value);
        }
        break;

        case YOP_PUSH_KF:
        {
            yapValue *value = yapValueSetFloat(Y, yapValueAcquire(Y), *((yF32*)&frame->block->chunk->kFloats.data[operand]));
            yapArrayPush(&Y->stack, value);
        }
        break;

        case YOP_PUSH_KS:
        {
            yapValue *value;
            if(frame->block->chunk->temporary)
                value = yapValueSetString(Y, yapValueAcquire(Y), frame->block->chunk->kStrings.data[operand]);
            else
                value = yapValueSetKString(Y, yapValueAcquire(Y), frame->block->chunk->kStrings.data[operand]);

            yapArrayPush(&Y->stack, value);
        }
        break;

        case YOP_VARREG_KS:
        {
            yapVariable *variable = yapContextRegisterVariable(Y, frame->block->chunk->kStrings.data[operand], yapValueNullPtr);
            yapContextPushRef(Y, variable);
        }
        break;

        case YOP_VARREF_KS:
        {
            yapVariable *variable = yapContextResolveVariable(Y, frame->block->chunk->kStrings.data[operand]);
            if(variable)
            {
                yapContextPushRef(Y, variable);
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
            yapValue *value = yapArrayPop(&Y->stack);
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
            yapArrayPush(&Y->stack, *value->refVal);
        }
        break;

        case YOP_ADD:
        {
            yapValue *b = yapArrayPop(&Y->stack);
            yapValue *a = yapArrayPop(&Y->stack);
            a = yapValueAdd(Y, a, b);
            if(a)
                yapArrayPush(&Y->stack, a);
            else
                continueLooping = yFalse;
        }
        break;

        case YOP_SUB:
        {
            yapValue *b = yapArrayPop(&Y->stack);
            yapValue *a = yapArrayPop(&Y->stack);
            yapValue *c = yapValueSub(Y, a, b);
            if(operand)
            {
                // Leave entries on the stack. Used in for loops.
                yapArrayPush(&Y->stack, a);
                yapArrayPush(&Y->stack, b);
            }
            if(c)
                yapArrayPush(&Y->stack, c);
            else
                continueLooping = yFalse;
        }
        break;

        case YOP_MUL:
        {
            yapValue *b = yapArrayPop(&Y->stack);
            yapValue *a = yapArrayPop(&Y->stack);
            a = yapValueMul(Y, a, b);
            if(a)
                yapArrayPush(&Y->stack, a);
            else
                continueLooping = yFalse;
        }
        break;

        case YOP_DIV:
        {
            yapValue *b = yapArrayPop(&Y->stack);
            yapValue *a = yapArrayPop(&Y->stack);
            a = yapValueDiv(Y, a, b);
            if(a)
                yapArrayPush(&Y->stack, a);
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
            yapValue *b = yapArrayPop(&Y->stack);
            yapValue *a = yapArrayPop(&Y->stack);
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
            yapArrayPush(&Y->stack, yapValueSetInt(Y, a, cmp));
        }
        break;

        case YOP_SETVAR:
        {
            yapValue *ref = yapArrayPop(&Y->stack);
            yapValue *val = yapArrayTop(&Y->stack);
            if(!operand)
                yapArrayPop(&Y->stack);
            continueLooping = yapValueSetRefVal(Y, ref, val);
        }
        break;

        case YOP_INHERITS:
        {
            yapValue *ref = yapArrayPop(&Y->stack);
            yapValue *val = yapArrayPop(&Y->stack);
            continueLooping = yapValueSetRefInherits(Y, ref, val);
            if(continueLooping && operand)
            {
                yapArrayPush(&Y->stack, *ref->refVal);
            }
        }
        break;

        case YOP_VARARGS:
        {
            int i;
            int varargCount = frame->argCount - operand;
            yapValue *varargsArray = yapValueArrayCreate(Y);

            // Only one of these for loops will actually loop
            for(; varargCount < 0; varargCount++)
                yapArrayPush(&Y->stack, &yapValueNull);
            for(; varargCount > 0; varargCount--)
                yapArrayUnshift(varargsArray->arrayVal, yapArrayPop(&Y->stack));

            yapArrayPush(&Y->stack, varargsArray);
        }
        break;

        case YOP_INDEX:
        {
            yapValue *index = yapArrayPop(&Y->stack);
            yapValue *value = yapArrayPop(&Y->stack);
            if(value && index)
            {
                int opFlags = operand;
                yapValue *ret = yapValueIndex(Y, value, index, (opFlags & YOF_LVALUE) ? yTrue : yFalse);
                if(ret)
                {
                    yapArrayPush(&Y->stack, ret);
                    if(opFlags & YOF_PUSHOBJ)
                        yapArrayPush(&Y->stack, value);
                    else if(opFlags & YOF_PUSHTHIS)
                        yapArrayPush(&Y->stack, yapContextThis(Y));
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
                    yapArraySquash(&Y->stack);
                }
                yapArrayPush(&Y->stack, val);
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
                yapArrayPop(&Y->stack);
        }
        break;

        case YOP_CALL:
        {
            int argCount = operand;
            yapFrame *oldFrame = frame;
            yapValue *thisVal = yapArrayPop(&Y->stack);
            yapValue *callable = yapArrayPop(&Y->stack);
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
            //                    yapArrayPush(&Y->stack, frame->thisVal);
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
                    yapTrace(("-- cleaning stack entry --\n"));
                    yapArrayPop(&Y->stack);
                }
            }
            else if(keepCount > offerCount)
            {
                int i;
                for(i = 0; i < (keepCount - offerCount); i++)
                {
                    yapTrace(("-- padding stack with null --\n"));
                    yapArrayPush(&Y->stack, &yapValueNull);
                }
            }
            Y->lastRet = 0;
        }
        break;

        case YOP_CLOSE:
        {
            yapValue *v = yapArrayTop(&Y->stack);
            yapValueAddClosureVars(Y, v);
        }
        break;

        case YOP_IF:
        {
            yapBlock *block = NULL;
            yapValue *cond, *ifBody, *elseBody = NULL;
            cond   = yapArrayPop(&Y->stack);
            ifBody = yapArrayPop(&Y->stack);
            if(operand)
                elseBody = yapArrayPop(&Y->stack);
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
                frame = yapContextPushFrame(Y, block, 0, YFT_COND, NULL);
                if(frame)
                    newFrame = yTrue;
                else
                    continueLooping = yFalse;
            }
        }
        break;

        case YOP_ENTER:
        {
            yapValue *blockRef = yapArrayPop(&Y->stack);

            if(blockRef && blockRef->type == YVT_BLOCK && blockRef->blockVal)
            {
                yU32 frameType = operand;
                frame = yapContextPushFrame(Y, blockRef->blockVal, 0, frameType, NULL);
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
                yapValue *cond = yapArrayPop(&Y->stack);
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
                yapFrameReset(frame, yTrue);
                newFrame = yTrue;
            }
            else
            {
                continueLooping = yFalse;
            }
        }
        break;

        case YOP_TOSTRING:
        {
            yapValue *value = yapArrayPop(&Y->stack);
            if(!value)
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_TOSTRING: empty stack!");
                continueLooping = yFalse;
                break;
            };
            yapArrayPush(&Y->stack, yapValueToString(Y, value));
        }
        break;

        case YOP_TOINT:
        {
            yapValue *value = yapArrayPop(&Y->stack);
            if(!value)
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_TOINT: empty stack!");
                continueLooping = yFalse;
                break;
            };
            yapArrayPush(&Y->stack, yapValueToInt(Y, value));
        }
        break;

        case YOP_TOFLOAT:
        {
            yapValue *value = yapArrayPop(&Y->stack);
            if(!value)
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_TOFLOAT: empty stack!");
                continueLooping = yFalse;
                break;
            };
            yapArrayPush(&Y->stack, yapValueToFloat(Y, value));
        }
        break;

        case YOP_NOT:
        {
            yapValue *value = yapArrayPop(&Y->stack);
            if(!value)
            {
                yapContextSetError(Y, YVE_RUNTIME, "YOP_NOT: empty stack!");
                continueLooping = yFalse;
                break;
            };
            value = yapValueToBool(Y, value);
            value = yapValueSetInt(Y, value, !value->intVal); // Double temporary?
            yapArrayPush(&Y->stack, value);
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
            yapArrayPush(&Y->stack, yapValueSetInt(Y, yapValueAcquire(Y), ret));
        }
        break;

        case YOP_FORMAT:
        {
            yapValue *format = yapArrayPop(&Y->stack);
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
            yapArrayPush(&Y->stack, val);
        }
        break;

        case YOP_NTH:
        {
            yapValue *val = yapArrayPop(&Y->stack);

            if(val->type == YVT_ARRAY)
            {
                yapValue *nth = yapArrayPop(&Y->stack);
                if(nth->intVal >= 0 && nth->intVal < val->arrayVal->count)
                {
                    yapArrayPush(&Y->stack, val->arrayVal->data[nth->intVal]);
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
            yapValue *val = yapArrayPop(&Y->stack);
            if(val->type == YVT_ARRAY)
            {
                yapValue *count = yapValueAcquire(Y);
                count = yapValueSetInt(Y, count, val->arrayVal->count);
                yapArrayPush(&Y->stack, count);
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

static void yapHashEntryVariableMark(yapContext *Y, yapHashEntry *entry)
{
    yapVariableMark(Y, entry->value);
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

    for(i = 0; i < Y->usedVariables.count; i++)
    {
        yapVariable *variable = (yapVariable *)Y->usedVariables.data[i];
        variable->used = yFalse;
    }

    // -----------------------------------------------------------------------
    // mark all values used by things the VM still cares about

    yapHashIterateP1(Y->globals, (yapIterateCB1)yapHashEntryVariableMark, Y);

    for(i = 0; i < Y->frames.count; i++)
    {
        yapFrame *frame = (yapFrame *)Y->frames.data[i];
        if(frame->thisVal)
            yapValueMark(Y, frame->thisVal);
        yapHashIterateP1(frame->locals, (yapIterateCB1)yapHashEntryVariableMark, Y);
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
            yapValueRelease(Y, value);
            Y->usedValues.data[i] = NULL; // for future squashing
        }
    }
    yapArraySquash(&Y->usedValues);

    for(i = 0; i < Y->usedVariables.count; i++)
    {
        yapVariable *variable = (yapVariable *)Y->usedVariables.data[i];
        if(!variable->used)
        {
            yapVariableDestroy(variable);
            Y->usedVariables.data[i] = NULL; // for future squashing
        }
    }

    yapArraySquash(&Y->usedVariables);
}
