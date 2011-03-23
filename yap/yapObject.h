#ifndef YAPOBJECT_H
#define YAPOBJECT_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapValue;
struct yapVM;

// ---------------------------------------------------------------------------

typedef struct yapObjectEntry
{
    char *key;
    struct yapValue *val;
} yapObjectEntry;

typedef struct yapObject
{
    yapArray *entries;
} yapObject;

yapObject *yapObjectCreate(struct yapVM *vm);
void yapObjectDestroy(yapObject *v);
void yapObjectMark(yapObject *v);

struct yapValue **yapObjectGetRef(struct yapVM *vm, yapObject *dict, const char *key, yBool create);

#endif
