// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapiConversions.h"

#include "yapTypes.h"
#include "yapHash.h"
#include "yapObject.h"
#include "yapValue.h"
#include "yapContext.h"

#include <stdio.h>

// ---------------------------------------------------------------------------

static yU32 convert_to_string(struct yapContext *Y, yU32 argCount)
{
    yapValue *v = NULL;
    if(!yapContextGetArgs(Y, argCount, "?", &v))
    {
        return yapContextArgsFailure(Y, argCount, "string(value)");
    }

    yapArrayPush(Y, &Y->stack, yapValueToString(Y, v));
    return 1;
}

static yU32 convert_to_int(struct yapContext *Y, yU32 argCount)
{
    yapValue *v = NULL;
    if(!yapContextGetArgs(Y, argCount, "?", &v))
    {
        return yapContextArgsFailure(Y, argCount, "int(value)");
    }

    yapArrayPush(Y, &Y->stack, yapValueToInt(Y, v));
    return 1;
}

static yU32 convert_to_float(struct yapContext *Y, yU32 argCount)
{
    yapValue *v = NULL;
    if(!yapContextGetArgs(Y, argCount, "?", &v))
    {
        return yapContextArgsFailure(Y, argCount, "float(value)");
    }

    yapArrayPush(Y, &Y->stack, yapValueToFloat(Y, v));
    return 1;
}

// ---------------------------------------------------------------------------

void yapIntrinsicsRegisterConversions(struct yapContext *Y)
{
    yapContextRegisterGlobalFunction(Y, "string", convert_to_string);
    yapContextRegisterGlobalFunction(Y, "int", convert_to_int);
    yapContextRegisterGlobalFunction(Y, "float", convert_to_float);
}
