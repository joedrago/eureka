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
// EVT_REF Funcs

static void refFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    dst->refVal = src->refVal;
}

static ekBool refFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return (*p->refVal) ? ekTrue : ekFalse;
}

static ekS32 refFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return 1; // ?
}

static ekF32 refFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return 1.0f; // ?
}

static void refFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    ekStringConcat(E, &params->output, "(ref: ");
    ekValueTypeSafeCall((*p->refVal)->type, Dump)(E, params, *p->refVal);
    ekStringConcat(E, &params->output, ")");
}

void ekValueTypeRegisterRef(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "ref", 0);
    type->funcClone      = refFuncClone;
    type->funcToBool     = refFuncToBool;
    type->funcToInt      = refFuncToInt;
    type->funcToFloat    = refFuncToFloat;
    type->funcDump       = refFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_REF);
}
