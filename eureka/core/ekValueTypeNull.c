// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekValue.h"

#include "ekContext.h"
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

void ekValueTypeRegisterNull(struct ekContext *E)
{
    ekValueType *type   = ekValueTypeCreate(E, "null");
    type->funcClear      = ekValueTypeFuncNotUsed;
    type->funcClone      = ekValueTypeFuncNotUsed;
    type->funcToBool     = ekValueTypeFuncNotUsed;
    type->funcToInt      = ekValueTypeFuncNotUsed;
    type->funcToFloat    = ekValueTypeFuncNotUsed;
    type->funcToString   = nullFuncToString;
    type->funcIter       = ekValueTypeFuncNotUsed;
    type->funcArithmetic = ekValueTypeFuncNotUsed;
    type->funcCmp        = ekValueTypeFuncNotUsed;
    type->funcIndex      = ekValueTypeFuncNotUsed;
    type->funcDump       = nullFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_NULL);
}
