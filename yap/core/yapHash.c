// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapHash.h"
#include "yapContext.h"

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

yapHash *yapHashCreate(struct yapContext *Y, int sizeEstimate)
{
    int width = 7; // TODO: do something interesting to calculate width from sizeEstimate
    yapHash *yh;
    yh = yapAlloc(sizeof(yapHash));
    yh->table = yapAlloc(sizeof(yapHashEntry*) * width);
    yh->width = width;
    return yh;
}

static void yapHashEntryDestroy(struct yapContext *Y, yapHashEntry *entry, yapDestroyCB cb)
{
    if(cb)
        cb(Y, entry->value);
    yapFree(entry->key);
    yapFree(entry);
}

void yapHashIterate(struct yapContext *Y, yapHash *yh, yapIterateCB cb)
{
    int i;
    if(!cb)
        return;
    for(i=0; i<yh->width; i++)
    {
        yapHashEntry *entry = yh->table[i];
        while(entry)
        {
            cb(Y, entry);
            entry = entry->next;
        }
    }
}

void yapHashIterateP1(struct yapContext *Y, yapHash *yh, yapIterateCB1 cb, void *arg1)
{
    int i;
    if(!cb)
        return;
    for(i=0; i<yh->width; i++)
    {
        yapHashEntry *entry = yh->table[i];
        while(entry)
        {
            cb(Y, arg1, entry);
            entry = entry->next;
        }
    }
}

void yapHashClear(struct yapContext *Y, yapHash *yh, yapDestroyCB cb)
{
    int i;
    for(i=0; i<yh->width; i++)
    {
        yapHashEntry *entry = yh->table[i];
        while(entry)
        {
            yapHashEntry *freeme = entry;
            entry = entry->next;
            yapHashEntryDestroy(Y, freeme, cb);
        }
    }
    memset(yh->table, 0, sizeof(yapHashEntry*) * yh->width);
}

void yapHashDestroy(struct yapContext *Y, yapHash *yh, yapDestroyCB cb)
{
    yapHashClear(Y, yh, cb);
    yapFree(yh->table);
    yapFree(yh);
}

void **yapHashLookup(struct yapContext *Y, yapHash *yh, const char *key, yBool create)
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
        entry->key = yapStrdup(Y, key);
        entry->hash = hash;
        entry->next = yh->table[index];
        yh->table[index] = entry;
        return &entry->value;
    }
    return NULL;
}

void **yapHashSet(struct yapContext *Y, yapHash *yh, const char *key, void *value)
{
    void **ref = yapHashLookup(Y, yh, key, yTrue);
    yapAssert(ref);
    *ref = value;
    return ref;
}

void yapHashDelete(struct yapContext *Y, yapHash *yh, const char *key, yapDestroyCB cb)
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
        yapHashEntryDestroy(Y, found, cb);
    }
}
