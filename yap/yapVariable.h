#ifndef YAPVARIABLE_H
#define YAPVARIABLE_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapValue;
struct yapVM;

// ---------------------------------------------------------------------------

typedef struct yapVariable
{
    struct yapValue *value;
    yFlag used: 1;
} yapVariable;

yapVariable *yapVariableCreate(struct yapVM *vm, struct yapValue *value);
void yapVariableDestroy(yapVariable *v);

void yapVariableMark(struct yapVM *vm, yapVariable *variable); // used by yapVMGC()

#endif
