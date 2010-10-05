#include "yapVariable.h"

#include "yapTypes.h"
#include "yapValue.h"
#include "yapVM.h"

yapVariable *yapVariableCreate(struct yapVM *vm, const char *name)
{
    yapVariable *v = (yapVariable *)yapAlloc(sizeof(yapVariable));
    v->name = yapStrdup(name);
    v->value = &yapValueNull;
    yapArrayPush(&vm->usedVariables, v);
    return v;
}

void yapVariableDestroy(yapVariable *v)
{
    yapFree(v->name);
    yapFree(v);
}

void yapVariableMark(yapVariable *variable)
{
    variable->used = yTrue;
}
