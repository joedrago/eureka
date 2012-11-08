// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekValue.h"

#include "ekFrame.h"
#include "ekMap.h"
#include "ekObject.h"
#include "ekLexer.h"
#include "ekContext.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// EVT_CFUNCTION Funcs

static void cfunctionFuncClear(struct ekContext *E, struct ekValue *p)
{
    if(p->closureVars)
    {
        ekMapDestroy(E, p->closureVars, ekValueRemoveRef);
    }
}

static void cfunctionFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    dst->cFuncVal = src->cFuncVal;
}

static ekBool cfunctionFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return ekTrue;
}

static ekS32 cfunctionFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return 1; // ?
}

static ekF32 cfunctionFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return 1.0f; // ?
}

static void cfunctionFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    char temp[64];
    sprintf(temp, "(cfunction:0x%p)", p->cFuncVal);
    ekStringConcat(E, &params->output, temp);
}

void ekValueTypeRegisterCFunction(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "cfunction");
    type->funcClear      = cfunctionFuncClear;
    type->funcClone      = cfunctionFuncClone;
    type->funcToBool     = cfunctionFuncToBool;
    type->funcToInt      = cfunctionFuncToInt;
    type->funcToFloat    = cfunctionFuncToFloat;
    type->funcToString   = ekValueTypeFuncNotUsed;
    type->funcIter       = ekValueTypeFuncNotUsed;
    type->funcArithmetic = ekValueTypeFuncNotUsed;
    type->funcCmp        = ekValueTypeFuncNotUsed;
    type->funcIndex      = ekValueTypeFuncNotUsed;
    type->funcDump       = cfunctionFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_CFUNCTION);
}
