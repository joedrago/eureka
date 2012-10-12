// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapObject.h"

#include "yapTypes.h"
#include "yapHash.h"
#include "yapValue.h"
#include "yapContext.h"

yapObject *yapObjectCreate(struct yapContext *Y, yapValue *isa)
{
    yapObject *v = (yapObject *)yapAlloc(sizeof(yapObject));
    v->isa = isa;
    v->hash = yapHashCreate(Y, 0);
    return v;
}

void yapObjectDestroy(struct yapContext *Y, yapObject *v)
{
    yapHashDestroy(Y, v->hash, NULL);
    yapFree(v);
}

static void yapHashValueMark(struct yapContext *Y, yapHashEntry *entry)
{
    yapValueMark(Y, entry->value);
}

void yapObjectMark(struct yapContext *Y, yapObject *v)
{
    yapHashIterate(Y, v->hash, (yapIterateCB)yapHashValueMark);
    if(v->isa)
    {
        yapValueMark(Y, v->isa);    // Is this necessary?
    }
}

struct yapValue **yapObjectGetRef(struct yapContext *Y, yapObject *object, const char *key, yBool create)
{
    struct yapValue **ref = (struct yapValue **)yapHashLookup(Y, object->hash, key, create);
    if(create)
    {
        if(*ref == NULL)
        {
            *ref = yapValueNullPtr;
        }
    }
    else if(!ref)
    {
        if(object->isa)
        {
            return yapObjectGetRef(Y, object->isa->objectVal, key, create);
        }
        ref = &yapValueNullPtr;
    }
    return ref;
}
