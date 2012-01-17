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
#include "yapVM.h"

yapObject *yapObjectCreate(struct yapVM *vm, yapValue *isa)
{
    yapObject *v = (yapObject *)yapAlloc(sizeof(yapObject));
    v->isa = isa;
    v->hash = yapHashCreate(0);
    return v;
}

void yapObjectDestroy(yapObject *v)
{
    yapHashDestroy(v->hash, NULL);
    yapFree(v);
}

static void yapHashValueMark(yapVM *vm, yapHashEntry *entry)
{
    yapValueMark(vm, entry->value);
}

void yapObjectMark(yapVM *vm, yapObject *v)
{
    yapHashIterateP1(v->hash, (yapIterateCB1)yapHashValueMark, vm);
    if(v->isa)
        yapValueMark(vm, v->isa); // Is this necessary?
}

struct yapValue **yapObjectGetRef(struct yapVM *vm, yapObject *object, const char *key, yBool create)
{
    struct yapValue **ref = (struct yapValue **)yapHashLookup(object->hash, key, create);
    if(create)
    {
        if(*ref == NULL)
            *ref = yapValueNullPtr;
    }
    else if(!ref)
    {
        if(object->isa)
            return yapObjectGetRef(vm, object->isa->objectVal, key, create);
        ref = &yapValueNullPtr;
    }
    return ref;
}
