#include "yapVariable.h"

#include "yapTypes.h"
#include "yapValue.h"

yapVariable *yapVariableCreate(const char *name)
{
    yapVariable *v = (yapVariable *)yapAlloc(sizeof(yapVariable));
    v->name = yapStrdup(name);
    v->value = None;
    return v;
}

void yapVariableDestroy(yapVariable *v)
{
    yapFree(v->name);
    yapFree(v);
}
