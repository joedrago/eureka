#include "yapValue.h"

#include "yapVM.h"

#include <stdio.h>
#include <string.h>

static char *NULL_STRING_FORM = "[null]";

// ---------------------------------------------------------------------------

yapValue yapValueNull = {YVT_NULL};

void yapValueSetInt(yapValue *p, int v)
{
    yapValueClear(p);
    p->type = YVT_INT;
    p->intVal = v;
}

void yapValueSetKString(yapValue *p, char *s)
{
    yapValueClear(p);
    p->type = YVT_STRING;
    p->stringVal = s;
    p->constant = yTrue;
}

void yapValueSetString(yapValue *p, char *s)
{
    yapValueClear(p);
    p->type = YVT_STRING;
    p->stringVal = yapStrdup(s);
    p->constant = yFalse;
}

void yapValueSetFunction(yapValue *p, struct yapBlock *block)
{
    yapValueClear(p);
    p->type = YVT_BLOCK;
    p->blockVal = block;
}

void yapValueSetCFunction(yapValue *p, yapCFunction func)
{
    yapValueClear(p);
    p->type = YVT_CFUNCTION;
    p->cFuncVal = func;
}

void yapValueClear(yapValue *p)
{
    if(p->type == YVT_STRING && !p->constant && !p->shared)
        yapFree(p->stringVal);

    memset(p, 0, sizeof(*p));
    p->type = YVT_NULL;
}

void yapValueDestroy(yapValue *p)
{
    yapValueClear(p);
    yapFree(p);
}

yapValue * yapValueCreate(struct yapVM *vm)
{
    yapValue *value = yapAlloc(sizeof(yapValue));
    yapArrayPush(&vm->usedValues, value);
    return value;
}

void yapValueCloneData(struct yapVM *vm, yapValue *dst, yapValue *src)
{
    dst->type = src->type;
    dst->constant = src->constant;
    switch(dst->type)
    {
    case YVT_NULL:
        break;
    case YVT_MODULE:
        dst->moduleVal = src->moduleVal;
        break;
    case YVT_BLOCK:
        dst->blockVal = src->blockVal;
        break;
    case YVT_INT:
        dst->intVal = src->intVal;
        break;
    case YVT_STRING:
        dst->stringVal = src->stringVal;
        if(!dst->constant)
        {
            dst->shared = yTrue;
            src->shared = yTrue;
        }
        break;
    default:
        yapVMSetError(vm, "yapValueCloneData(): cannot clone type %d", dst->type);
    };
}

yapValue *yapValueClone(struct yapVM *vm, yapValue *p)
{
    yapValue *n = yapValueCreate(vm);
    yapValueCloneData(vm, n, p);
    return n;
}

void yapValueMark(yapValue *value)
{
    if(value->type == YVT_NULL)
        return;

    if(value->used)
        value->shared = yTrue;
    else
        value->used = yTrue;

    // TODO: Arrays and Dicts need to have their subvalues marked recursively
}

yapValue * yapValueAdd(struct yapVM *vm, yapValue *a, yapValue *b)
{
    if(a->type == YVT_INT && b->type == YVT_INT)
        a->intVal += b->intVal;
    else
        yapTrace(("Don't know how to add types %d and %d\n", a->type, b->type));
    return a;
}

yapValue * yapValueSub(struct yapVM *vm, yapValue *a, yapValue *b)
{
    if(a->type == YVT_INT && b->type == YVT_INT)
        a->intVal -= b->intVal;
    else
        yapTrace(("Don't know how to add types %d and %d\n", a->type, b->type));
    return a;
}

yapValue * yapValueMul(struct yapVM *vm, yapValue *a, yapValue *b)
{
    if(a->type == YVT_INT && b->type == YVT_INT)
        a->intVal *= b->intVal;
    else
        yapTrace(("Don't know how to add types %d and %d\n", a->type, b->type));
    return a;
}

yapValue * yapValueDiv(struct yapVM *vm, yapValue *a, yapValue *b)
{
    if(a->type == YVT_INT && b->type == YVT_INT)
    {
        if(!b->intVal)
        {
            yapVMSetError(vm, "divide by zero!");
            return NULL;
        }
        a->intVal /= b->intVal;
    }
    else
        yapTrace(("Don't know how to add types %d and %d\n", a->type, b->type));
    return a;
}

yBool yapValueAsBool(yapValue *p)
{
    switch(p->type)
    {
        case YVT_NULL: 
            return yFalse;
        case YVT_STRING: 
            return (p->stringVal[0] != 0) ? yTrue : yFalse;
        case YVT_INT: 
            return (p->intVal != 0) ? yTrue : yFalse;
    };

    return yTrue;
}

void yapValueToString(yapValue *p)
{
    switch(p->type)
    {
        case YVT_STRING: 
            break;

        case YVT_NULL: 
            yapValueSetKString(p, NULL_STRING_FORM);
            break;

        case YVT_INT: 
            {
                char temp[32];
                sprintf(temp, "%d", p->intVal);
                yapValueSetString(p, temp);
            }
            break;
    };
}

