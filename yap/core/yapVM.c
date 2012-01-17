// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapVM.h"

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

void yapVMRegisterGlobal(yapVM *vm, const char *name, yapValue *value)
{
    yapVariable *variable = yapVariableCreate(vm, value);
    yapHashSet(vm->globals, name, variable);
}

void yapVMRegisterGlobalFunction(yapVM *vm, const char *name, yapCFunction func)
{
    yapVMRegisterGlobal(vm, name, yapValueSetCFunction(vm, yapValueAcquire(vm), func));
}

static yBool yapChunkCanBeTemporary(yapChunk *chunk)
{
    // A simple test ... if there aren't any functions in this chunk and all ktable lookups
    // are duped (chunk flagged as temporary), than the chunk can safely go away in between
    // calls to yapVMLoop (assuming zero yapFrames on the stack).
    return !chunk->hasFuncs;
}

void yapVMEval(yapVM *vm, const char *text, yU32 evalOpts)
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
            yapVMSetError(vm, YVE_COMPILE, s);
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
            yapVMPushFrame(vm, chunk->block, 0, YFT_FUNC|YFT_CHUNK, yapValueNullPtr);
            yapVMLoop(vm, yTrue);

#ifdef YAP_TRACE_OPS
            printf("---  end  chunk execution ---\n");
#endif
            if(!chunk->temporary)
            {
                yapArrayPush(&vm->chunks, chunk);
                chunk = NULL; // forget the ptr
            }
        }

        if(chunk)
        {
            yapChunkDestroy(chunk);
        }
    }

#ifdef YAP_ENABLE_MEMORY_STATS
    yapMemoryStatsPrint("yapVMEval: ");
#endif
}

yapVM *yapVMCreate(void)
{
    yapVM *vm = yapAlloc(sizeof(yapVM));
    vm->globals = yapHashCreate(0);
    yapValueTypeRegisterAllBasicTypes(vm);
    yapIntrinsicsRegister(vm);
    yapModuleRegisterAll(vm);
    return vm;
}

void yapVMRecover(yapVM *vm)
{
    if(vm->errorType == YVE_RUNTIME)
    {
        int prevStackCount = 0;

        yapFrame *frame = yapVMPopFrames(vm, YFT_CHUNK, yTrue);
        if(frame) // recovery should work on an empty frame stack
        {
            prevStackCount = frame->prevStackCount;
            yapVMPopFrames(vm, YFT_CHUNK, yFalse);
        }
        yapArrayShrink(&vm->stack, prevStackCount, NULL);

        yapVMGC(vm);
    }
    yapVMClearError(vm);
}

void yapVMSetError(yapVM *vm, yU32 errorType, const char *errorFormat, ...)
{
    va_list args;
    char tempStr[MAX_ERROR_LENGTH + 1];

    yapVMClearError(vm);
    va_start(args, errorFormat);
    vsprintf(tempStr, errorFormat, args);
    va_end(args);

    vm->errorType = errorType;
    vm->error = yapStrdup(tempStr);
}

void yapVMClearError(yapVM *vm)
{
    if(vm->error)
    {
        yapFree(vm->error);
        vm->error = NULL;
    }
    vm->errorType = YVE_NONE;
}

void yapVMDestroy(yapVM *vm)
{
    yapHashDestroy(vm->globals, NULL);
    yapArrayClear(&vm->frames, (yapDestroyCB)yapFrameDestroy);
    yapArrayClear(&vm->stack, NULL);
    yapArrayClear(&vm->chunks, (yapDestroyCB)yapChunkDestroy);

    yapArrayClear(&vm->usedVariables, (yapDestroyCB)yapVariableDestroy);
    yapArrayClearP1(&vm->usedValues, (yapDestroyCB1)yapValueDestroy, vm);
    yapArrayClearP1(&vm->freeValues, (yapDestroyCB1)yapValueDestroy, vm);

    yapArrayClear(&vm->types, (yapDestroyCB)yapValueTypeDestroy);
    yapVMClearError(vm);

    yapFree(vm);
}

static yapVariable *yapVMResolveVariable(yapVM *vm, const char *name)
{
    int i;
    yapFrame *frame;
    yapVariable **variableRef;

    for(i = vm->frames.count - 1; i >= 0; i--)
    {
        frame = (yapFrame *)vm->frames.data[i];

        // Check the local variables
        variableRef = (yapVariable **)yapHashLookup(frame->locals, name, yFalse);
        if(variableRef) return *variableRef;

        if(frame->type == YFT_FUNC)
            break;
    }

    // check global vars
    variableRef = (yapVariable **)yapHashLookup(vm->globals, name, yFalse);
    if(variableRef) return *variableRef;

    return NULL;
}

yapFrame *yapVMPushFrame(yapVM *vm, yapBlock *block, int argCount, yU32 frameType, struct yapValue *thisVal)
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
                yapArrayPop(&vm->stack);
        }
        else if(block->argCount > argCount)
        {
            // Too few arguments -- pad with nulls
            int i;
            for(i = 0; i < (block->argCount - argCount); i++)
                yapArrayPush(&vm->stack, &yapValueNull);
        }
    }

    frame = yapFrameCreate(frameType, thisVal, block, vm->stack.count, argCount);
    yapArrayPush(&vm->frames, frame);

    return frame;
}

static yBool yapVMCallCFunction(yapVM *vm, yapCFunction func, yU32 argCount, yapValue *thisVal);
static yBool yapVMCreateObject(yapVM *vm, yapFrame **framePtr, yapValue *isa, int argCount);

static yBool yapVMCall(yapVM *vm, yapFrame **framePtr, yapValue *thisVal, yapValue *callable, int argCount)
{
    if(!callable)
    {
        yapVMSetError(vm, YVE_RUNTIME, "YOP_CALL: empty stack!");
        return yFalse;
    }
    if(callable->type == YVT_REF)
    {
        callable = *callable->refVal;
    }
    if(!yapValueIsCallable(callable))
    {
        yapVMSetError(vm, YVE_RUNTIME, "YOP_CALL: variable not callable");
        return yFalse;
    }
    if(callable->type == YVT_CFUNCTION)
    {
        if(!yapVMCallCFunction(vm, *callable->cFuncVal, argCount, thisVal))
            return yFalse;
    }
    else if(callable->type == YVT_OBJECT)
    {
        return yapVMCreateObject(vm, framePtr, callable, argCount);
    }
    else
    {
        *framePtr = yapVMPushFrame(vm, callable->blockVal, argCount, YFT_FUNC, thisVal);
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

static yapValue *yapFindFunc(yapVM *vm, yapValue *object, const char *name)
{
    yapValue *v = *(yapObjectGetRef(vm, object->objectVal, name, yFalse));
    if(v == &yapValueNull)
    {
        if(object->objectVal->isa)
            return yapFindFunc(vm, object->objectVal->isa, name);
        return NULL;
    }
    else
    {
        if(!yapValueIsCallable(v))
            return NULL;
    }
    return v;
}

yBool yapVMCallFuncByName(yapVM *vm, yapValue *thisVal, const char *name, int argCount)
{
    yapFrame *frame = NULL;
    yapVariable *variable = yapVMResolveVariable(vm, name);
    if(!variable)
        return yFalse;
    if(yapVMCall(vm, &frame, thisVal, variable->value, argCount))
    {
        yapVMLoop(vm, yTrue);
        return yTrue;
    }
    return yFalse;
}

static yBool yapVMCreateObject(yapVM *vm, yapFrame **framePtr, yapValue *isa, int argCount)
{
    yBool ret = yTrue;
    yapValue *initFunc = yapFindFunc(vm, isa, "init");
    yapValue *newObject = yapValueObjectCreate(vm, isa, (initFunc) ? 0 : argCount);

    if(initFunc)
        return yapVMCall(vm, framePtr, newObject, initFunc, argCount);

    yapArrayPush(&vm->stack, newObject);
    return ret;
}

// TODO: this needs to protect against variable masking/shadowing
static yapVariable *yapVMRegisterVariable(yapVM *vm, const char *name, yapValue *value)
{
    yapVariable *variable;
    yapFrame *frame = yapArrayTop(&vm->frames);
    if(!frame)
        return NULL;

    variable = yapVariableCreate(vm, value);
    if(frame->block == frame->block->chunk->block)
    {
        // If we're in the chunk's "main" function, all variable
        // registration goes into the globals
        yapHashSet(vm->globals, name, variable);
    }
    else
    {
        yapHashSet(frame->locals, name, variable);
    }
    return variable;
}

static void yapVMPushRef(yapVM *vm, yapVariable *variable)
{
    yapValue *value = yapValueSetRef(vm, yapValueAcquire(vm), &variable->value);
    yapArrayPush(&vm->stack, value);
}

// TODO: merge this function with PushFrame and _RET
static yBool yapVMCallCFunction(yapVM *vm, yapCFunction func, yU32 argCount, yapValue *thisVal)
{
    int retCount;
    yapFrame *frame = yapFrameCreate(YFT_FUNC, thisVal, NULL, vm->stack.count, argCount);
    yapArrayPush(&vm->frames, frame);

    retCount = func(vm, argCount);

    yapArrayPop(&vm->frames); // Removes 'frame' from top of stack
    yapFrameDestroy(frame);
    frame = yapArrayTop(&vm->frames);
    if(!frame)
        return yFalse;

    // Stash lastRet for any YOP_KEEPs in the pipeline
    vm->lastRet = retCount;
    return yTrue;
}

void yapVMPopValues(yapVM *vm, yU32 count)
{
    while(count)
    {
        yapArrayPop(&vm->stack);
        count--;
    }
}

yapValue *yapVMGetValue(yapVM *vm, yU32 howDeep)
{
    if(howDeep >= vm->stack.count)
        return NULL;

    return vm->stack.data[(vm->stack.count - 1) - howDeep];
}

static yapVMFrameCleanup(yapVM *vm, yapFrame *frame)
{
    if(frame->cleanupCount)
    {
        int i;
        for(i=0; i < frame->cleanupCount; i++)
        {
            int index = ((vm->stack.count - 1) - vm->lastRet) - i;
            vm->stack.data[index] = NULL;
        }
        yapArraySquash(&vm->stack);
    }
}

struct yapFrame *yapVMPopFrames(yapVM *vm, yU32 frameTypeToFind, yBool keepIt)
{
    yapFrame *frame = yapArrayTop(&vm->frames);

    if(frameTypeToFind != YFT_ANY)
    {
        while(frame && !(frame->type & frameTypeToFind))
        {
            yapVMFrameCleanup(vm, frame);
            yapFrameDestroy(frame);
            yapArrayPop(&vm->frames);
            frame = yapArrayTop(&vm->frames);
        };
    }

    if(frame && !keepIt)
    {
        yapVMFrameCleanup(vm, frame);
        yapFrameDestroy(frame);
        yapArrayPop(&vm->frames);
        frame = yapArrayTop(&vm->frames);
    }

    return frame;
}

static yS32 yapVMPopInts(yapVM *vm, int count, int *output)
{
    yS32 i;
    for(i=0; i<count; i++)
    {
        yapValue *v = yapArrayPop(&vm->stack);
        if(!v)
            return i;
        v = yapValueToInt(vm, v);
        if(!v)
            return i;
        output[i] = v->intVal;
    }
    return i;
}

yapValue * yapVMThis(yapVM *vm)
{
    int i;
    for(i = vm->frames.count - 1; i >= 0; i--)
    {
        yapFrame *frame = (yapFrame *)vm->frames.data[i];
        if(frame->type & YFT_FUNC)
            return frame->thisVal;
    }
    return yapValueNullPtr;
}

yBool yapVMGetArgs(yapVM *vm, int argCount, const char *argFormat, ...)
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
                yapVMPopValues(vm, argCount);
                return yTrue;
            }
            return yFalse;
        };

        v = yapVMGetArg(vm, argsTaken, argCount);
        if(!v)
        {
            // this is a very serious failure (argCount doesn't agree with yapVMGetArg)
            yapVMSetError(vm, YVE_RUNTIME, "yapVMGetArgs(): VM stack and argCount disagree!");
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
            yapArrayPush(leftovers, yapVMGetArg(vm, argsTaken, argCount));
        }
    }

    if(argsTaken != argCount)
    {
        // too many args!
        return yFalse;
    }

    va_end(args);
    yapVMPopValues(vm, argCount);
    return yTrue;
}

// TODO: reuse code between yapVMArgsFailure and yapVMSetError
int yapVMArgsFailure(yapVM *vm, int argCount, const char *errorFormat, ...)
{
    va_list args;
    char tempStr[MAX_ERROR_LENGTH + 1];

    yapVMClearError(vm);
    va_start(args, errorFormat);
    vsprintf(tempStr, errorFormat, args);
    va_end(args);

    vm->errorType = YVE_RUNTIME;
    vm->error = yapStrdup(tempStr);

    yapVMPopValues(vm, argCount);
    return 0;
}

#ifdef YAP_TRACE_OPS
static const char *yapValueDebugString(yapVM *vm, yapValue *v)
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
        sprintf(valString, "(%s)", v->stringVal);
        break;
    case YVT_ARRAY:
        sprintf(valString, "(count: %d)", v->arrayVal->count);
        break;
    }

    sprintf(buffer, "%s 0x%p %s", yapValueTypeName(vm, v->type), v, valString);
    return buffer;
}

static void yapVMLogState(yapVM *vm)
{
    int i;

    printf("\n\n\n------------------------------------------\n");

    if(vm->frames.count > 0)
    {
        yapFrame *frame = yapArrayTop(&vm->frames);
        printf("0x%p [cleanup:%d][lastRet:%d] IP: ", frame, frame->cleanupCount, vm->lastRet);
        yapOpsDump(frame->ip, 1);
    }

    printf("\n");
    printf("-- Stack --\n");

    for(i=0; i<vm->stack.count; i++)
    {
        yapValue *v = (yapValue*)vm->stack.data[vm->stack.count - 1 - i];
        printf("%2.2d: %s\n", i, yapValueDebugString(vm, v));
    }
}
#endif

void yapVMLoop(yapVM *vm, yBool stopAtPop)
{
    yapFrame *frame = yapArrayTop(&vm->frames);
    yBool continueLooping = yTrue;
    yBool newFrame;
    yU16 opcode;
    yU16 operand;
    yU32 startingFrameCount = vm->frames.count;

    if(!frame)
    {
        yapVMSetError(vm, YVE_RUNTIME, "yapVMLoop(): No stack frame!");
        return;
    }

    // Main VM loop!
    while(continueLooping && !vm->error)
    {
        if(stopAtPop && (vm->frames.count < startingFrameCount))
            break;

        newFrame = yFalse;

        // These are put into temporary variables for future ntohs() cross-platform safety
        opcode  = frame->ip->opcode;
        operand = frame->ip->operand;

#ifdef YAP_TRACE_OPS
        yapVMLogState(vm);
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
            yapValue *performSkipValue = yapArrayTop(&vm->stack);
            yBool performSkip = yFalse;
            if(!performSkipValue)
            {
                yapVMSetError(vm, YVE_RUNTIME, "YOP_SKIP: empty stack!");
                continueLooping = yFalse;
            }
            performSkipValue = yapValueToBool(vm, performSkipValue);
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
                yapArrayPop(&vm->stack);
            }
        }
        break;

        case YOP_PUSHNULL:
        {
            yapArrayPush(&vm->stack, &yapValueNull);
        }
        break;

        case YOP_PUSHTHIS:
        {
            yapArrayPush(&vm->stack, yapVMThis(vm));
        }
        break;

        case YOP_PUSHI:
        {
            yapValue *value = yapValueSetInt(vm, yapValueAcquire(vm), operand);
            yapArrayPush(&vm->stack, value);
        }
        break;

        case YOP_PUSH_KB:
        {
            yapValue *value = yapValueSetFunction(vm, yapValueAcquire(vm), frame->block->chunk->blocks.data[operand]);
            yapArrayPush(&vm->stack, value);
        }
        break;

        case YOP_PUSH_KI:
        {
            yapValue *value = yapValueSetInt(vm, yapValueAcquire(vm), frame->block->chunk->kInts.data[operand]);
            yapArrayPush(&vm->stack, value);
        }
        break;

        case YOP_PUSH_KF:
        {
            yapValue *value = yapValueSetFloat(vm, yapValueAcquire(vm), *((yF32*)&frame->block->chunk->kFloats.data[operand]));
            yapArrayPush(&vm->stack, value);
        }
        break;

        case YOP_PUSH_KS:
        {
            yapValue *value;
            if(frame->block->chunk->temporary)
                value = yapValueSetString(vm, yapValueAcquire(vm), frame->block->chunk->kStrings.data[operand]);
            else
                value = yapValueSetKString(vm, yapValueAcquire(vm), frame->block->chunk->kStrings.data[operand]);

            yapArrayPush(&vm->stack, value);
        }
        break;

        case YOP_VARREG_KS:
        {
            yapVariable *variable = yapVMRegisterVariable(vm, frame->block->chunk->kStrings.data[operand], yapValueNullPtr);
            yapVMPushRef(vm, variable);
        }
        break;

        case YOP_VARREF_KS:
        {
            yapVariable *variable = yapVMResolveVariable(vm, frame->block->chunk->kStrings.data[operand]);
            if(variable)
            {
                yapVMPushRef(vm, variable);
            }
            else
            {
                yapVMSetError(vm, YVE_RUNTIME, "YOP_GETVAR_KS: no variable named '%s'", frame->block->chunk->kStrings.data[operand]);
                continueLooping = yFalse;
            }
        }
        break;

        case YOP_REFVAL:
        {
            yapValue *value = yapArrayPop(&vm->stack);
            if(!value)
            {
                yapVMSetError(vm, YVE_RUNTIME, "YOP_REFVAL: empty stack!");
                continueLooping = yFalse;
                break;
            };
            if(value->type != YVT_REF)
            {
                yapVMSetError(vm, YVE_RUNTIME, "YOP_REFVAL: requires ref on top of stack");
                continueLooping = yFalse;
                break;
            }
            yapArrayPush(&vm->stack, *value->refVal);
        }
        break;

        case YOP_ADD:
        {
            yapValue *b = yapArrayPop(&vm->stack);
            yapValue *a = yapArrayPop(&vm->stack);
            a = yapValueAdd(vm, a, b);
            if(a)
                yapArrayPush(&vm->stack, a);
            else
                continueLooping = yFalse;
        }
        break;

        case YOP_SUB:
        {
            yapValue *b = yapArrayPop(&vm->stack);
            yapValue *a = yapArrayPop(&vm->stack);
            yapValue *c = yapValueSub(vm, a, b);
            if(operand)
            {
                // Leave entries on the stack. Used in for loops.
                yapArrayPush(&vm->stack, a);
                yapArrayPush(&vm->stack, b);
            }
            if(c)
                yapArrayPush(&vm->stack, c);
            else
                continueLooping = yFalse;
        }
        break;

        case YOP_MUL:
        {
            yapValue *b = yapArrayPop(&vm->stack);
            yapValue *a = yapArrayPop(&vm->stack);
            a = yapValueMul(vm, a, b);
            if(a)
                yapArrayPush(&vm->stack, a);
            else
                continueLooping = yFalse;
        }
        break;

        case YOP_DIV:
        {
            yapValue *b = yapArrayPop(&vm->stack);
            yapValue *a = yapArrayPop(&vm->stack);
            a = yapValueDiv(vm, a, b);
            if(a)
                yapArrayPush(&vm->stack, a);
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
            yapValue *b = yapArrayPop(&vm->stack);
            yapValue *a = yapArrayPop(&vm->stack);
            yS32 cmp = yapValueCmp(vm, a, b);
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
            yapArrayPush(&vm->stack, yapValueSetInt(vm, a, cmp));
        }
        break;

        case YOP_SETVAR:
        {
            yapValue *ref = yapArrayPop(&vm->stack);
            yapValue *val = yapArrayTop(&vm->stack);
            if(!operand)
                yapArrayPop(&vm->stack);
            continueLooping = yapValueSetRefVal(vm, ref, val);
        }
        break;

        case YOP_INHERITS:
        {
            yapValue *ref = yapArrayPop(&vm->stack);
            yapValue *val = yapArrayPop(&vm->stack);
            continueLooping = yapValueSetRefInherits(vm, ref, val);
            if(continueLooping && operand)
            {
                yapArrayPush(&vm->stack, *ref->refVal);
            }
        }
        break;

        case YOP_VARARGS:
        {
            int i;
            int varargCount = frame->argCount - operand;
            yapValue *varargsArray = yapValueArrayCreate(vm);

            // Only one of these for loops will actually loop
            for(; varargCount < 0; varargCount++)
                yapArrayPush(&vm->stack, &yapValueNull);
            for(; varargCount > 0; varargCount--)
                yapArrayUnshift(varargsArray->arrayVal, yapArrayPop(&vm->stack));

            yapArrayPush(&vm->stack, varargsArray);
        }
        break;

        case YOP_INDEX:
        {
            yapValue *index = yapArrayPop(&vm->stack);
            yapValue *value = yapArrayPop(&vm->stack);
            if(value && index)
            {
                int opFlags = operand;
                yapValue *ret = yapValueIndex(vm, value, index, (opFlags & YOF_LVALUE) ? yTrue : yFalse);
                if(ret)
                {
                    yapArrayPush(&vm->stack, ret);
                    if(opFlags & YOF_PUSHOBJ)
                        yapArrayPush(&vm->stack, value);
                    else if(opFlags & YOF_PUSHTHIS)
                        yapArrayPush(&vm->stack, yapVMThis(vm));
                }
                else
                {
                    if(vm->errorType == YVE_NONE)
                        yapVMSetError(vm, YVE_RUNTIME, "YOP_INDEX: Failed attempt to index into type %s", yapValueTypeName(vm, value->type));
                    continueLooping = yFalse;
                }
            }
            else
            {
                yapVMSetError(vm, YVE_RUNTIME, "YOP_INDEX: empty stack!");
                continueLooping = yFalse;
            }
        }
        break;

        case YOP_DUPE:
        case YOP_MOVE:
        {
            int topIndex = vm->stack.count - 1;
            int requestedIndex = topIndex - operand;
            if(requestedIndex >= 0)
            {
                yapValue *val = vm->stack.data[requestedIndex];
                if(opcode == YOP_MOVE)
                {
                    if(operand == 0)
                        break; // no sense in moving the top to the top
                    vm->stack.data[requestedIndex] = NULL;
                    yapArraySquash(&vm->stack);
                }
                yapArrayPush(&vm->stack, val);
            }
            else
            {
                yapVMSetError(vm, YVE_RUNTIME, "%s: impossible index", (opcode == YOP_DUPE) ? "YOP_DUPE" : "YOP_MOVE");
                continueLooping = yFalse;
            }
        }
        break;

        case YOP_POP:
        {
            int i;
            for(i = 0; i < operand; i++)
                yapArrayPop(&vm->stack);
        }
        break;

        case YOP_CALL:
        {
            int argCount = operand;
            yapFrame *oldFrame = frame;
            yapValue *thisVal = yapArrayPop(&vm->stack);
            yapValue *callable = yapArrayPop(&vm->stack);
            continueLooping = yapVMCall(vm, &frame, thisVal, callable, argCount);
            if(frame != oldFrame)
                newFrame = yTrue;
        }
        break;

        case YOP_RET:
        {
            int argCount = operand;
            frame = yapVMPopFrames(vm, YFT_FUNC, yTrue);
            //                if(!argCount && frame && (frame->flags & YFF_INIT) && frame->thisVal)
            //                {
            //                    // We are leaving an init function that does not return any value (which
            //                    // is different from returning null). This bit of magic makes init() functions
            //                    // return 'this' instead of nothing. It avoids the boilerplate 'return this'
            //                    // at the end of every init() in order for it to work at all, but allows for
            //                    // generator init functions.
            //                    argCount = 1;
            //                    yapArrayPush(&vm->stack, frame->thisVal);
            //                }
            frame = yapVMPopFrames(vm, YFT_FUNC, yFalse);
            if(frame)
            {
                // Stash lastRet for any YOP_KEEPs in the pipeline
                vm->lastRet = argCount;
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
            vm->lastRet = 0; // reset this here in case as a 'normal' termination of a for loop won't ever set it, but it is legal to override it with a return
        }
        break;

        case YOP_KEEP:
        {
            int keepCount = operand;
            int offerCount = vm->lastRet;

            if(keepCount < offerCount)
            {
                int i;
                for(i = 0; i < (offerCount - keepCount); i++)
                {
                    yapTrace(("-- cleaning stack entry --\n"));
                    yapArrayPop(&vm->stack);
                }
            }
            else if(keepCount > offerCount)
            {
                int i;
                for(i = 0; i < (keepCount - offerCount); i++)
                {
                    yapTrace(("-- padding stack with null --\n"));
                    yapArrayPush(&vm->stack, &yapValueNull);
                }
            }
            vm->lastRet = 0;
        }
        break;

        case YOP_CLOSE:
        {
            yapValue *v = yapArrayTop(&vm->stack);
            yapValueAddClosureVars(vm, v);
        }
        break;

        case YOP_IF:
        {
            yapBlock *block = NULL;
            yapValue *cond, *ifBody, *elseBody = NULL;
            cond   = yapArrayPop(&vm->stack);
            ifBody = yapArrayPop(&vm->stack);
            if(operand)
                elseBody = yapArrayPop(&vm->stack);
            // TODO: verify ifBody/elseBody are YVT_BLOCK
            cond = yapValueToBool(vm, cond);
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
                frame = yapVMPushFrame(vm, block, 0, YFT_COND, NULL);
                if(frame)
                    newFrame = yTrue;
                else
                    continueLooping = yFalse;
            }
        }
        break;

        case YOP_ENTER:
        {
            yapValue *blockRef = yapArrayPop(&vm->stack);

            if(blockRef && blockRef->type == YVT_BLOCK && blockRef->blockVal)
            {
                yU32 frameType = operand;
                frame = yapVMPushFrame(vm, blockRef->blockVal, 0, frameType, NULL);
                if(frame)
                    newFrame = yTrue;
                else
                    continueLooping = yFalse;
            }
            else
            {
                yapVMSetError(vm, YVE_RUNTIME, "hurr");
                continueLooping = yFalse;
            }
        }
        break;

        case YOP_LEAVE:
        {
            yBool performLeave = yTrue;
            if(operand)
            {
                yapValue *cond = yapArrayPop(&vm->stack);
                cond = yapValueToBool(vm, cond);
                performLeave   = !cond->intVal; // don't leave if expr is true!
            }

            if(performLeave)
            {
                frame = yapVMPopFrames(vm, YFT_ANY, yFalse);
                continueLooping = (frame) ? yTrue : yFalse;
            }
        }
        break;

        case YOP_BREAK:
        {
            // a C-style break. Find the innermost loop and kill it.
            frame = yapVMPopFrames(vm, YFT_LOOP, yFalse);
            continueLooping = (frame) ? yTrue : yFalse;
        }
        break;

        case YOP_CONTINUE:
        {
            // a C-style continue. Find the innermost loop and reset it.
            frame = yapVMPopFrames(vm, YFT_LOOP, yTrue);
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
            yapValue *value = yapArrayPop(&vm->stack);
            if(!value)
            {
                yapVMSetError(vm, YVE_RUNTIME, "YOP_TOSTRING: empty stack!");
                continueLooping = yFalse;
                break;
            };
            yapArrayPush(&vm->stack, yapValueToString(vm, value));
        }
        break;

        case YOP_TOINT:
        {
            yapValue *value = yapArrayPop(&vm->stack);
            if(!value)
            {
                yapVMSetError(vm, YVE_RUNTIME, "YOP_TOINT: empty stack!");
                continueLooping = yFalse;
                break;
            };
            yapArrayPush(&vm->stack, yapValueToInt(vm, value));
        }
        break;

        case YOP_TOFLOAT:
        {
            yapValue *value = yapArrayPop(&vm->stack);
            if(!value)
            {
                yapVMSetError(vm, YVE_RUNTIME, "YOP_TOFLOAT: empty stack!");
                continueLooping = yFalse;
                break;
            };
            yapArrayPush(&vm->stack, yapValueToFloat(vm, value));
        }
        break;

        case YOP_NOT:
        {
            yapValue *value = yapArrayPop(&vm->stack);
            if(!value)
            {
                yapVMSetError(vm, YVE_RUNTIME, "YOP_NOT: empty stack!");
                continueLooping = yFalse;
                break;
            };
            value = yapValueToBool(vm, value);
            value = yapValueSetInt(vm, value, !value->intVal); // Double temporary?
            yapArrayPush(&vm->stack, value);
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
            if(yapVMPopInts(vm, argsNeeded, i) != argsNeeded)
            {
                yapVMSetError(vm, YVE_RUNTIME, "Bitwise operations require integer friendly arguments");
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
            yapArrayPush(&vm->stack, yapValueSetInt(vm, yapValueAcquire(vm), ret));
        }
        break;

        case YOP_FORMAT:
        {
            yapValue *format = yapArrayPop(&vm->stack);
            yapValue *val;
            if(!format)
            {
                yapVMSetError(vm, YVE_RUNTIME, "YOP_FORMAT: empty stack!");
                continueLooping = yFalse;
                break;
            };
            val = yapValueStringFormat(vm, format, operand);
            if(!val)
            {
                yapVMSetError(vm, YVE_RUNTIME, "YOP_FORMAT: bad format");
                continueLooping = yFalse;
                break;
            };
            yapArrayPush(&vm->stack, val);
        }
        break;

        case YOP_NTH:
        {
            yapValue *val = yapArrayPop(&vm->stack);

            if(val->type == YVT_ARRAY)
            {
                yapValue *nth = yapArrayPop(&vm->stack);
                if(nth->intVal >= 0 && nth->intVal < val->arrayVal->count)
                {
                    yapArrayPush(&vm->stack, val->arrayVal->data[nth->intVal]);
                    vm->lastRet = 1;
                }
                else
                {
                    yapVMSetError(vm, YVE_RUNTIME, "YOP_NTH: index out of range");
                    continueLooping = yFalse;
                    break;
                }
            }
            else if(val->type == YVT_OBJECT)
            {
                yapFrame *oldFrame = frame;
                yapValue *getFunc = yapFindFunc(vm, val, "get");
                if(!getFunc)
                {
                    yapVMSetError(vm, YVE_RUNTIME, "YVT_NTH: iterable does not have a get() function");
                    continueLooping = yFalse;
                    break;
                }
                continueLooping = yapVMCall(vm, &frame, val, getFunc, 1 /* the index */);
                if(frame != oldFrame)
                    newFrame = yTrue;
            }
            else
            {
                yapVMSetError(vm, YVE_RUNTIME, "YOP_NTH: Invalid value type %d", val->type);
                continueLooping = yFalse;
                break;
            }
        }
        break;

        case YOP_COUNT:
        {
            yapValue *val = yapArrayPop(&vm->stack);
            if(val->type == YVT_ARRAY)
            {
                yapValue *count = yapValueAcquire(vm);
                count = yapValueSetInt(vm, count, val->arrayVal->count);
                yapArrayPush(&vm->stack, count);
                vm->lastRet = 1;
            }
            else if(val->type == YVT_OBJECT)
            {
                yapFrame *oldFrame = frame;
                yapValue *countFunc = yapFindFunc(vm, val, "count");
                if(!countFunc)
                {
                    yapVMSetError(vm, YVE_RUNTIME, "YVT_COUNT: iterable does not have a count() function");
                    continueLooping = yFalse;
                    break;
                }
                continueLooping = yapVMCall(vm, &frame, val, countFunc, 0);
                if(frame != oldFrame)
                    newFrame = yTrue;
            }
            else
            {
                yapVMSetError(vm, YVE_RUNTIME, "YVT_COUNT: Invalid value type %d", val->type);
                continueLooping = yFalse;
                break;
            }
        }
        break;

        default:
            yapVMSetError(vm, YVE_RUNTIME, "Unknown VM Opcode: %d", opcode);
            continueLooping = yFalse;
            break;
        }

        if(continueLooping && !newFrame)
            frame->ip++;

        yapVMGC(vm); // TODO: Only do this when (new var) vm->gc == 0, then reset
    }

    yapVMGC(vm);
}

static void yapHashEntryVariableMark(yapVM *vm, yapHashEntry *entry)
{
    yapVariableMark(vm, entry->value);
}

void yapVMGC(struct yapVM *vm)
{
    int i, j;

    // -----------------------------------------------------------------------
    // walk all used values and clear their GC flags

    for(i = 0; i < vm->usedValues.count; i++)
    {
        yapValue *value = (yapValue *)vm->usedValues.data[i];
        value->used = yFalse;
    }

    for(i = 0; i < vm->usedVariables.count; i++)
    {
        yapVariable *variable = (yapVariable *)vm->usedVariables.data[i];
        variable->used = yFalse;
    }

    // -----------------------------------------------------------------------
    // mark all values used by things the VM still cares about

    yapHashIterateP1(vm->globals, (yapIterateCB1)yapHashEntryVariableMark, vm);

    for(i = 0; i < vm->frames.count; i++)
    {
        yapFrame *frame = (yapFrame *)vm->frames.data[i];
        if(frame->thisVal)
            yapValueMark(vm, frame->thisVal);
        yapHashIterateP1(frame->locals, (yapIterateCB1)yapHashEntryVariableMark, vm);
    }

    for(i = 0; i < vm->stack.count; i++)
    {
        yapValue *value = (yapValue *)vm->stack.data[i];
        yapValueMark(vm, value);
    }

    // -----------------------------------------------------------------------
    // sweep!

    for(i = 0; i < vm->usedValues.count; i++)
    {
        yapValue *value = (yapValue *)vm->usedValues.data[i];
        if(!value->used)
        {
            yapValueRelease(vm, value);
            vm->usedValues.data[i] = NULL; // for future squashing
        }
    }
    yapArraySquash(&vm->usedValues);

    for(i = 0; i < vm->usedVariables.count; i++)
    {
        yapVariable *variable = (yapVariable *)vm->usedVariables.data[i];
        if(!variable->used)
        {
            yapVariableDestroy(variable);
            vm->usedVariables.data[i] = NULL; // for future squashing
        }
    }

    yapArraySquash(&vm->usedVariables);
}
