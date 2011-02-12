#ifndef YAPDICT_H
#define YAPDICT_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapValue;
struct yapVM;

// ---------------------------------------------------------------------------

typedef struct yapDictEntry
{
    char *key;
    struct yapValue *val;
};

typedef struct yapDict
{
    yapArray *entries;
} yapDict;

yapDict *yapDictCreate(struct yapVM *vm);
void yapDictDestroy(yapDict *v);
void yapDictMark(yapDict *v);

struct yapValue **yapDictGetRef(struct yapVM *vm, yapDict *dict, const char *key, yBool create);

#endif
