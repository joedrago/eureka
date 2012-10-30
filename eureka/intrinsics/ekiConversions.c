// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekiConversions.h"

#include "ekTypes.h"
#include "ekMap.h"
#include "ekObject.h"
#include "ekValue.h"
#include "ekContext.h"

#include <stdio.h>

// ---------------------------------------------------------------------------

static yU32 convert_to_string(struct ekContext *Y, yU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(Y, argCount, "?", &v))
    {
        return ekContextArgsFailure(Y, argCount, "string(value)");
    }

    ekArrayPush(Y, &Y->stack, ekValueToString(Y, v));
    return 1;
}

static yU32 convert_to_int(struct ekContext *Y, yU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(Y, argCount, "?", &v))
    {
        return ekContextArgsFailure(Y, argCount, "int(value)");
    }

    ekArrayPush(Y, &Y->stack, ekValueToInt(Y, v));
    return 1;
}

static yU32 convert_to_float(struct ekContext *Y, yU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(Y, argCount, "?", &v))
    {
        return ekContextArgsFailure(Y, argCount, "float(value)");
    }

    ekArrayPush(Y, &Y->stack, ekValueToFloat(Y, v));
    return 1;
}

// ---------------------------------------------------------------------------

void ekIntrinsicsRegisterConversions(struct ekContext *Y)
{
    ekContextRegisterGlobalFunction(Y, "string", convert_to_string);
    ekContextRegisterGlobalFunction(Y, "int", convert_to_int);
    ekContextRegisterGlobalFunction(Y, "float", convert_to_float);
}
