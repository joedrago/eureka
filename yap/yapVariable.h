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

yapVariable *yapVariableCreate(const char *name);
void yapVariableDestroy(yapVariable *v);

#endif
