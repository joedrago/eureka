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

yapModule * yapVMLoadModule(yapVM *vm, const char *name, const char *text)
{
    int x;
    yapModule *main;
    yapVariable *moduleRef;
    yapVariable *callVariable;
    yapBlock *block;

    // Someday this'll actually compile stuff!

    // Alloc main
    main = yapModuleCreate();
    yapArrayPush(&vm->modules, main);

    // compile main's block
    block = yapBlockCreate();
    // note: block->module is set in the next stanza
    block->ops = yapOpsAlloc(30);

    // call(55)
    x = 0;
    block->ops[x  ].opcode  = YOP_PUSH_KI;
    block->ops[x++].operand = yap32ArrayPushUnique(&main->kInts, 55);
    block->ops[x  ].opcode  = YOP_VARREF_KS;
    block->ops[x++].operand = yapArrayPushUniqueString(&main->kStrings, "call");
    block->ops[x  ].opcode  = YOP_CALL;
    block->ops[x++].operand = 1;

    block->ops[x  ].opcode  = YOP_VARREG_KS;
    block->ops[x++].operand = yapArrayPushUniqueString(&main->kStrings, "foo");
    block->ops[x  ].opcode  = YOP_POP;
    block->ops[x++].operand = 0;
    block->ops[x  ].opcode  = YOP_VARREF_KS;
    block->ops[x++].operand = yapArrayPushUniqueString(&main->kStrings, "foo");
    block->ops[x  ].opcode  = YOP_PUSH_KI;
    block->ops[x++].operand = yap32ArrayPushUnique(&main->kInts, 2);
    block->ops[x  ].opcode  = YOP_SETVAR;
    block->ops[x++].operand = 0;
    block->ops[x  ].opcode  = YOP_VARREF_KS;
    block->ops[x++].operand = yapArrayPushUniqueString(&main->kStrings, "foo");
    block->ops[x  ].opcode  = YOP_REFVAL;
    block->ops[x++].operand = 0;

    block->ops[x  ].opcode  = YOP_RET;
    block->ops[x++].operand = 1;
    yapArrayPush(&vm->blocks, block);

    // populate main module's data
    block->module = main;
    main->block = block;

    // Add a function: int call(int a) -- returns a+2
    block = yapBlockCreate();
    block->module = main;
    block->ops = yapOpsAlloc(4);

    // return arg0 + 2;
    x = 0;
    block->ops[x  ].opcode  = YOP_PUSHARGN;
    block->ops[x++].operand = 0;
    block->ops[x  ].opcode  = YOP_ADD_KI;
    block->ops[x++].operand = yap32ArrayPushUnique(&main->kInts, 2);
    block->ops[x  ].opcode  = YOP_RET;
    block->ops[x++].operand = 1;

    yapArrayPush(&vm->blocks, block);

    // Alloc main.call()
    callVariable = yapVariableCreate(vm, "call");
    callVariable->value = yapValueCreate(vm);
    callVariable->value->type = YVT_FUNCTION;
    callVariable->value->blockVal = block;
    yapArrayPush(&main->variables, callVariable);

    // Alloc the global variable "main"
    moduleRef = yapVariableCreate(vm, name);
    moduleRef->value = yapValueCreate(vm);
    moduleRef->value->type = YVT_MODULE;
    moduleRef->value->moduleVal = main;
    yapArrayPush(&vm->globals, moduleRef);

    // Execute the module's block
    yapVMPushFrame(vm, moduleRef, 0);
    yapVMLoop(vm);
    yapVMGC(vm);

    return main;
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

    yapArrayClear(&vm->blocks, (yapDestroyCB)yapBlockDestroy);

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
    int i, j;
    yapModule *module;
    yapFrame *frame;
    yapVariable *v;

    frame = yapArrayTop(&vm->frames);
    if(frame)
    {
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
    }

    // Then check global vars as a last ditch effort
    v = yapArrayFindVariableByName(&vm->globals, name);
    if(v) return v;

    return NULL;
}

yapFrame * yapVMPushFrame(yapVM *vm, yapVariable *ref, int numArgs)
{
    yapFrame *frame;
    yapBlock *block;
    int i;

    block = (ref->value->type == YVT_MODULE) ? ref->value->moduleVal->block : ref->value->blockVal;

    printf("Function Call: %s(", ref->name);

    frame = yapFrameCreate();
    frame->block = block;
    frame->ip = block->ops;
    frame->bp = vm->stack.count - numArgs;
    for(i=0; i<numArgs; i++)
    {
        yapValue *value = yapArrayPop(&vm->stack);
        yapArrayPush(&frame->args, value);

        if(i) printf(", ");
        switch(value->type)
        {
        case YVT_INT:    printf("%d", value->intVal); break;
        case YVT_STRING: printf("%s", value->stringVal); break;
        };
    }
    yapArrayPush(&vm->frames, frame);

    printf(")\n");
    return frame;
}

static void yapVMPushRef(yapVM *vm, yapVariable *variable)
{
    yapValue *value = yapValueCreate(vm);
    value->type = YVT_REF;
    value->refVal = variable;
    yapArrayPush(&vm->stack, value);
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
    while(continueLooping && !vm->error)
    {
        calledFunc = yFalse;

        // These are put into temporary variables for future ntohs() cross-platform safety
        opcode  = frame->ip->opcode;
        operand = frame->ip->operand;

        switch(opcode)
        {
        case YOP_NOOP:
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
                if(!val)
                {
                    yapVMSetError(vm, "YOP_SETVAR: empty stack!");
                    continueLooping = yFalse;
                    break;
                }
                if(!ref)
                {
                    yapVMSetError(vm, "YOP_SETVAR: empty stack!");
                    continueLooping = yFalse;
                    break;
                }
                if(ref->type != YVT_REF)
                {
                    yapVMSetError(vm, "YOP_SETVAR: value on stop of stack, ref underneath");
                    continueLooping = yFalse;
                    break;
                }
                ref->refVal->value = yapValueClone(vm, val);
            }
            break;

        case YOP_POP:
            yapArrayPop(&vm->stack);
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
                frame = yapVMPushFrame(vm, callable->refVal, operand);
                if(frame)
                    calledFunc = yTrue;
                else
                    continueLooping = yFalse;
            }
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
                    printf(" * Ret: (");

                    // Stash return values in frame
                    for(i=0; i<operand;i++)
                    {
                        int stackIndex = (vm->stack.count - operand) + i;
                        // Reuse struct instead of copy -- it is about to be popped                                        
                        yapArrayPush(&frame->ret, vm->stack.data[stackIndex]);

                        if(i) printf(", ");
                        {
                            yapValue *value = (yapValue *)vm->stack.data[stackIndex];
                            switch(value->type)
                            {
                            case YVT_INT:    printf("%d", value->intVal); break;
                            case YVT_STRING: printf("%s", value->stringVal); break;
                            default:         printf("??"); break;
                            };
                        }

                        vm->stack.data[stackIndex] = NULL;
                    }

                    printf(")\n");
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
