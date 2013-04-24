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
// EVT_BLOCK Funcs

static void blockFuncClear(struct ekContext *E, struct ekValue *p)
{
    if(p->closureVars)
    {
        ekMapDestroy(E, p->closureVars, ekValueRemoveRef);
    }
}

static void blockFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    dst->blockVal = src->blockVal;
}

static ekBool blockFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return ekTrue;
}

static ekS32 blockFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return 1; // ?
}

static ekF32 blockFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return 1.0f; // ?
}

static void blockFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    char temp[64];
    sprintf(temp, "(block:0x%p)", p->blockVal);
    ekStringConcat(E, &params->output, temp);
}

void ekValueTypeRegisterBlock(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "block");
    type->funcClear      = blockFuncClear;
    type->funcClone      = blockFuncClone;
    type->funcToBool     = blockFuncToBool;
    type->funcToInt      = blockFuncToInt;
    type->funcToFloat    = blockFuncToFloat;
    type->funcDump       = blockFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_BLOCK);
}
