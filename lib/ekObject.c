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

ekObject * ekObjectCreate(struct ekContext * E, ekValue * prototype)
{
    ekObject * v = (ekObject *)ekAlloc(sizeof(ekObject));
    v->prototype = prototype;
    v->hash = ekMapCreate(E, EMKT_STRING);
    return v;
}

void ekObjectDestroy(struct ekContext * E, ekObject * v)
{
    ekMapDestroy(E, v->hash, NULL);
    ekFree(v);
}

static void ekMapValueMark(struct ekContext * E, void * ignored, ekMapEntry * entry)
{
    ekValueMark(E, entry->valuePtr);
}

void ekObjectMark(struct ekContext * E, ekObject * v)
{
    ekMapIterateP1(E, v->hash, ekMapValueMark, NULL);
    if (v->prototype) {
        ekValueMark(E, v->prototype); // Is this necessary?
    }
}

struct ekValue ** ekObjectGetRef(struct ekContext * E, ekObject * object, const char * key, ekBool create)
{
    struct ekValue ** ref = NULL;
    ekMapEntry * hashEntry = ekMapGetS(E, object->hash, key, create);
    if (hashEntry) {
        ref = (struct ekValue **)&hashEntry->valuePtr;
    }

    if (create) {
        if (*ref == NULL) {
            *ref = ekValueNullPtr;
        }
    } else if (!ref) {
        if (object->prototype) {
            return ekObjectGetRef(E, object->prototype->objectVal, key, create);
        }
        ref = &ekValueNullPtr;
    }
    return ref;
}
