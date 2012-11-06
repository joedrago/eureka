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

static ekU32 convert_to_string(struct ekContext *E, ekU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(E, argCount, "?", &v))
    {
        return ekContextArgsFailure(E, argCount, "string(value)");
    }

    ekArrayPush(E, &E->stack, ekValueToString(E, v));
    return 1;
}

static ekU32 convert_to_int(struct ekContext *E, ekU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(E, argCount, "?", &v))
    {
        return ekContextArgsFailure(E, argCount, "int(value)");
    }

    ekArrayPush(E, &E->stack, ekValueToInt(E, v));
    return 1;
}

static ekU32 convert_to_float(struct ekContext *E, ekU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(E, argCount, "?", &v))
    {
        return ekContextArgsFailure(E, argCount, "float(value)");
    }

    ekArrayPush(E, &E->stack, ekValueToFloat(E, v));
    return 1;
}

// ---------------------------------------------------------------------------

void ekIntrinsicsRegisterConversions(struct ekContext *E)
{
    ekContextRegisterGlobalFunction(E, "string", convert_to_string);
    ekContextRegisterGlobalFunction(E, "int", convert_to_int);
    ekContextRegisterGlobalFunction(E, "float", convert_to_float);
}
