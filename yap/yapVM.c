#include "yapVM.h"

#include "yapBlock.h"
#include "yapCompiler.h"
#include "yapFrame.h"
#include "yapModule.h"
#include "yapOp.h"
#include "yapValue.h"
#include "yapVariable.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_ERROR_LENGTH 1023

void yapVMRegisterIntrinsic(yapVM *vm, const char *name, yapCFunction func)
{
    yapVariable *intrinsic = yapVariableCreate(vm, name);
    intrinsic->value = yapValueCreate(vm);
    yapValueSetCFunction(intrinsic->value, func);
    yapArrayPush(&vm->globals, intrinsic);
}

yapModule * yapVMLoadModule(yapVM *vm, const char *name, const char *text)
{
    yapModule *module;
    yapVariable *moduleRef;

    yapCompiler *compiler = yapCompilerCreate();
    yapCompile(compiler, text);
    module = compiler->module;
    compiler->module = NULL;
    yapCompilerDestroy(compiler);

    if(module->block)
    {
        // Alloc the global variable "main"
        moduleRef = yapVariableCreate(vm, name);
        moduleRef->value = yapValueCreate(vm);
        moduleRef->value->type = YVT_MODULE;
        moduleRef->value->moduleVal = module;
        yapArrayPush(&vm->globals, moduleRef);

        yapArrayPush(&vm->modules, module);

        // Execute the module's block
        yapVMPushFrame(vm, module->block, 0, YFT_FUNC);
        yapVMLoop(vm);
        yapVMGC(vm);
    }
    else
    {
        yapModuleDestroy(module);
        module = NULL;
    }

    return module;
}

yapVM * yapVMCreate(void)
{
    yapVM *vm = yapAlloc(sizeof(yapVM));
    return vm;
}

void yapVMSetError(yapVM *vm, const char *errorFormat, ...)
{
    va_list args;
    char tempStr[MAX_ERROR_LENGTH+1];

    yapVMClearError(vm);
    va_start(args, errorFormat);
    vsprintf(tempStr, errorFormat, args);
    va_end(args);

    vm->error = yapStrdup(tempStr);
}

void yapVMClearError(yapVM *vm)
{
    if(vm->error)
    {
        yapFree(vm->error);
        vm->error = NULL;
    }
}

void yapVMDestroy(yapVM *vm)
{
    yapArrayClear(&vm->globals, NULL);
    yapArrayClear(&vm->frames, (yapDestroyCB)yapFrameDestroy);
    yapArrayClear(&vm->stack, NULL);
    yapArrayClear(&vm->modules, (yapDestroyCB)yapModuleDestroy);

    yapArrayClear(&vm->usedVariables, (yapDestroyCB)yapVariableDestroy);
    yapArrayClear(&vm->usedValues, (yapDestroyCB)yapValueDestroy);

    yapVMClearError(vm);

    yapFree(vm);
}

static yapVariable * yapArrayFindVariableByName(yapArray *a, const char *name)
{
    int i;
    for(i=0; i<a->count; i++)
    {
        yapVariable *v = (yapVariable *)a->data[i];
        if(!strcmp(v->name, name))
        {
            return v;
        }
    }
    return NULL;
}

static yapVariable * yapVMResolveVariable(yapVM *vm, const char *name)
{
    int i;
    yapModule *module;
    yapFrame *frame;
    yapVariable *v;

    for(i=vm->frames.count-1; i>=0; i--)
    {
        frame = (yapFrame*)vm->frames.data[i];

        // Check the local variables
        v = yapArrayFindVariableByName(&frame->variables, name);
        if(v) return v;

        // Next: Check the variables in the module this block is from
        module = frame->block->module;
        if(module)
        {
            v = yapArrayFindVariableByName(&module->variables, name);
            if(v) return v;
        }

        if(frame->type == YFT_FUNC)
            break;
    }

    // Then check global vars as a last ditch effort
    v = yapArrayFindVariableByName(&vm->globals, name);
    if(v) return v;

    return NULL;
}

yapFrame * yapVMPushFrame(yapVM *vm, yapBlock *block, int argCount, yU32 frameType)
{
    yapFrame *frame;
    int i;

    // accomodate the function's arglist by padding/removing stack entries
    if(argCount > block->argCount)
    {
        // Too many arguments passed to this function. Pop some!
        int i;
        for(i=0; i<(argCount - block->argCount); i++)
            yapArrayPop(&vm->stack);
    }
    else if(block->argCount > argCount)
    {
        // Too few arguments -- pad with nulls
        int i;
        for(i=0; i<(block->argCount - argCount); i++)
            yapArrayPush(&vm->stack, yapValueClone(vm, &yapValueNull));
    }

    frame = yapFrameCreate(frameType, block, vm->stack.count - argCount);
    yapArrayPush(&vm->frames, frame);

    return frame;
}

static void yapVMPushRef(yapVM *vm, yapVariable *variable)
{
    yapValue *value = yapValueCreate(vm);
    value->type = YVT_REF;
    value->refVal = variable;
    yapArrayPush(&vm->stack, value);
}

yBool yapVMSetVar(yapVM *vm, yapValue *ref, yapValue *val)
{
    if(!val)
    {
        yapVMSetError(vm, "YOP_SETVAR: empty stack!");
        return yFalse;
    }
    if(!ref)
    {
        yapVMSetError(vm, "YOP_SETVAR: empty stack!");
        return yFalse;
    }
    if(ref->type != YVT_REF)
    {
        yapVMSetError(vm, "YOP_SETVAR: value on top of stack, ref underneath");
        return yFalse;
    }
    ref->refVal->value = yapValueClone(vm, val);

    if(val->type == YVT_STRING)
        yapTrace(("-- set '%s' to '%s'\n", ref->refVal->name, val->stringVal));
    else if(val->type == YVT_NULL)
        yapTrace(("-- set '%s' to null\n", ref->refVal->name));
    return yTrue;
}

// TODO: merge this function with PushFrame and _RET
yBool yapVMCallCFunction(yapVM *vm, yapCFunction func, yU32 argCount)
{
    int retCount;
    yapFrame *frame = yapFrameCreate(YFT_FUNC, NULL, vm->stack.count - argCount);
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

yapValue * yapVMPopValue(yapVM *vm)
{
    return yapArrayPop(&vm->stack);
}

struct yapFrame * yapVMPopFrames(yapVM *vm, yU32 frameTypeToFind, yBool keepIt)
{
    yapFrame *frame = yapArrayTop(&vm->frames);

    if(frameTypeToFind != YFT_ANY)
    {
        while(frame && frame->type != frameTypeToFind)
        {
            yapFrameDestroy(frame);
            frame = yapArrayPop(&vm->frames);
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

void yapVMLoop(yapVM *vm)
{
    yapFrame *frame = yapArrayTop(&vm->frames);
    yBool continueLooping = yTrue;
    yBool newFrame;
    yU16 opcode;
    yU16 operand;

    if(!frame)
    {
        yapVMSetError(vm, "yapVMLoop(): No stack frame!");
        return;
    }

    // Main VM loop!
    while(continueLooping && !vm->error)
    {
        newFrame = yFalse;

        // These are put into temporary variables for future ntohs() cross-platform safety
        opcode  = frame->ip->opcode;
        operand = frame->ip->operand;

        switch(opcode)
        {
        case YOP_NOOP:
            break;

        case YOP_START:
            break;

        case YOP_SKIP:
            {
                int i;
                for(i=0; i<operand; i++)
                    frame->ip++;
            }
            break;

        case YOP_PUSHNULL:
            {
                yapArrayPush(&vm->stack, yapValueClone(vm, &yapValueNull));
            }
            break;

        case YOP_PUSHLBLOCK:
            {
                yapValue *value = yapValueCreate(vm);
                yapValueSetFunction(value, frame->block->module->blocks.data[operand]);
                yapArrayPush(&vm->stack, value);
            }
            break;

        case YOP_PUSH_KI:
            {
                yapValue *value = yapValueCreate(vm);
                yapValueSetInt(value, frame->block->module->kInts.data[operand]);
                yapArrayPush(&vm->stack, value);
            }
            break;

        case YOP_ADD_KI:
            {
                yapValue *value = yapArrayTop(&vm->stack);
                if(!yapValueConvertToInt(vm, value))
                    break;
                yapValueSetInt(value, value->intVal + frame->block->module->kInts.data[operand]);
            }
            break;

        case YOP_SUB_KI:
            {
                yapValue *value = yapArrayTop(&vm->stack);
                if(!yapValueConvertToInt(vm, value))
                    break;
                yapValueSetInt(value, value->intVal - frame->block->module->kInts.data[operand]);
            }
            break;

        case YOP_PUSH_KS:
            {
                yapValue *value = yapValueCreate(vm);
                yapValueSetKString(value, frame->block->module->kStrings.data[operand]);
                yapArrayPush(&vm->stack, value);
            }
            break;

        case YOP_VARREG_KS:
            {
                yapVariable *variable = yapVariableCreate(vm, frame->block->module->kStrings.data[operand]);
                yapArrayPush(&frame->variables, variable);
                yapVMPushRef(vm, variable);
            }
            break;

        case YOP_VARREF_KS:
            {
                yapVariable *variable = yapVMResolveVariable(vm, frame->block->module->kStrings.data[operand]);
                if(variable)
                {
                    yapVMPushRef(vm, variable);
                }
                else
                {
                    yapVMSetError(vm, "YOP_GETVAR_KS: no variable named '%s'", frame->block->module->kStrings.data[operand]);
                    continueLooping = yFalse;
                }
            }
            break;

        case YOP_REFVAL:
            {
                yapValue *value = yapArrayTop(&vm->stack);
                if(!value)
                {
                    yapVMSetError(vm, "YOP_REFVAL_KS: empty stack!");
                    continueLooping = yFalse;
                    break;
                };
                if(value->type != YVT_REF)
                {
                    yapVMSetError(vm, "YOP_REFVAL_KS: requires ref on top of stack");
                    continueLooping = yFalse;
                    break;
                }
                yapValueCloneData(vm, value, value->refVal->value);
            }
            break;

        case YOP_SETVAR:
            {
                yapValue *val = yapArrayPop(&vm->stack);
                yapValue *ref = yapArrayPop(&vm->stack);
                continueLooping = yapVMSetVar(vm, ref, val);
            }
            break;

        case YOP_SETARG:
            {
                yapValue *ref = yapArrayPop(&vm->stack);
                yapValue *val = yapArrayPop(&vm->stack);
                continueLooping = yapVMSetVar(vm, ref, val);
            }
            break;

        case YOP_POP:
            {
                int i;
                for(i=0; i<operand; i++)
                    yapArrayPop(&vm->stack);
            }
            break;

        case YOP_CALL:
            {
                yapValue *callable = yapArrayPop(&vm->stack);
                if(!callable)
                {
                    yapVMSetError(vm, "YOP_CALL: empty stack!");
                    continueLooping = yFalse;
                    break;
                }
                if(callable->type != YVT_REF)
                {
                    yapVMSetError(vm, "YOP_CALL: top of stack not a reference");
                    continueLooping = yFalse;
                    break;
                }
                if(!yapValueIsCallable(callable->refVal->value))
                {
                    yapVMSetError(vm, "YOP_CALL: variable not callable");
                    continueLooping = yFalse;
                    break;
                }
                if(callable->refVal->value->type == YVT_CFUNCTION)
                {
                    continueLooping = yapVMCallCFunction(vm, *callable->refVal->value->cFuncVal, operand);
                }
                else
                {
                    yapBlock *block = (callable->refVal->value->type == YVT_MODULE)
                                    ? callable->refVal->value->moduleVal->block 
                                    : callable->refVal->value->blockVal;

                    frame = yapVMPushFrame(vm, block, operand, YFT_FUNC);
                    if(frame)
                        newFrame = yTrue;
                    else
                        continueLooping = yFalse;
                }
            }
            break;

        case YOP_RET:
            {
                frame = yapVMPopFrames(vm, YFT_FUNC, yFalse);
                if(frame)
                {
                    // Stash lastRet for any YOP_KEEPs in the pipeline
                    vm->lastRet = operand;
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
                    for(i=0; i<(offerCount - keepCount); i++)
                    {
                        yapTrace(("-- cleaning stack entry --\n"));
                        yapArrayPop(&vm->stack);
                    }
                }
                else if(keepCount > offerCount)
                {
                    int i;
                    for(i=0; i<(keepCount - offerCount ); i++)
                    {
                        yapTrace(("-- padding stack with null --\n"));
                        yapArrayPush(&vm->stack, yapValueClone(vm, &yapValueNull));
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
                if(yapValueAsBool(cond))
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
                    frame = yapVMPushFrame(vm, blockRef->blockVal, 0, YFT_LOOP);
                    if(frame)
                        newFrame = yTrue;
                    else
                        continueLooping = yFalse;
                }
                else
                {
                    yapVMSetError(vm, "hurr");
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
                    performLeave   = !yapValueAsBool(cond); // don't leave if expr is true!
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

        default:
            yapVMSetError(vm, "Unknown VM Opcode: %d", opcode);
            continueLooping = yFalse;
            break;
        }

        if(continueLooping && !newFrame)
            frame->ip++;
    }

    yapVMGC(vm);
}

void yapVMGC(struct yapVM *vm)
{
    int i, j;

    // -----------------------------------------------------------------------
    // walk all used values and clear their GC flags

    for(i=0; i<vm->usedValues.count; i++)
    {
        yapValue *value = (yapValue *)vm->usedValues.data[i];
        value->used   = yFalse;
        value->shared = yFalse;
    }

    for(i=0; i<vm->usedVariables.count; i++)
    {
        yapVariable *variable = (yapVariable *)vm->usedVariables.data[i];
        variable->used = yFalse;
    }

    // -----------------------------------------------------------------------
    // mark all values used by things the VM still cares about

    for(i=0; i<vm->globals.count; i++)
    {
        yapVariable *variable = (yapVariable *)vm->globals.data[i];
        yapVariableMark(variable);
        yapValueMark(variable->value);
    }

    for(i=0; i<vm->frames.count; i++)
    {
        yapFrame *frame = (yapFrame *)vm->frames.data[i];
        for(j=0; j<frame->variables.count; j++)
        {
            yapVariable *variable = (yapVariable *)frame->variables.data[j];
            yapVariableMark(variable);
            yapValueMark(variable->value);
        }
    }

    for(i=0; i<vm->stack.count; i++)
    {
        yapValue *value = (yapValue *)vm->stack.data[i];
        yapValueMark(value);
    }

    for(i=0; i<vm->modules.count; i++)
    {
        yapModule *module = (yapModule *)vm->modules.data[i];
        for(j=0; j<module->variables.count; j++)
        {
            yapVariable *variable = (yapVariable *)module->variables.data[j];
            yapVariableMark(variable);
            yapValueMark(variable->value);
        }
    }

    // -----------------------------------------------------------------------
    // sweep!

    for(i=0; i<vm->usedValues.count; i++)
    {
        yapValue *value = (yapValue *)vm->usedValues.data[i];
        if(value->used)
        {
            if(value->type == YVT_REF)
                yapVariableMark(value->refVal);
        }
        else
        {
            yapValueDestroy(value);
            vm->usedValues.data[i] = NULL; // for future squashing
        }
    }
    yapArraySquash(&vm->usedValues);

    for(i=0; i<vm->usedVariables.count; i++)
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
