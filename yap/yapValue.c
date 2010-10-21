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

void yapValueDonateString(yapValue *p, char *s)
{
    yapValueClear(p);
    p->type = YVT_STRING;
    p->stringVal = s;
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

// ---------------------------------------------------------------------------

void yapValueArrayPush(yapValue *p, yapValue *v)
{
    if(p->type != YVT_ARRAY)
    {
        yapValueClear(p);
        p->arrayVal = yapArrayCreate();
        p->type = YVT_ARRAY;
    }

    yapArrayPush(p->arrayVal, v);
}

// ---------------------------------------------------------------------------

void yapValueClear(yapValue *p)
{
    if((p->type == YVT_STRING) && !p->constant /*COW && !p->shared */)
        yapFree(p->stringVal);
    else if((p->type == YVT_ARRAY) && !p->shared)
        yapArrayDestroy(p->arrayVal, NULL);

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
    case YVT_CFUNCTION:
        dst->cFuncVal = src->cFuncVal;
        break;
    case YVT_REF:
        dst->refVal = src->refVal;
        break;
    case YVT_STRING:
        if(src->constant)
            dst->stringVal = src->stringVal;
        else
            dst->stringVal = yapStrdup(src->stringVal);
        /*COW
        dst->stringVal = src->stringVal;
        if(!dst->constant)
        {
            dst->shared = yTrue;
            src->shared = yTrue;
        } */
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

    if(!value->used && (value->type == YVT_ARRAY))
    {
        int i;
        for(i=0; i<value->arrayVal->count; i++)
        {
            yapValue *child = (yapValue *)value->arrayVal->data[i];
            yapValueMark(child);
        }
    }

    if(!value->used && (value->type == YVT_REF))
        yapVariableMark(value->refVal);

    // TODO: Dicts need to have their subvalues marked recursively

    if(value->used)
        value->shared = yTrue;
    else
        value->used = yTrue;
}

// TODO: Make a real string class that isn't terrible
static char *concat(char *a, char *b)
{
    yU32 newLen = (yU32)strlen(a) + (yU32)strlen(b);
    char *newString = yapAlloc(newLen + 1);
    strcpy(newString, a); // TODO: make this smarter
    strcat(newString, b);
    return newString;
}

yapValue * yapValueConcat(struct yapVM *vm, yapValue *a, yapValue *b)
{
    a = yapValueToString(vm, a);
    b = yapValueToString(vm, b);
    yapValueDonateString(a, concat(a->stringVal, b->stringVal));
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

yapValue * yapValueStringFormat(struct yapVM *vm, yapValue *format, yS32 argCount)
{
    char *out = yapStrdup("");
    int outSize = 1;
    int outPos = 0;
    int addLen;

    char *curr = format->stringVal;
    char *next;

    yapValue *arg;
    int argIndex = 0;

    while(curr && (next = strchr(curr, '%')))
    {
        // First, add in all of the stuff before the %
        {
            addLen = next - curr;
            outSize += addLen;
            out = yapRealloc(out, outSize);
            memcpy(&out[outPos], curr, addLen);
            outPos += addLen;
        }
        next++;

        switch(*next)
        {
        case '\0':
            curr = NULL; 
            break;
        case '%':
            out = yapRealloc(out, ++outSize);
            out[outPos++] = '%';
            break;
        case 's':
            arg = yapVMGetArg(vm, argIndex++, argCount);
            if(arg)
            {
                arg = yapValueToString(vm, arg);
                addLen = strlen(arg->stringVal);
                outSize += addLen;
                out = yapRealloc(out, outSize);
                memcpy(&out[outPos], arg->stringVal, addLen);
                outPos += addLen;
            }
            break;
        case 'd':
            arg = yapVMGetArg(vm, argIndex++, argCount);
            if(arg)
            {
                char temp[32];
                arg = yapValueToInt(vm, arg);
                sprintf(temp, "%d", arg->intVal);
                addLen = strlen(temp);
                outSize += addLen;
                out = yapRealloc(out, outSize);
                memcpy(&out[outPos], temp, addLen);
                outPos += addLen;
            }
            break;
        };

        curr = next+1;
    }

    // Add the remainder of the string, if any
    if(curr)
    {
        addLen = strlen(curr);
        outSize += addLen;
        out = yapRealloc(out, outSize);
        memcpy(&out[outPos], curr, addLen);
        outPos += addLen;
    }

    // Terminate the string
    out[outPos] = 0;

    format = yapValueCreate(vm);
    format->type = YVT_STRING;
    format->stringVal = out;

    yapVMPopValues(vm, argCount);
    return format;
}
