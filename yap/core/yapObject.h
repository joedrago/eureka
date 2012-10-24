// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPOBJECT_H
#define YAPOBJECT_H

#include "yapContext.h"

// ---------------------------------------------------------------------------
// Forwards

struct yap2Hash;
struct yapValue;

// ---------------------------------------------------------------------------

typedef struct yapObject
{
    struct yapValue *isa; // "is a": what this object is derived from. Can be NULL.
    struct yap2Hash *hash;
} yapObject;

yapObject *yapObjectCreate(struct yapContext *Y, struct yapValue *isa);
void yapObjectDestroy(struct yapContext *Y, yapObject *v);

struct yapValue **yapObjectGetRef(struct yapContext *Y, yapObject *object, const char *key, yBool create);

#endif
