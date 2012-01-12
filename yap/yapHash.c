#include "yapHash.h"

#include <stdlib.h>
#include <string.h>

static yU32 djb2hash(const unsigned char *str)
{
    yU32 hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

yapHash *yapHashCreate(int sizeEstimate)
{
    int width = 7; // TODO: do something interesting to calculate width from sizeEstimate
    yapHash *yh;
    yh = yapAlloc(sizeof(yapHash));
    yh->table = yapAlloc(sizeof(yapHashEntry*) * width);
    yh->width = width;
    return yh;
}

static void yapHashEntryDestroy(yapHashEntry *entry, yapDestroyCB cb)
{
    if(cb)
        cb(entry->value);
    yapFree(entry->key);
    yapFree(entry);
}

void yapHashIterateP1(yapHash *yh, yapIterateCB1 cb, void *arg1)
{
    int i;
    if(!cb)
        return;
    for(i=0; i<yh->width; i++)
    {
        yapHashEntry *entry = yh->table[i];
        while(entry)
        {
            cb(arg1, entry);
            entry = entry->next;
        }
    }
}

void yapHashClear(yapHash *yh, yapDestroyCB cb)
{
    int i;
    for(i=0; i<yh->width; i++)
    {
        yapHashEntry *entry = yh->table[i];
        while(entry)
        {
            yapHashEntry *freeme = entry;
            entry = entry->next;
            yapHashEntryDestroy(freeme, cb);
        }
    }
    memset(yh->table, 0, sizeof(yapHashEntry*) * yh->width);
}

void yapHashDestroy(yapHash *yh, yapDestroyCB cb)
{
    yapHashClear(yh, cb);
    yapFree(yh->table);
    yapFree(yh);
}

void **yapHashLookup(yapHash *yh, const char *key, yBool create)
{
    yU32 hash = djb2hash(key);
    yU32 index = hash % yh->width;
    yapHashEntry *entry = yh->table[index];
    yapHashEntry *found = NULL;
    while(entry)
    {
        if((entry->hash == hash)
        && (!strcmp(entry->key, key)))
        {
            found = entry;
            break;
        }
        entry = entry->next;
    }

    if(found)
        return &found->value;
    if(create)
    {
        yh->count++;
        entry = yapAlloc(sizeof(yapHashEntry));
        entry->key = yapStrdup(key);
        entry->hash = hash;
        entry->next = yh->table[index];
        yh->table[index] = entry;
        return &entry->value;
    }
    return NULL;
}

void yapHashSet(yapHash *yh, const char *key, void *value)
{
    void **ref = yapHashLookup(yh, key, yTrue);
    yapAssert(ref);
    *ref = value;
}

void yapHashDelete(yapHash *yh, const char *key, yapDestroyCB cb)
{
    yU32 hash = djb2hash(key);
    yU32 index = hash % yh->width;
    yapHashEntry *entry = yh->table[index];
    yapHashEntry *parent = NULL;
    yapHashEntry *found = NULL;
    while(entry)
    {
        if((entry->hash == hash)
        && (!strcmp(entry->key, key)))
        {
            found = entry;
            break;
        }
        parent = entry;
        entry = entry->next;
    }

    if(found)
    {
        if(parent)
        {
            parent->next = found->next;
        }
        else
        {
            // No parent means it was the head
            yh->table[index] = NULL;
        }
        yapHashEntryDestroy(found, cb);
    }
}
