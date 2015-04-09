// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekValue.h"

#include "ekContext.h"
#include "ekFrame.h"
#include "ekMap.h"
#include "ekValueType.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// EVT_NULL Funcs

static char *NULL_STRING_FORM = "(null)";

static struct ekValue *nullFuncToString(struct ekContext *E, struct ekValue *p)
{
    return ekValueCreateKString(E, NULL_STRING_FORM);
}

static void nullFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    ekStringConcat(E, &params->output, NULL_STRING_FORM);
}

static ekU32 nullIterator(struct ekContext *E, ekU32 argCount)
{
    ekFrame *frame = ekArrayTop(E, &E->frames);
    ekAssert(frame->closure && frame->closure->closureVars);
    ekAssert(argCount == 0);
    ekContextPopValues(E, argCount);
    ekContextReturn(E, ekValueNullPtr);
}

static ekU32 nullCreateIterator(struct ekContext *E, ekU32 argCount)
{
    ekValue *thisValue = NULL;
    ekValue *closure;

    if(!ekContextGetArgs(E, argCount, "0", &thisValue))
    {
        return ekContextArgsFailure(E, argCount, "null iterator missing argument");
    }

    closure = ekValueCreateCFunction(E, nullIterator);
    closure->closureVars = ekMapCreate(E, EMKT_STRING);
    ekContextReturn(E, closure);
}

static ekCFunction *nullFuncIter(struct ekContext *E, struct ekValue *p)
{
    return nullCreateIterator;
}

void ekValueTypeRegisterNull(struct ekContext *E)
{
    ekValueType *type   = ekValueTypeCreate(E, "null", '0');
    type->funcToString   = nullFuncToString;
    type->funcDump       = nullFuncDump;
    type->funcIter       = nullFuncIter;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_NULL);
}
