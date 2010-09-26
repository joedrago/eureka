#include "yapValue.h"

#include "yapVM.h"

#include <string.h>

// ---------------------------------------------------------------------------

yapValue yapValueNone = {YVT_NONE};

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

void yapValueClear(yapValue *p)
{
    if(p->type == YVT_STRING && !p->constant && !p->shared)
        yapFree(p->stringVal);

    memset(p, 0, sizeof(*p));
    p->type = YVT_UNKNOWN;
}

void yapValueFree(yapValue *p)
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

yapValue *yapValueClone(struct yapVM *vm, yapValue *p)
{
    yapValue *n = yapValueCreate(vm);
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
        n->stringVal = p->stringVal;
        if(!n->constant)
        {
            n->shared = yTrue;
            p->shared = yTrue;
        }
        break;
    };
    return n;
}

void yapValueMark(yapValue *value)
{
    if(value->type == YVT_NONE)
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
