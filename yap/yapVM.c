#include "yapVM.h"

#include "yapBlock.h"
#include "yapFrame.h"
#include "yapModule.h"
#include "yapOp.h"
#include "yapValue.h"
#include "yapVariable.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_ERROR_LENGTH 1023

int yapVMCompile(yapVM *vm, const char *text)
{
    yapModule *main;
    yapVariable *v;
    yapBlock *block;

    // Someday this'll actually compile stuff!

    // compile main's block
    block = yapBlockCreate();
    // note: block->module is set in the next stanza
    block->ops = yapOpsAlloc(7);

    // call(55)
    block->ops[0].opcode  = YOP_PUSHKI;
    block->ops[0].operand = yap32ArrayPushUnique(&vm->kInts, 55);
    block->ops[1].opcode  = YOP_PUSHKS;
    block->ops[1].operand = yapArrayPushUniqueString(&vm->kStrings, "call");
    block->ops[2].opcode  = YOP_CALL;
    block->ops[2].operand = 1;

    // this kinda does nothing other than illustrate that bp is cleaning the stack well
    block->ops[3].opcode  = YOP_PUSHKI;
    block->ops[3].operand = yap32ArrayPushUnique(&vm->kInts, 2);
    block->ops[4].opcode  = YOP_PUSHKI;
    block->ops[4].operand = yap32ArrayPushUnique(&vm->kInts, 2);
    block->ops[5].opcode  = YOP_PUSHKI;
    block->ops[5].operand = yap32ArrayPushUnique(&vm->kInts, 2);

    block->ops[6].opcode  = YOP_RET;
    block->ops[6].operand = 1;
    yapArrayPush(&vm->blocks, block);

    // Alloc main
    main = yapModuleCreate();
    block->module = main;
    main->block = block;
    yapArrayPush(&vm->modules, main);

    // Alloc the global variable "main"
    v = yapVariableCreate("main");
    v->value = yapValueCreate(vm);
    v->value->type = YVT_MODULE;
    v->value->moduleVal = main;
    yapArrayPush(&vm->globals, v);

    // Add a function: int call(int a) -- returns a+2
    block = yapBlockCreate();
    block->module = main;
    block->ops = yapOpsAlloc(4);

    // return arg0 + 2;
    block->ops[0].opcode  = YOP_PUSHARGN;
    block->ops[0].operand = 0;
    block->ops[1].opcode  = YOP_ADDKI;
    block->ops[1].operand = yap32ArrayPushUnique(&vm->kInts, 2);
    block->ops[2].opcode  = YOP_RET;
    block->ops[2].operand = 1;

    yapArrayPush(&vm->blocks, block);

    // Alloc main.call()
    v = yapVariableCreate("call");
    v->value = yapValueCreate(vm);
    v->value->type = YVT_FUNCTION;
    v->value->blockVal = block;
    yapArrayPush(&main->variables, v);

    return 2;
}

yapVM * yapVMCreate(void)
{
    yapVM *vm = yapAlloc(sizeof(yapVM));
    yapVMCompile(vm, "wooo");
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
    yapArrayClear(&vm->globals, (yapDestroyCB)yapVariableDestroy);
    yapArrayClear(&vm->frames, (yapDestroyCB)yapFrameDestroy);
    yapArrayClear(&vm->stack, NULL);
    yapArrayClear(&vm->modules, (yapDestroyCB)yapModuleDestroy);

    yapArrayClear(&vm->blocks, (yapDestroyCB)yapBlockDestroy);

    yapArrayClear(&vm->usedValues, (yapDestroyCB)yapValueDestroy);
    yapArrayClear(&vm->freeValues, (yapDestroyCB)yapValueDestroy);

    yapArrayClear(&vm->kStrings, (yapDestroyCB)yapFree);
    yap32ArrayClear(&vm->kInts);

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
    int i, j;
    yapModule *module;
    yapFrame *frame;
    yapVariable *v;

    // Check locals as we walk up the stack
    for(i=vm->frames.count - 1; i>=0; i--)
    {
        frame = (yapFrame *)vm->frames.data[i];
        v = yapArrayFindVariableByName(&frame->variables, name);
        if(v) return v;
    }

    // Next: Check the variables in the module this block is from
    frame = yapArrayTop(&vm->frames);
    if(frame)
    {
        module = frame->block->module;
        if(module)
        {
            v = yapArrayFindVariableByName(&module->variables, name);
            if(v) return v;
        }
    }

    // Then check global vars as a last ditch effort
    v = yapArrayFindVariableByName(&vm->globals, name);
    if(v) return v;

    return NULL;
}

yapFrame * yapVMPushFrame(yapVM *vm, const char *name, int numArgs)
{
    yapFrame *frame;
    yapBlock *block;
    yapVariable *v;
    int i;

    v = yapVMResolveVariable(vm, name);
    if(!v)
    {
        yapVMSetError(vm, "Cannot find callable '%s'", name);
        return NULL;
    }

    if(!yapValueIsCallable(v->value))
    {
        yapVMSetError(vm, "Variable '%s' is not callable", name);
        return NULL;
    }

    block = (v->value->type == YVT_MODULE) ? v->value->moduleVal->block : v->value->blockVal;

    frame = yapFrameCreate();
    frame->block = block;
    frame->ip = block->ops;
    frame->bp = vm->stack.count - numArgs;
    for(i=0; i<numArgs; i++)
    {
        yapValue *value = yapArrayPop(&vm->stack);
        yapArrayPush(&frame->args, value);
    }
    yapArrayPush(&vm->frames, frame);
    return frame;
}

void yapFrameDestroy(yapFrame *frame)
{
    yapArrayClear(&frame->ret,  NULL);
    yapArrayClear(&frame->args, NULL);
    yapFree(frame);
}

#define YOP_UNIMPLEMENTED(OP) case OP: printf("NYI: " #OP "\n"); break

void yapVMLoop(yapVM *vm)
{
    yapFrame *frame = yapArrayTop(&vm->frames);
    yBool continueLooping = yTrue;
    yBool calledFunc;
    yU16 opcode;
    yU16 operand;

    if(!frame)
    {
        yapVMSetError(vm, "yapVMLoop(): No stack frame!");
        return;
    }

    // Main VM loop!
    while(continueLooping)
    {
        calledFunc = yFalse;

        // These are put into temporary variables for future ntohs() cross-platform safety
        opcode  = frame->ip->opcode;
        operand = frame->ip->operand;

        switch(opcode)
        {
        case YOP_NOOP:
            break;

        case YOP_PUSHKI:
            {
                yapValue *value = yapValueCreate(vm);
                yapValueSetInt(value, vm->kInts.data[operand]);
                yapArrayPush(&vm->stack, value);
            }
            break;

        case YOP_ADDKI:
            {
                yapValue *value = yapArrayTop(&vm->stack);
                if(!yapValueConvertToInt(vm, value))
                    break;
                yapValueSetInt(value, value->intVal + vm->kInts.data[operand]);
            }
            break;

        case YOP_SUBKI:
            {
                yapValue *value = yapArrayTop(&vm->stack);
                if(!yapValueConvertToInt(vm, value))
                    break;
                yapValueSetInt(value, value->intVal - vm->kInts.data[operand]);
            }
            break;

        case YOP_PUSHKS:
            {
                yapValue *value = yapValueCreate(vm);
                yapValueSetKString(value, vm->kStrings.data[operand]);
                yapArrayPush(&vm->stack, value);
            }
            break;

        case YOP_PUSHARGN:
            {
                if(operand >= frame->args.count)
                {
                    yapVMSetError(vm, "YOP_PUSHARGN: requesting arg %d when %d is max index", operand, frame->args.count-1);
                    continueLooping = yFalse;
                }
                else
                {
                    yapValue *value = yapValueClone(vm, (yapValue *)frame->args.data[operand]);
                    yapArrayPush(&vm->stack, value);
                }
            }
            break;

        case YOP_CALL:
            {
                yapValue *callName = (yapValue *)yapArrayPop(&vm->stack);
                if(callName && callName->type == YVT_STRING)
                    frame = yapVMPushFrame(vm, callName->stringVal, operand);
                if(frame)
                    calledFunc = yTrue;
                else
                    continueLooping = yFalse;
            };
            break;

        case YOP_RET:
            {
                int i;
                int prevBP = frame->bp; // Stash for future value stack popping

                yapArrayPop(&vm->frames); // Removes 'frame' from top of stack
                yapFrameDestroy(frame);
                frame = yapArrayTop(&vm->frames);
                if(frame)
                {
                    // Stash return values in frame
                    for(i=0; i<operand;i++)
                    {
                        int stackIndex = (vm->stack.count - operand) + i;
                        // Reuse struct instead of copy -- it is about to be popped                                        
                        yapArrayPush(&frame->ret, vm->stack.data[stackIndex]);
                        vm->stack.data[stackIndex] = NULL;
                    }
                }
                else
                {
                    // Throw away return values and bail out of loop
                    continueLooping = yFalse;
                }

                // Clear out value stack of argument cruft
                while(vm->stack.count > prevBP)
                    yapArrayPop(&vm->stack);
            };
            break;

        default:
            yapVMSetError(vm, "Unknown VM Opcode: %d", opcode);
            continueLooping = yFalse;
            break;
        }

        if(continueLooping && !calledFunc)
            frame->ip++;
    }

    yapVMGC(vm);
}

void yapVMCall(yapVM *vm, const char *name, int numArgs)
{
    yapFrame *frame = yapVMPushFrame(vm, name, numArgs);
    if(!frame)
        return;

    yapVMLoop(vm);
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

    // -----------------------------------------------------------------------
    // mark all values used by things the VM still cares about

    for(i=0; i<vm->globals.count; i++)
    {
        yapVariable *variable = (yapVariable *)vm->globals.data[i];
        yapValueMark(variable->value);
    }

    for(i=0; i<vm->frames.count; i++)
    {
        yapFrame *frame = (yapFrame *)vm->frames.data[i];
        for(j=0; j<frame->variables.count; j++)
        {
            yapVariable *variable = (yapVariable *)frame->variables.data[j];
            yapValueMark(variable->value);
        }
    }

    for(i=0; i<vm->stack.count; i++)
    {
        yapVariable *variable = (yapVariable *)vm->stack.data[i];
        yapValueMark(variable->value);
    }

    for(i=0; i<vm->modules.count; i++)
    {
        yapModule *module = (yapModule *)vm->modules.data[i];
        for(j=0; j<module->variables.count; j++)
        {
            yapVariable *variable = (yapVariable *)module->variables.data[j];
            yapValueMark(variable->value);
        }
    }

    // -----------------------------------------------------------------------
    // sweep!

    for(i=0; i<vm->usedValues.count; i++)
    {
        yapValue *value = (yapValue *)vm->usedValues.data[i];
        if(!value->used)
        {
            yapArrayPush(&vm->freeValues, value);
            vm->usedValues.data[i] = NULL; // for future squashing
        }
    }
    yapArraySquash(&vm->usedValues);
}
