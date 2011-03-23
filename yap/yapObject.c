#include "yapDict.h"

#include "yapTypes.h"
#include "yapValue.h"
#include "yapVM.h"

yapDict *yapDictCreate(struct yapVM *vm)
{
    yapDict *v = (yapDict *)yapAlloc(sizeof(yapDict));
    v->entries = yapArrayCreate();
    return v;
}

void yapDestroyDictEntry(struct yapDictEntry *e)
{
    yapFree(e->key);
    yapFree(e);
}

void yapDictDestroy(yapDict *v)
{
    yapArrayDestroy(v->entries, (yapDestroyCB)yapDestroyDictEntry);
    yapFree(v);
}

void yapDictMark(yapDict *v)
{
    int i;
    for(i=0; i<v->entries->count; i++)
    {
        struct yapDictEntry *e = v->entries->data[i];
        yapValueMark(e->val);
    }
}

struct yapValue **yapDictGetRef(struct yapVM *vm, yapDict *dict, const char *key, yBool create)
{
    struct yapValue **ref = NULL;
    int i;
    for(i=0; i<dict->entries->count; i++)
    {
        struct yapDictEntry *e = dict->entries->data[i];
        if(!strcmp(e->key, key))
            ref = &e->val;
    }
    if(!ref)
    {
        if(create)
        {
            int newIndex;
            struct yapDictEntry *e = yapAlloc(sizeof(struct yapDictEntry));
            e->key = yapStrdup(key);
            e->val = yapValueNullPtr;
            newIndex = yapArrayPush(dict->entries, e);
            ref = &(((struct yapDictEntry *)dict->entries->data[newIndex])->val);
        }
        else
        {
            ref = &yapValueNullPtr;
        }
    }
    return ref;
}
