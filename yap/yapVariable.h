#ifndef YAPVARIABLE_H
#define YAPVARIABLE_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapValue;

// ---------------------------------------------------------------------------

typedef struct yapVariable
{
    char *name;
    struct yapValue *value;
    yFlag used:1;
} yapVariable;

yapVariable *yapVariableCreate(struct yapVM *vm, const char *name);
void yapVariableDestroy(yapVariable *v);

void yapVariableMark(yapVariable *variable); // used by yapVMGC()

#endif
