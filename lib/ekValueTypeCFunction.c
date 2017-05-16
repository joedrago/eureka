// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekValue.h"

#include "ekContext.h"
#include "ekMap.h"
#include "ekValueType.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// EVT_CFUNCTION Funcs

static void cfunctionFuncClear(struct ekContext * E, struct ekValue * p)
{
    if (p->closureVars) {
        ekMapDestroy(E, p->closureVars, NULL);
    }
}

static void cfunctionFuncClone(struct ekContext * E, struct ekValue * dst, struct ekValue * src)
{
    dst->cFuncVal = src->cFuncVal;
}

static void ekMapEntryValueMark(struct ekContext * Y, void * ignored, ekMapEntry * entry)
{
    ekValueMark(Y, entry->valuePtr);
}

static void cfunctionFuncMark(struct ekContext * Y, struct ekValue * value)
{
    if (value->closureVars) {
        ekMapIterateP1(Y, value->closureVars, ekMapEntryValueMark, NULL);
    }
}

static ekBool cfunctionFuncToBool(struct ekContext * E, struct ekValue * p)
{
    return ekTrue;
}

static ekS32 cfunctionFuncToInt(struct ekContext * E, struct ekValue * p)
{
    return 1; // ?
}

static ekF32 cfunctionFuncToFloat(struct ekContext * E, struct ekValue * p)
{
    return 1.0f; // ?
}

static void cfunctionFuncDump(struct ekContext * E, ekDumpParams * params, struct ekValue * p)
{
    char temp[64];
    sprintf(temp, "(cfunction:0x%p)", p->cFuncVal);
    ekStringConcat(E, &params->output, temp);
}

void ekValueTypeRegisterCFunction(struct ekContext * E)
{
    ekValueType * type = ekValueTypeCreate(E, "cfunction", 0);
    type->funcClear      = cfunctionFuncClear;
    type->funcClone      = cfunctionFuncClone;
    type->funcMark       = cfunctionFuncMark;
    type->funcToBool     = cfunctionFuncToBool;
    type->funcToInt      = cfunctionFuncToInt;
    type->funcToFloat    = cfunctionFuncToFloat;
    type->funcDump       = cfunctionFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_CFUNCTION);
}
