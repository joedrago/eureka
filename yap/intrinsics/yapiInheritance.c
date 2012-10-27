// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapiInheritance.h"

#include "yapTypes.h"
#include "yapMap.h"
#include "yapObject.h"
#include "yapValue.h"
#include "yapContext.h"

#include <stdio.h>

// ---------------------------------------------------------------------------

static yU32 object(struct yapContext *Y, yU32 argCount)
{
    yapValue *v;
    v = yapValueCreateObject(Y, NULL, argCount, yFalse);
    yapArrayPush(Y, &Y->stack, v);
    return 1;
}

static yU32 inherit(struct yapContext *Y, yU32 argCount)
{
    yapValue *v;
    v = yapValueCreateObject(Y, NULL, argCount, yTrue);
    yapArrayPush(Y, &Y->stack, v);
    return 1;
}

static yU32 prototype(struct yapContext *Y, yU32 argCount)
{
    yapValue *object = NULL;
    yapValue *newPrototype = NULL;
    if(!yapContextGetArgs(Y, argCount, "o|o", &object, &newPrototype))
    {
        return yapContextArgsFailure(Y, argCount, "prototype(object [, newPrototypetype])");
    }

    if(object && newPrototype)
    {
        if(object->objectVal->isa)
        {
            yapValueRemoveRefNote(Y, object->objectVal->isa, "prototype removing old isa");
        }
        object->objectVal->isa = newPrototype;
        yapValueAddRefNote(Y, object->objectVal->isa, "prototype new isa");
    }

    if(object && object->objectVal->isa)
    {
        yapValueAddRefNote(Y, object->objectVal->isa, "prototype return isa");
        yapArrayPush(Y, &Y->stack, object->objectVal->isa);
    }
    else
    {
        yapArrayPush(Y, &Y->stack, &yapValueNull);
    }
    yapValueRemoveRefNote(Y, object, "prototype object done");
    if(newPrototype)
    {
        yapValueRemoveRefNote(Y, newPrototype, "prototype newPrototype done");
    }
    return 1;
}

// ---------------------------------------------------------------------------

void yapIntrinsicsRegisterInheritance(struct yapContext *Y)
{
    yapContextRegisterGlobalFunction(Y, "object", object);
    yapContextRegisterGlobalFunction(Y, "dict", object); // alias

    yapContextRegisterGlobalFunction(Y, "inherit", inherit);
    yapContextRegisterGlobalFunction(Y, "prototype", prototype);
}

