#include "yapValue.h"

#include "yapVM.h"

#include <string.h>

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
    p->type = YVT_FUNCTION;
    p->blockVal = block;
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
    case YVT_FUNCTION:
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


yBool yapValueEnsureExistence(yapVM *vm, yapValue *p)
{
    if(!p)
    {
        yapVMSetError(vm, "Attempting to manipulate NULL value! Are you popping an empty value stack?");
        return yFalse;
    }
    return yTrue;
}

yBool yapValueConvertToInt(yapVM *vm, yapValue *p)
{
    if(!yapValueEnsureExistence(vm, p))
        return yFalse;

    // TODO: Convert string/float to int, etc
    p->type = YVT_INT;
    return yTrue;
}
