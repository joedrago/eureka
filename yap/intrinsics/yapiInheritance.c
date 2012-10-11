// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapiInheritance.h"

#include "yapTypes.h"
#include "yapHash.h"
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
    yapObject *object = NULL;
    yapValue *newProto = NULL;
    yBool replaceProto = yFalse;
    if(argCount > 0)
    {
        yapValue *arg = yapContextGetArg(Y, 0, argCount);
        if(arg->type == YVT_REF)
            arg = *arg->refVal;
        if(arg->type == YVT_OBJECT)
            object = arg->objectVal;
    }
    if(object && (argCount > 1))
    {
        yapValue *arg = yapContextGetArg(Y, 1, argCount);
        if(arg->type == YVT_REF)
            arg = *arg->refVal;
        if(arg->type == YVT_OBJECT)
            newProto = arg;
        else if(arg->type == YVT_NULL)
            newProto = NULL;
        else
        {
            yapContextSetError(Y, YVE_RUNTIME, "prototype(): replacement prototype must be object or null");
            newProto = NULL;
        }
        replaceProto = yTrue;
    }

    yapContextPopValues(Y, argCount);

    if(object && replaceProto)
    {
        object->isa = newProto;
    }

    if(object && object->isa)
    {
        yapArrayPush(Y, &Y->stack, object->isa);
    }
    else
    {
        yapArrayPush(Y, &Y->stack, &yapValueNull);
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

