// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapObject.h"

#include "yapTypes.h"
#include "yapMap.h"
#include "yapValue.h"
#include "yapContext.h"

yapObject *yapObjectCreate(struct yapContext *Y, yapValue *isa)
{
    yapObject *v = (yapObject *)yapAlloc(sizeof(yapObject));
    v->isa = isa;
    if(v->isa)
    {
        yapValueAddRefNote(Y, v->isa, "yapObject isa");
    }
    v->hash = yapMapCreate(Y, YMKT_STRING);
    return v;
}

void yapObjectDestroy(struct yapContext *Y, yapObject *v)
{
    if(v->isa)
    {
        yapValueRemoveRefNote(Y, v->isa, "yapObject isa done");
    }
    yapMapDestroy(Y, v->hash, yapValueRemoveRefHashed);
    yapFree(v);
}

struct yapValue **yapObjectGetRef(struct yapContext *Y, yapObject *object, const char *key, yBool create)
{
    struct yapValue **ref = NULL;
    yapMapEntry *hashEntry = yapMapGetS(Y, object->hash, key, create);
    if(hashEntry)
    {
        ref = (struct yapValue **)&hashEntry->valuePtr;
    }
    
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
