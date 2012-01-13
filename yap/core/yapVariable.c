#include "yapVariable.h"

#include "yapTypes.h"
#include "yapValue.h"
#include "yapVM.h"

yapVariable *yapVariableCreate(struct yapVM *vm, struct yapValue *value)
{
    yapVariable *v = (yapVariable *)yapAlloc(sizeof(yapVariable));
    v->value = value;
    yapArrayPush(&vm->usedVariables, v);
    return v;
}

void yapVariableDestroy(yapVariable *v)
{
    yapFree(v);
}

void yapVariableMark(struct yapVM *vm, yapVariable *variable)
{
    variable->used = yTrue;
    yapValueMark(vm, variable->value);
}
