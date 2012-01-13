#ifndef YAPHASH_H
#define YAPHASH_H

#include "yapTypes.h"

typedef struct yapHashEntry
{
    yU32 hash;
    char *key;
    void *value;
    struct yapHashEntry *next;    
} yapHashEntry;

typedef struct yapHash
{
    int count;
    int width; // number of ptrs in table array
    yapHashEntry **table;
} yapHash;

yapHash *yapHashCreate(int sizeEstimate);
void yapHashClear(yapHash *yh, yapDestroyCB cb);
void yapHashDestroy(yapHash *yh, yapDestroyCB cb);

typedef void (*yapIterateCB1)(void *arg1, yapHashEntry *entry);
void yapHashIterateP1(yapHash *yh, yapIterateCB1 cb, void *arg1); // One prefixed argument: cb(arg1, p)

void **yapHashLookup(yapHash *yh, const char *key, yBool create);
void yapHashSet(yapHash *yh, const char *key, void *value);       // assumes 'create' and 'replace'
void yapHashDelete(yapHash *yh, const char *key, yapDestroyCB cb);

#endif
