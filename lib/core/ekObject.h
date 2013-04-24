// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKOBJECT_H
#define EKOBJECT_H

#include "ekContext.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekMap;
struct ekValue;

// ---------------------------------------------------------------------------

typedef struct ekObject
{
    struct ekValue *isa; // "is a": what this object is derived from. Can be NULL.
    struct ekMap *hash;
} ekObject;

ekObject *ekObjectCreate(struct ekContext *E, struct ekValue *isa);
void ekObjectDestroy(struct ekContext *E, ekObject *v);

struct ekValue **ekObjectGetRef(struct ekContext *E, ekObject *object, const char *key, ekBool create);

#endif
