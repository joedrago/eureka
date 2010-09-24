#include "yapVM.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_ERROR_LENGTH 1023

int yapCompile(const char *text, yapArray *outBlocks)
{
    yapBlock *block;

    // Someday this'll actually compile stuff!

    block = yapAlloc(sizeof(*block));
    block->ops = yapAlloc(sizeof(*block->ops) * 4);
    block->ops[0].opcode  = YOP_PUSHKI;
    block->ops[0].operand = 2;
    block->ops[1].opcode  = YOP_ADDKI;
    block->ops[1].operand = 2;
    block->ops[2].opcode  = YOP_RET;
    block->ops[2].operand = 1;
    block->name = yapStrdup("call");
    yapArrayPush(outBlocks, block);

    block = yapAlloc(sizeof(*block));
    block->ops = yapAlloc(sizeof(*block->ops) * 6);
    block->ops[0].opcode  = YOP_CALL;
    block->ops[1].opcode  = YOP_PUSHKI;
    block->ops[1].operand = 2;
    block->ops[2].opcode  = YOP_PUSHKI;
    block->ops[2].operand = 2;
    block->ops[3].opcode  = YOP_PUSHKI;
    block->ops[3].operand = 2;
    block->ops[4].opcode  = YOP_RET;
    block->ops[4].operand = 1;
    block->name = yapStrdup("main");
    yapArrayPush(outBlocks, block);

    return 2;
}

void yapBlockDestroy(yapBlock *block)
{
    yapFree(block->ops);
    yapFree(block->name);
    yapFree(block);
}

yapVM * yapVMCreate(void)
{
    yapVM *vm = yapAlloc(sizeof(*vm));
    yapCompile("wooo", &vm->blocks);
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
    yapVMClearError(vm);
    yapArrayDestroy(&vm->blocks, (yapDestroyCB)yapBlockDestroy);
    yapArrayDestroy(&vm->frames, (yapDestroyCB)yapFrameFree);
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

static yapBlock * yapVMFindBlock(yapVM *vm, const char *blockName)
{
    int i;
    for(i=0; i<vm->blocks.count; i++)
    {
        yapBlock *block = (yapBlock *)(vm->blocks.data[i]);
        if(!strcmp(block->name, blockName))
            return block;
    }
    return NULL;
}

yapFrame * yapVMPushFrame(yapVM *vm, const char *blockName, int numArgs)
{
    yapFrame *frame;
    yapBlock *block = yapVMFindBlock(vm, blockName);
    if(!block)
    {
        yapVMSetError(vm, "Cannot find blocktion '%s'", blockName);
        return NULL;
    }

    frame = yapAlloc(sizeof(*frame));
    frame->block = block;
    frame->ip = block->ops;
    frame->bp = vm->stack.count - numArgs;
    yapArrayPush(&vm->frames, frame);
    return frame;
}

void yapFrameFree(yapFrame *frame)
{
    yapArrayDestroy(&frame->ret, (yapDestroyCB)yapValueFree);
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
                frame = yapVMPushFrame(vm, "call", operand); // TODO: Get name from top of stack, get var, get block from var
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

void yapVMCall(yapVM *vm, const char *blockName, int numArgs)
{
    yapFrame *frame = yapVMPushFrame(vm, blockName, numArgs);
    if(!frame)
        return;

    yapVMLoop(vm);
}
