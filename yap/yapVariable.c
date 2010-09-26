#include "yapVariable.h"

#include "yapTypes.h"
#include "yapValue.h"

yapVariable *yapVariableAlloc(const char *name)
{
    yapVariable *v = (yapVariable *)yapAlloc(sizeof(*v));
    v->name = yapStrdup(name);
    v->value = None;
    return v;
}

void yapVariableFree(yapVariable *v)
{
    yapFree(v->name);
    yapFree(v);
}
