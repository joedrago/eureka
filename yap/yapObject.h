#ifndef YAPOBJECT_H
#define YAPOBJECT_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapHash;
struct yapValue;
struct yapVM;

typedef struct yapObject
{
    struct yapValue *isa; // "is a": what this object is derived from. Can be NULL.
    struct yapHash *hash;
} yapObject;

yapObject *yapObjectCreate(struct yapVM *vm, struct yapValue *isa);
void yapObjectDestroy(yapObject *v);
void yapObjectMark(struct yapVM *vm, yapObject *v);

struct yapValue **yapObjectGetRef(struct yapVM *vm, yapObject *object, const char *key, yBool create);

#endif
