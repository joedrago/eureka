#include "yapVM.h"

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
    block = yapAlloc(sizeof(*block));
    // note: block->module is set in the next stanza
    block->ops = yapAlloc(sizeof(*block->ops) * 7);

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
    
    // Create main
    main = yapAlloc(sizeof(*main));
    block->module = main;
    main->block = block;
    yapArrayPush(&vm->modules, main);

    // Create the global variable "main"
    v = yapVariableAlloc("main");
    v->value.type = YVT_MODULE;
    v->value.moduleVal = main;
    yapArrayPush(&vm->globals, v);

    // Add a function: int call(int a) -- returns a+2
    block = yapAlloc(sizeof(*block));
    block->module = main;
    block->ops = yapAlloc(sizeof(*block->ops) * 4);

    // return arg0 + 2;
    block->ops[0].opcode  = YOP_PUSHARGN;
    block->ops[0].operand = 0;
    block->ops[1].opcode  = YOP_ADDKI;
    block->ops[1].operand = yap32ArrayPushUnique(&vm->kInts, 2);
    block->ops[2].opcode  = YOP_RET;
    block->ops[2].operand = 1;

    yapArrayPush(&vm->blocks, block);

    // Create main.call()
    v = yapVariableAlloc("call");
    v->value.type = YVT_FUNCTION;
    v->value.blockVal = block;
    yapArrayPush(&main->variables, v);

    return 2;
}

void yapModuleFree(yapModule *module)
{
    // module->block is owned by vm->blocks
    yapArrayClear(&module->variables, yapVariableFree);
    yapFree(module);
}

void yapBlockFree(yapBlock *block)
{
    yapFree(block->ops);
    yapFree(block);
}

yapVM * yapVMCreate(void)
{
    yapVM *vm = yapAlloc(sizeof(*vm));
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

void yapVMFree(yapVM *vm)
{
    yapArrayClear(&vm->globals, (yapDestroyCB)yapVariableFree);
    yapArrayClear(&vm->blocks, (yapDestroyCB)yapBlockFree);
    yapArrayClear(&vm->modules, (yapDestroyCB)yapModuleFree);
    yapArrayClear(&vm->frames, (yapDestroyCB)yapFrameFree);
    yapArrayClear(&vm->stack, (yapDestroyCB)yapValueFree);
    yapArrayClear(&vm->kStrings, (yapDestroyCB)yapFree);
    yap32ArrayClear(&vm->kInts);
    yapVMClearError(vm);
    yapFree(vm);
}

void yapValueClear(yapValue *p)
{
    if(p->type == YVT_STRING && !p->constant)
        yapFree(p->stringVal);

    memset(p, 0, sizeof(*p));
    p->type = YVT_UNKNOWN;
}

void yapValueFree(yapValue *p)
{
    yapValueClear(p);
    yapFree(p);
}

yapValue *yapValueDupe(yapValue *p)
{
    yapValue *n = yapAlloc(sizeof(*n));
    n->type = p->type;
    n->constant = p->constant;
    switch(n->type)
    {
    case YVT_MODULE:
        n->moduleVal = p->moduleVal;
        break;
    case YVT_FUNCTION:
        n->blockVal = p->blockVal;
        break;
    case YVT_INT:
        n->intVal = p->intVal;
        break;
    case YVT_STRING:
        if(n->constant)
            n->stringVal = p->stringVal;
        else
            n->stringVal = yapStrdup(p->stringVal);
        break;
    };
    return n;
}

yapVariable *yapVariableAlloc(const char *name)
{
    yapVariable *v = (yapVariable *)yapAlloc(sizeof(*v));
    v->name = yapStrdup(name);
    v->value.type = YVT_UNKNOWN;
    return v;
}

void yapVariableFree(yapVariable *v)
{
    yapFree(v->name);
    yapValueClear(&v->value);
    yapFree(v);
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
    p->intVal = v;
}

yInline void yapValueSetConstantString(yapValue *p, char *s)
{
    yapValueClear(p);
    p->type = YVT_STRING;
    p->stringVal = s;
    p->constant = yTrue;
}

yInline void yapValueSetString(yapValue *p, const char *s)
{
    yapValueClear(p);
    p->type = YVT_STRING;
    p->stringVal = yapStrdup(s);
    p->constant = yFalse;
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

    block = (v->value.type == YVT_MODULE) ? v->value.moduleVal->block : v->value.blockVal;

    frame = yapAlloc(sizeof(*frame));
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

void yapFrameFree(yapFrame *frame)
{
    yapArrayClear(&frame->ret,  (yapDestroyCB)yapValueFree);
    yapArrayClear(&frame->args, (yapDestroyCB)yapValueFree);
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
                yapValue *value = yapAlloc(sizeof(*value));
                yapValueSetConstantString(value, vm->kStrings.data[operand]);
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
                    yapValue *value = yapValueDupe((yapValue *)frame->args.data[operand]);
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
                if(callName)
                    yapValueFree(callName);
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

void yapVMCall(yapVM *vm, const char *name, int numArgs)
{
    yapFrame *frame = yapVMPushFrame(vm, name, numArgs);
    if(!frame)
        return;

    yapVMLoop(vm);
}
