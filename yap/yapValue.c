#include "yapValue.h"
#include "yapVM.h"

#include <string.h>

// ---------------------------------------------------------------------------

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

yapValue * yapValueCreate(struct yapVM *vm)
{
    return yapAlloc(sizeof(yapValue));
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
        if(n->constant)
            n->stringVal = p->stringVal;
        else
            n->stringVal = yapStrdup(p->stringVal);
        break;
    };
    return n;
}

