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
// EVT_INT Funcs

static void boolFuncClone(struct ekContext * E, struct ekValue * dst, struct ekValue * src)
{
    dst->intVal = src->intVal;
}

static ekBool boolFuncToBool(struct ekContext * E, struct ekValue * p)
{
    return p->boolVal;
}

static ekS32 boolFuncToInt(struct ekContext * E, struct ekValue * p)
{
    return (p->boolVal) ? 1 : 0;
}

static ekF32 boolFuncToFloat(struct ekContext * E, struct ekValue * p)
{
    return (p->boolVal) ? 1.0f : 0.0f;
}

static struct ekValue * boolFuncToString(struct ekContext * E, struct ekValue * p)
{
    const char * temp = "false";
    if (p->boolVal) {
        temp = "true";
    }
    ekValueRemoveRefNote(E, p, "boolFuncToString doesnt need int anymore");
    return ekValueCreateString(E, temp);
}

static ekBool boolFuncCmp(struct ekContext * E, struct ekValue * a, struct ekValue * b, ekS32 * cmpResult)
{
    if (b->type == EVT_BOOL) {
        *cmpResult = a->boolVal - b->boolVal;
        return ekTrue;
    }
    return ekFalse;
}

static void boolFuncDump(struct ekContext * E, ekDumpParams * params, struct ekValue * p)
{
    const char * temp = "false";
    if (p->boolVal) {
        temp = "true";
    }
    ekStringConcat(E, &params->output, temp);
}

static ekU32 ekiBool(struct ekContext * E, ekU32 argCount)
{
    ekValue * v = NULL;
    if (!ekContextGetArgs(E, argCount, "?", &v)) {
        return ekContextArgsFailure(E, argCount, "bool(value)");
    }

    ekArrayPush(E, &E->stack, ekValueToBool(E, v));
    return 1;
}

void ekValueTypeRegisterBool(struct ekContext * E)
{
    ekValueType * type = ekValueTypeCreate(E, "bool", 'b');
    type->funcClone      = boolFuncClone;
    type->funcToBool     = boolFuncToBool;
    type->funcToInt      = boolFuncToInt;
    type->funcToFloat    = boolFuncToFloat;
    type->funcToString   = boolFuncToString;
    type->funcCmp        = boolFuncCmp;
    type->funcDump       = boolFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_BOOL);

    ekContextAddIntrinsic(E, "bool", ekiBool);
}
