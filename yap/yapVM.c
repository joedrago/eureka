#include "yapVM.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_ERROR_LENGTH 1023

yapUnit * yapUnitCompile(const char *text)
{
    yapUnit *unit = yapAlloc(sizeof(*unit));

    // Someday this'll actually compile stuff!

    {
        unit->code = yapAlloc(sizeof(*unit->code) * 8);
        unit->code[0].opcode  = YOP_PUSHKI;
        unit->code[0].operand = 2;
        unit->code[1].opcode  = YOP_ADDKI;
        unit->code[1].operand = 2;
        unit->code[2].opcode  = YOP_RET;
        unit->code[2].operand = 1;

        unit->code[3].opcode  = YOP_CALL;
        unit->code[4].opcode  = YOP_PUSHKI;
        unit->code[4].operand = 2;
        unit->code[5].opcode  = YOP_PUSHKI;
        unit->code[5].operand = 2;
        unit->code[6].opcode  = YOP_PUSHKI;
        unit->code[6].operand = 2;
        unit->code[7].opcode  = YOP_RET;
        unit->code[7].operand = 1;
    }

    {
        yapFunction *func = yapAlloc(sizeof(*func));
        func->name = yapStrdup("call");
        func->unit = unit;
        func->pc   = 0;
        yapArrayPush(&unit->funcs, func);
    }

    {
        yapFunction *func = yapAlloc(sizeof(*func));
        func->name = yapStrdup("main");
        func->unit = unit;
        func->pc   = 3;
        yapArrayPush(&unit->funcs, func);
    }

    return unit;
}

void yapFunctionDestroy(yapFunction *func)
{
    yapFree(func->name);
    yapFree(func);
}

void yapUnitDestroy(yapUnit *p)
{
    yapArrayDestroy(&p->funcs, (yapDestroyCB)yapFunctionDestroy);
    yapFree(p->code);
    yapFree(p);
}

yapVM * yapVMCreate(void)
{
    yapVM *vm = yapAlloc(sizeof(*vm));
    yapUnit *unit = yapUnitCompile("wooo"); // TODO: replace
    yapVMLink(vm, unit);
    return vm;
}

void yapVMLink(yapVM *vm, yapUnit *unit)
{
    int i;
    for(i=0; i<unit->funcs.count; i++)
    {
        yapArrayPush(&vm->funcs, unit->funcs.data[i]);
    }
    yapArrayPush(&vm->units, unit);
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
    yapVMClearError(vm);
    yapArrayDestroy(&vm->units, (yapDestroyCB)yapUnitDestroy);
    yapArrayDestroy(&vm->funcs, NULL); // Shallow destroy, as units own funcs
    yapArrayDestroy(&vm->frames, yapFrameFree);
    yapArrayDestroy(&vm->stack, (yapDestroyCB)yapValueFree);
    yapFree(vm);
}

void yapValueClear(yapValue *p)
{
    p->type = YVT_UNKNOWN;

    // TODO: Clear string data if need be
}

void yapValueFree(yapValue *p)
{
    yapValueClear(p);
    yapFree(p);
}

yInline yBool yapValueEnsureExistence(yapVM *vm, yapValue *p)
{
    if(!p)
    {
        yapVMSetError(vm, "Attempting to manipulate NULL value! Are you popping an empty value stack?");
        return yFalse;
    }
    return yTrue;
}

yInline yBool yapValueConvertToInt(yapVM *vm, yapValue *p)
{
    if(!yapValueEnsureExistence(vm, p))
        return yFalse;

    // TODO: Convert string/float to int, etc
    p->type = YVT_INT;
    return yTrue;
}

yInline void yapValueSetInt(yapValue *p, int v)
{
    yapValueClear(p);
    p->type = YVT_INT;
    p->intValue = v;
}

static yapFunction * yapVMFindFunction(yapVM *vm, const char *funcName)
{
    int i;
    for(i=0; i<vm->funcs.count; i++)
    {
        yapFunction *func = (yapFunction *)(vm->funcs.data[i]);
        if(!strcmp(func->name, funcName))
            return func;
    }
    return NULL;
}

yapFrame * yapVMPushFrame(yapVM *vm, const char *funcName, int numArgs)
{
    yapFrame *frame;
    yapFunction *func = yapVMFindFunction(vm, funcName);
    if(!func)
    {
        yapVMSetError(vm, "Cannot find function '%s'", funcName);
        return NULL;
    }

    frame = yapAlloc(sizeof(*frame));
    frame->func = func;
    frame->ip   = &func->unit->code[func->pc];
    frame->bp   = vm->stack.count - numArgs;
    yapArrayPush(&vm->frames, frame);
    return frame;
}

void yapFrameFree(yapFrame *frame)
{
    yapArrayDestroy(&frame->ret, yapValueFree);
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
                yapValue *value = yapAlloc(sizeof(*value));
                yapValueSetInt(value, operand); // TODO: ADDRESS INTO KI TABLE
                yapArrayPush(&vm->stack, value);
            }
            break;

        case YOP_ADDKI:
            {
                yapValue *value = yapArrayTop(&vm->stack);
                if(!yapValueConvertToInt(vm, value))
                    break;
                yapValueSetInt(value, value->intValue + operand); // TODO: ADDRESS INTO KI TABLE
            }
            break;

        case YOP_SUBKI:
            {
                yapValue *value = yapArrayTop(&vm->stack);
                if(!yapValueConvertToInt(vm, value))
                    break;
                yapValueSetInt(value, value->intValue - operand); // TODO: ADDRESS INTO KI TABLE
            }
            break;

        case YOP_CALL:
            {
                frame = yapVMPushFrame(vm, "call", operand);
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
                yapFrameFree(frame);
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
                {
                    yapValue *value = yapArrayPop(&vm->stack);
                    if(value)
                        yapValueFree(value);
                }
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
}

void yapVMCall(yapVM *vm, const char *funcName, int numArgs)
{
    yapFrame *frame = yapVMPushFrame(vm, funcName, numArgs);
    if(!frame)
        return;

    yapVMLoop(vm);
}
