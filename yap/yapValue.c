#include "yapValue.h"

#include "yapLexer.h"
#include "yapVM.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

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

yapValue * yapValueConcat(struct yapVM *vm, yapValue *a, yapValue *b)
{
    yU32 newLen;
    char *newString;

    a = yapValueToString(vm, a);
    b = yapValueToString(vm, b);
    newLen = (yU32)strlen(a->stringVal) + (yU32)strlen(b->stringVal);
    newString = yapAlloc(newLen + 1);
    strcpy(newString, a->stringVal); // TODO: make this smarter
    strcat(newString, b->stringVal);

    a->stringVal = newString;        // due to yapValueToString's clone, no free required
    a->type = YVT_STRING;
    a->shared   = yFalse;
    a->constant = yFalse;
    return a;
}

yapValue * yapValueIntOp(struct yapVM *vm, yapValue *a, yapValue *b, char op)
{
    a = yapValueToInt(vm, a);
    b = yapValueToInt(vm, b);
    switch(op)
    {
        case '+': yapValueSetInt(a, a->intVal + b->intVal); break;
        case '-': yapValueSetInt(a, a->intVal - b->intVal); break;
        case '*': yapValueSetInt(a, a->intVal * b->intVal); break;
        case '/': 
            if(!b->intVal)
            {
                yapVMSetError(vm, "divide by zero!");
                return NULL;
            }
            yapValueSetInt(a, a->intVal / b->intVal);
            break;
    };
    return a;
}

yapValue * yapValueAdd(struct yapVM *vm, yapValue *a, yapValue *b)
{
    if(a->type == YVT_STRING)
        return yapValueConcat(vm, a, b);
    else if(a->type == YVT_INT)
        a = yapValueIntOp(vm, a, b, '+');
    else
        yapTrace(("Don't know how to add types %d and %d\n", a->type, b->type));
    return a;
}

yapValue * yapValueSub(struct yapVM *vm, yapValue *a, yapValue *b)
{
    if(a->type == YVT_INT)
        a = yapValueIntOp(vm, a, b, '-');
    else
        yapTrace(("Don't know how to subtract types %d and %d\n", a->type, b->type));
    return a;
}

yapValue * yapValueMul(struct yapVM *vm, yapValue *a, yapValue *b)
{
    if(a->type == YVT_INT)
        a = yapValueIntOp(vm, a, b, '*');
    else
        yapTrace(("Don't know how to multiply types %d and %d\n", a->type, b->type));
    return a;
}

yapValue * yapValueDiv(struct yapVM *vm, yapValue *a, yapValue *b)
{
    if(a->type == YVT_INT)
        a = yapValueIntOp(vm, a, b, '/');
    else
        yapTrace(("Don't know how to divide types %d and %d\n", a->type, b->type));
    return a;
}

yapValue * yapValueToBool(struct yapVM *vm, yapValue *p)
{
    yapValue *v = yapValueClone(vm, p);
    switch(v->type)
    {
        case YVT_NULL: 
            v->intVal = 0;
            break;
        case YVT_STRING: 
            // I can get away with non-free'ing murder here due to copy-on-write
            v->intVal = (p->stringVal[0] != 0) ? 1 : 0;
            break;
    };

    v->type = YVT_INT;
    return v;
}

yapValue * yapValueToInt(struct yapVM *vm, yapValue *p)
{
    yapValue *v = yapValueClone(vm, p);
    switch(v->type)
    {
        case YVT_NULL: 
            v->intVal = 0;
            break;
        case YVT_STRING: 
        {
            yapToken t = { p->stringVal, strlen(p->stringVal) };
            v->intVal = yapTokenToInt(&t);
            break;
        }
    };

    v->type = YVT_INT;
    return v;
}

yapValue * yapValueToString(struct yapVM *vm, yapValue *p)
{
    yapValue *v = yapValueClone(vm, p);
    switch(v->type)
    {
        case YVT_STRING: 
            break;

        case YVT_NULL: 
            yapValueSetKString(v, NULL_STRING_FORM);
            break;

        case YVT_INT: 
            {
                char temp[32];
                sprintf(temp, "%d", v->intVal);
                yapValueSetString(v, temp);
            }
            break;
    };
    return v;
}

