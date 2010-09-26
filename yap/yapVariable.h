#ifndef YAPVARIABLE_H
#define YAPVARIABLE_H

// ---------------------------------------------------------------------------
// Forwards

struct yapValue;

// ---------------------------------------------------------------------------

typedef struct yapVariable
{
    char *name;
    struct yapValue *value;
} yapVariable;

yapVariable *yapVariableAlloc(const char *name);
void yapVariableFree(yapVariable *v);

#endif
