#include "yapValue.h"

#include "yapObject.h"
#include "yapLexer.h"
#include "yapVariable.h"
#include "yapVM.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

static char *NULL_STRING_FORM = "[null]";

// ---------------------------------------------------------------------------

yapValue yapValueNull = {YVT_NULL};
yapValue *yapValueNullPtr = &yapValueNull;

yapValue * yapValueSetInt(struct yapVM *vm, yapValue *p, int v)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(p);
    p->type = YVT_INT;
    p->intVal = v;
    p->used = yTrue;
    yapTrace(("yapValueSetInt %p [%d]\n", p, v));
    return p;
}

yapValue * yapValueSetKString(struct yapVM *vm, yapValue *p, char *s)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(p);
    p->type = YVT_STRING;
    p->stringVal = s;
    p->constant = yTrue;
    p->used = yTrue;
    yapTrace(("yapValueSetKString %p\n", p));
    return p;
}

yapValue * yapValueSetString(struct yapVM *vm, yapValue *p, char *s)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(p);
    p->type = YVT_STRING;
    p->stringVal = yapStrdup(s);
    p->constant = yFalse;
    p->used = yTrue;
    yapTrace(("yapValueSetString %p\n", p));
    return p;
}

yapValue * yapValueDonateString(struct yapVM *vm, yapValue *p, char *s)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(p);
    p->type = YVT_STRING;
    p->stringVal = s;
    p->constant = yFalse;
    p->used = yTrue;
    yapTrace(("yapValueDonateString %p\n", p));
    return p;
}

yapValue * yapValueSetFunction(struct yapVM *vm, yapValue *p, struct yapBlock *block)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(p);
    p->type = YVT_BLOCK;
    p->blockVal = block;
    p->used = yTrue;
    yapTrace(("yapValueSetFunction %p\n", p));
    return p;
}

yapValue * yapValueSetCFunction(struct yapVM *vm, yapValue *p, yapCFunction func)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(p);
    p->type = YVT_CFUNCTION;
    p->cFuncVal = func;
    p->used = yTrue;
    yapTrace(("yapValueSetCFunction %p\n", p));
    return p;
}

yapValue * yapValueSetModule(struct yapVM *vm, yapValue *p, struct yapModule *module)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(p);
    p->type = YVT_MODULE;
    p->moduleVal = module;
    p->used = yTrue;
    yapTrace(("yapValueSetModule %p\n", p));
    return p;
}

yapValue * yapValueSetRef(struct yapVM *vm, yapValue *p, struct yapValue **ref)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(p);
    p->type = YVT_REF;
    p->refVal = ref;
    p->used = yTrue;
    yapTrace(("yapValueSetRef %p\n", p));
    return p;
}

yBool yapValueSetRefVal(struct yapVM *vm, yapValue *ref, yapValue *p)
{
    if(!p)
    {
        yapVMSetError(vm, "yapValueSetRefVal: empty stack!");
        return yFalse;
    }
    if(!ref)
    {
        yapVMSetError(vm, "yapValueSetRefVal: empty stack!");
        return yFalse;
    }
    if(!(*ref->refVal))
    {
        yapVMSetError(vm, "yapValueSetRefVal: missing ref!");
        return yFalse;
    }
    if(ref->type != YVT_REF)
    {
        yapVMSetError(vm, "yapValueSetRefVal: value on top of stack, ref underneath");
        return yFalse;
    }

    *(ref->refVal) = p;
    p->used = yTrue;

    yapTrace(("yapValueSetRefVal %p = %p\n", ref, p));
    return yTrue;
}

// ---------------------------------------------------------------------------

yapValue * yapValueArrayCreate(struct yapVM *vm)
{
    yapValue *p = yapValueAcquire(vm);
    p->arrayVal = yapArrayCreate();
    p->type = YVT_ARRAY;
    p->used = yTrue;
    return p;
}

void yapValueArrayPush(yapVM *vm, yapValue *p, yapValue *v)
{
    p = yapValuePersonalize(vm, p);
    if(p->type != YVT_ARRAY)
    {
        yapValueClear(p);
        p->arrayVal = yapArrayCreate();
        p->type = YVT_ARRAY;
    }

    yapArrayPush(p->arrayVal, v);
    v->used = yTrue;
}

// ---------------------------------------------------------------------------

yapValue * yapValueObjectCreate(struct yapVM *vm)
{
    yapValue *p = yapValueAcquire(vm);
    p->objectVal = yapObjectCreate(vm);
    p->type = YVT_OBJECT;
    p->used = yTrue;
    return p;
}

// ---------------------------------------------------------------------------

void yapValueClear(yapValue *p)
{
    if((p->type == YVT_STRING) && !p->constant)
        yapFree(p->stringVal);
    else if(p->type == YVT_ARRAY)
        yapArrayDestroy(p->arrayVal, NULL);
    else if(p->type == YVT_OBJECT)
        yapObjectDestroy(p->objectVal);

    memset(p, 0, sizeof(*p));
    p->type = YVT_NULL;
}

void yapValueRelease(yapVM *vm, yapValue *p)
{
    yapTrace(("yapValueRelease %p\n", p));
    yapValueClear(p);
    yapArrayPush(&vm->freeValues, p);
}

void yapValueDestroy(yapValue *p)
{
    yapTrace(("yapValueFree %p\n", p));
    yapValueClear(p);
    yapFree(p);
}

static yapValue * yapValueCreate()
{
    yapValue *value = yapAlloc(sizeof(yapValue));
    yapTrace(("yapValueCreate %p\n", value));
    return value;
}

yapValue * yapValueAcquire(struct yapVM *vm)
{
    yapValue *value = yapArrayPop(&vm->freeValues);
    if(!value)
    {
        value = yapValueCreate();
    };
    yapArrayPush(&vm->usedValues, value);
    yapTrace(("yapValueAcquire %p\n", value));
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
        break;
    default:
        yapVMSetError(vm, "yapValueCloneData(): cannot clone type %d", dst->type);
    };
}

yapValue *yapValueClone(struct yapVM *vm, yapValue *p)
{
    yapValue *n = yapValueAcquire(vm);
    yapValueCloneData(vm, n, p);
    yapTrace(("yapValueClone %p -> %p\n", p, n));
    return n;
}

yapValue * yapValuePersonalize(struct yapVM *vm, yapValue *p)
{
    if(p == &yapValueNull)
        return yapValueAcquire(vm);

    if(p->type == YVT_ARRAY) // Arrays are all shared
        return p;

    if(p->type == YVT_OBJECT) // Objects are all shared
        return p;

    if(p->used)
        return yapValueClone(vm, p);

    return p;
}

void yapValueMark(yapValue *value)
{
    if(value->type == YVT_NULL)
        return;

    if(value->used)
        return;

    value->used = yTrue;

    if(value->type == YVT_ARRAY)
    {
        int i;
        for(i=0; i<value->arrayVal->count; i++)
        {
            yapValue *child = (yapValue *)value->arrayVal->data[i];
            yapValueMark(child);
        }
    }
    else if((value->type == YVT_REF) && *value->refVal)
    {
        yapValueMark(*value->refVal);
    }
    else if(value->type == YVT_OBJECT)
    {
        yapObjectMark(value->objectVal);
    }
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
    a = yapValueDonateString(vm, a, concat(a->stringVal, b->stringVal));
    return a;
}

yapValue * yapValueIntOp(struct yapVM *vm, yapValue *a, yapValue *b, char op)
{
    a = yapValueToInt(vm, a);
    b = yapValueToInt(vm, b);
    switch(op)
    {
        case '+': a = yapValueSetInt(vm, a, a->intVal + b->intVal); break;
        case '-': a = yapValueSetInt(vm, a, a->intVal - b->intVal); break;
        case '*': a = yapValueSetInt(vm, a, a->intVal * b->intVal); break;
        case '/': 
            if(!b->intVal)
            {
                yapVMSetError(vm, "divide by zero!");
                return NULL;
            }
            a = yapValueSetInt(vm, a, a->intVal / b->intVal);
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
    switch(p->type)
    {
        case YVT_INT: 
            break;

        case YVT_NULL: 
            p = yapValueSetInt(vm, p, 0);
            break;

        case YVT_STRING: 
            p = yapValueSetInt(vm, p, (p->stringVal[0] != 0) ? 1 : 0);
            break;

        default:
            printf("yapValueToBool: unhandled case %d\n", p->type);
            return NULL;
            break;
    };

    return p;
}

yapValue * yapValueToInt(struct yapVM *vm, yapValue *p)
{
    switch(p->type)
    {
        case YVT_INT: 
            break;

        case YVT_NULL: 
            p = yapValueSetInt(vm, p, 0);
            break;

        case YVT_STRING: 
        {
            yapToken t = { p->stringVal, strlen(p->stringVal) };
            p = yapValueSetInt(vm, p, yapTokenToInt(&t));
            break;
        }

        default:
            printf("yapValueToInt: unhandled case %d\n", p->type);
            return NULL;
            break;
    };

    return p;
}

yapValue * yapValueToString(struct yapVM *vm, yapValue *p)
{
    switch(p->type)
    {
        case YVT_STRING: 
            break;

        case YVT_NULL: 
            p = yapValueSetKString(vm, p, NULL_STRING_FORM);
            break;

        case YVT_INT: 
            {
                char temp[32];
                sprintf(temp, "%d", p->intVal);
                p = yapValueSetString(vm, p, temp);
            }
            break;

        default:
            printf("yapValueToString: unhandled case %d\n", p->type);
            return NULL;
            break;
    };
    return p;
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

    format = yapValueAcquire(vm);
    format->type = YVT_STRING;
    format->stringVal = out;

    yapVMPopValues(vm, argCount);
    return format;
}
