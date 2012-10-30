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

static yU32 object(struct ekContext *Y, yU32 argCount)
{
    ekValue *v;
    v = ekValueCreateObject(Y, NULL, argCount, yFalse);
    ekArrayPush(Y, &Y->stack, v);
    return 1;
}

static yU32 inherit(struct ekContext *Y, yU32 argCount)
{
    ekValue *v;
    v = ekValueCreateObject(Y, NULL, argCount, yTrue);
    ekArrayPush(Y, &Y->stack, v);
    return 1;
}

static yU32 prototype(struct ekContext *Y, yU32 argCount)
{
    ekValue *object = NULL;
    ekValue *newPrototype = NULL;
    if(!ekContextGetArgs(Y, argCount, "o|o", &object, &newPrototype))
    {
        return ekContextArgsFailure(Y, argCount, "prototype(object [, newPrototypetype])");
    }

    if(object && newPrototype)
    {
        if(object->objectVal->isa)
        {
            ekValueRemoveRefNote(Y, object->objectVal->isa, "prototype removing old isa");
        }
        object->objectVal->isa = newPrototype;
        ekValueAddRefNote(Y, object->objectVal->isa, "prototype new isa");
    }

    if(object && object->objectVal->isa)
    {
        ekValueAddRefNote(Y, object->objectVal->isa, "prototype return isa");
        ekArrayPush(Y, &Y->stack, object->objectVal->isa);
    }
    else
    {
        ekArrayPush(Y, &Y->stack, &ekValueNull);
    }
    ekValueRemoveRefNote(Y, object, "prototype object done");
    if(newPrototype)
    {
        ekValueRemoveRefNote(Y, newPrototype, "prototype newPrototype done");
    }
    return 1;
}

// ---------------------------------------------------------------------------

void ekIntrinsicsRegisterInheritance(struct ekContext *Y)
{
    ekContextRegisterGlobalFunction(Y, "object", object);
    ekContextRegisterGlobalFunction(Y, "dict", object); // alias

    ekContextRegisterGlobalFunction(Y, "inherit", inherit);
    ekContextRegisterGlobalFunction(Y, "prototype", prototype);
}

