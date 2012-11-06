// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekiInheritance.h"

#include "ekTypes.h"
#include "ekMap.h"
#include "ekObject.h"
#include "ekValue.h"
#include "ekContext.h"

#include <stdio.h>

// ---------------------------------------------------------------------------

static ekU32 object(struct ekContext *E, ekU32 argCount)
{
    ekValue *v;
    v = ekValueCreateObject(E, NULL, argCount, ekFalse);
    ekArrayPush(E, &E->stack, v);
    return 1;
}

static ekU32 inherit(struct ekContext *E, ekU32 argCount)
{
    ekValue *v;
    v = ekValueCreateObject(E, NULL, argCount, ekTrue);
    ekArrayPush(E, &E->stack, v);
    return 1;
}

static ekU32 prototype(struct ekContext *E, ekU32 argCount)
{
    ekValue *object = NULL;
    ekValue *newPrototype = NULL;
    if(!ekContextGetArgs(E, argCount, "o|o", &object, &newPrototype))
    {
        return ekContextArgsFailure(E, argCount, "prototype(object [, newPrototypetype])");
    }

    if(object && newPrototype)
    {
        if(object->objectVal->isa)
        {
            ekValueRemoveRefNote(E, object->objectVal->isa, "prototype removing old isa");
        }
        object->objectVal->isa = newPrototype;
        ekValueAddRefNote(E, object->objectVal->isa, "prototype new isa");
    }

    if(object && object->objectVal->isa)
    {
        ekValueAddRefNote(E, object->objectVal->isa, "prototype return isa");
        ekArrayPush(E, &E->stack, object->objectVal->isa);
    }
    else
    {
        ekArrayPush(E, &E->stack, &ekValueNull);
    }
    ekValueRemoveRefNote(E, object, "prototype object done");
    if(newPrototype)
    {
        ekValueRemoveRefNote(E, newPrototype, "prototype newPrototype done");
    }
    return 1;
}

// ---------------------------------------------------------------------------

void ekIntrinsicsRegisterInheritance(struct ekContext *E)
{
    ekContextRegisterGlobalFunction(E, "object", object);
    ekContextRegisterGlobalFunction(E, "map", object); // alias

    ekContextRegisterGlobalFunction(E, "inherit", inherit);
    ekContextRegisterGlobalFunction(E, "prototype", prototype);
}

