#include "yapVM.h"

#include "yapBlock.h"
#include "yapCompiler.h"
#include "yapObject.h"
#include "yapFrame.h"
#include "yapChunk.h"
#include "yapOp.h"
#include "yapValue.h"
#include "yapVariable.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_ERROR_LENGTH 1023

void yapVMRegisterGlobal(yapVM *vm, const char *name, yapValue *value)
{
    yapVariable *global = yapVariableCreate(vm, name);
    global->value = value;
    yapArrayPush(&vm->globals, global);
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
    yapVariable *chunkRef;
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
                yapChunkDump(chunk);
            }

#ifdef YAP_TRACE_OPS
            printf("--- begin chunk execution ---\n");
#endif
            // Execute the chunk's block
            yapVMPushFrame(vm, chunk->block, 0, YFT_FUNC|YFT_CHUNK);
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
    yapArrayClear(&vm->globals, NULL);
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

static yapVariable *yapArrayFindVariableByName(yapArray *a, const char *name)
{
    int i;
    for(i = 0; i < a->count; i++)
    {
        yapVariable *v = (yapVariable *)a->data[i];
        if(!strcmp(v->name, name))
        {
            return v;
        }
    }
    return NULL;
}

static yapVariable *yapVMResolveVariable(yapVM *vm, const char *name)
{
    int i;
    yapFrame *frame;
    yapVariable *v;

    for(i = vm->frames.count - 1; i >= 0; i--)
    {
        frame = (yapFrame *)vm->frames.data[i];

        // Check the local variables
        v = yapArrayFindVariableByName(&frame->variables, name);
        if(v) return v;

        if(frame->type == YFT_FUNC)
            break;
    }

    // check global vars
    v = yapArrayFindVariableByName(&vm->globals, name);
    if(v) return v;

    return NULL;
}

yapFrame *yapVMPushFrame(yapVM *vm, yapBlock *block, int argCount, yU32 frameType)
{
    yapFrame *frame;
    int i;

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

    frame = yapFrameCreate(frameType, block, vm->stack.count);
    yapArrayPush(&vm->frames, frame);

    return frame;
}

static yBool yapVMCallCFunction(yapVM *vm, yapCFunction func, yU32 argCount);
static yBool yapVMCreateObject(yapVM *vm, yapFrame **framePtr, yapValue *isa, int argCount);

static yBool yapVMCall(yapVM *vm, yapFrame **framePtr, yapValue *callable, int argCount)
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
        if(!yapVMCallCFunction(vm, *callable->cFuncVal, argCount))
            return yFalse;
    }
    else if(callable->type == YVT_OBJECT)
    {
        return yapVMCreateObject(vm, framePtr, callable, argCount);
    }
    else
    {
        *framePtr = yapVMPushFrame(vm, callable->blockVal, argCount, YFT_FUNC);
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

yBool yapVMCallFuncByName(yapVM *vm, const char *name, int argCount)
{
    yapFrame *frame = NULL;
    yapVariable *func = yapVMResolveVariable(vm, name);
    if(!func)
        return yFalse;
    if(yapVMCall(vm, &frame, func->value, argCount))
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
    yapValue *newObject = yapValueObjectCreate(vm, isa);

    if(initFunc)
    {
        yapArrayInject(&vm->stack, newObject, argCount);
        return yapVMCall(vm, framePtr, initFunc, argCount + 1);
    }

    // TODO: initFunc needs to be the noop call if its not there

    // No init, just throw out any arguments and push the new object
    yapVMPopValues(vm, argCount);
    yapArrayPush(&vm->stack, newObject);
    return ret;
}

// TODO: this needs to protect against variable masking/shadowing
static void yapVMRegisterVariable(yapVM *vm, yapVariable *variable)
{
    yapFrame *frame = yapArrayTop(&vm->frames);
    if(!frame)
        return;

    if(frame->block == frame->block->chunk->block)
    {
        // If we're in the chunk's "main" function, all variable
        // registration goes into the globals
        yapArrayPush(&vm->globals, variable);
    }
    else
    {
        yapArrayPush(&frame->variables, variable);
    }
}

static void yapVMPushRef(yapVM *vm, yapVariable *variable)
{
    yapValue *value = yapValueSetRef(vm, yapValueAcquire(vm), &variable->value);
    yapArrayPush(&vm->stack, value);
}

// TODO: merge this function with PushFrame and _RET
static yBool yapVMCallCFunction(yapVM *vm, yapCFunction func, yU32 argCount)
{
    int retCount;
    yapFrame *frame = yapFrameCreate(YFT_FUNC, NULL, vm->stack.count);
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

struct yapFrame *yapVMPopFrames(yapVM *vm, yU32 frameTypeToFind, yBool keepIt)
{
    yapFrame *frame = yapArrayTop(&vm->frames);

    if(frameTypeToFind != YFT_ANY)
    {
        while(frame && !(frame->type & frameTypeToFind))
        {
            yapFrameDestroy(frame);
            yapArrayPop(&vm->frames);
            frame = yapArrayTop(&vm->frames);
        };
    }

    if(frame && !keepIt)
    {
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
        printf(" -> %d\n", vm->stack.count);
        yapOpsDump(frame->ip, 1);
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
        case YOP_PUSHI:
        {
            yapValue *value = yapValueSetInt(vm, yapValueAcquire(vm), operand);
            yapArrayPush(&vm->stack, value);
        }
        break;

        case YOP_PUSHLBLOCK:
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
            yapVariable *variable = yapVariableCreate(vm, frame->block->chunk->kStrings.data[operand]);
            yapVMRegisterVariable(vm, variable);
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

        case YOP_INDEX:
        {
            yapValue *index = yapArrayPop(&vm->stack);
            yapValue *value = yapArrayPop(&vm->stack);
            if(value && index)
            {
                int lvalue = operand;
                yapValue *ret = yapValueIndex(vm, value, index, lvalue);
                if(ret)
                {
                    yapArrayPush(&vm->stack, ret);
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
            yapValue *callable;
            callable = yapArrayPop(&vm->stack);
            continueLooping = yapVMCall(vm, &frame, callable, argCount);
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
                frame = yapVMPushFrame(vm, block, 0, YFT_COND);
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
                frame = yapVMPushFrame(vm, blockRef->blockVal, 0, frameType);
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
            if(operand)
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
            else
            {
                // a C-style break. Find the innermost loop and kill it.
                frame = yapVMPopFrames(vm, YFT_LOOP, yFalse);
                continueLooping = (frame) ? yTrue : yFalse;
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
                yapArrayInject(&vm->stack, val, 1);
                continueLooping = yapVMCall(vm, &frame, getFunc, 2 /*obj, index*/);
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
                yapArrayPush(&vm->stack, val);
                continueLooping = yapVMCall(vm, &frame, countFunc, 1);
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

    for(i = 0; i < vm->globals.count; i++)
    {
        yapVariable *variable = (yapVariable *)vm->globals.data[i];
        yapVariableMark(variable);
        yapValueMark(vm, variable->value);
    }

    for(i = 0; i < vm->frames.count; i++)
    {
        yapFrame *frame = (yapFrame *)vm->frames.data[i];
        for(j = 0; j < frame->variables.count; j++)
        {
            yapVariable *variable = (yapVariable *)frame->variables.data[j];
            yapVariableMark(variable);
            yapValueMark(vm, variable->value);
        }
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
