#include "yapObject.h"

#include "yapTypes.h"
#include "yapValue.h"
#include "yapVM.h"

yapObject *yapObjectCreate(struct yapVM *vm)
{
    yapObject *v = (yapObject *)yapAlloc(sizeof(yapObject));
    v->entries = yapArrayCreate();
    return v;
}

void yapDestroyDictEntry(struct yapObjectEntry *e)
{
    yapFree(e->key);
    yapFree(e);
}

void yapObjectDestroy(yapObject *v)
{
    yapArrayDestroy(v->entries, (yapDestroyCB)yapDestroyDictEntry);
    yapFree(v);
}

void yapObjectMark(yapObject *v)
{
    int i;
    for(i=0; i<v->entries->count; i++)
    {
        struct yapObjectEntry *e = v->entries->data[i];
        yapValueMark(e->val);
    }
}

struct yapValue **yapObjectGetRef(struct yapVM *vm, yapObject *object, const char *key, yBool create)
{
    struct yapValue **ref = NULL;
    int i;
    for(i=0; i<object->entries->count; i++)
    {
        struct yapObjectEntry *e = object->entries->data[i];
        if(!strcmp(e->key, key))
            ref = &e->val;
    }
    if(!ref)
    {
        if(create)
        {
            int newIndex;
            struct yapObjectEntry *e = yapAlloc(sizeof(struct yapObjectEntry));
            e->key = yapStrdup(key);
            e->val = yapValueNullPtr;
            newIndex = yapArrayPush(object->entries, e);
            ref = &(((struct yapObjectEntry *)object->entries->data[newIndex])->val);
        }
        else
        {
            ref = &yapValueNullPtr;
        }
    }
    return ref;
}
