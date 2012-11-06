// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekObject.h"

#include "ekTypes.h"
#include "ekMap.h"
#include "ekValue.h"
#include "ekContext.h"

ekObject *ekObjectCreate(struct ekContext *E, ekValue *isa)
{
    ekObject *v = (ekObject *)ekAlloc(sizeof(ekObject));
    v->isa = isa;
    if(v->isa)
    {
        ekValueAddRefNote(E, v->isa, "ekObject isa");
    }
    v->hash = ekMapCreate(E, YMKT_STRING);
    return v;
}

void ekObjectDestroy(struct ekContext *E, ekObject *v)
{
    if(v->isa)
    {
        ekValueRemoveRefNote(E, v->isa, "ekObject isa done");
    }
    ekMapDestroy(E, v->hash, ekValueRemoveRefHashed);
    ekFree(v);
}

struct ekValue **ekObjectGetRef(struct ekContext *E, ekObject *object, const char *key, ekBool create)
{
    struct ekValue **ref = NULL;
    ekMapEntry *hashEntry = ekMapGetS(E, object->hash, key, create);
    if(hashEntry)
    {
        ref = (struct ekValue **)&hashEntry->valuePtr;
    }

    if(create)
    {
        if(*ref == NULL)
        {
            *ref = ekValueNullPtr;
        }
    }
    else if(!ref)
    {
        if(object->isa)
        {
            return ekObjectGetRef(E, object->isa->objectVal, key, create);
        }
        ref = &ekValueNullPtr;
    }
    return ref;
}
