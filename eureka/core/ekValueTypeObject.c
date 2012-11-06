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
// EVT_OBJECT Funcs

static void objectFuncClear(struct ekContext *E, struct ekValue *p)
{
    ekObjectDestroy(E, p->objectVal);
}

static void objectFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    ekAssert(0 && "objectFuncClone not implemented");
}

static ekBool objectFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return ekTrue;
}

static ekS32 objectFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return 1; // ?
}

static ekF32 objectFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return 1.0f; // ?
}

static struct ekValue *objectFuncToString(struct ekContext *E, struct ekValue *p)
{
    char temp[32];
    sprintf(temp, "[object:%p]", p->objectVal);
    return ekValueCreateString(E, temp);
}

static struct ekValue *objectFuncIndex(struct ekContext *E, struct ekValue *value, struct ekValue *index, ekBool lvalue)
{
    ekValue *ret = NULL;
    ekValue **ref = NULL;
    index = ekValueToString(E, index);
    ref = ekObjectGetRef(E, value->objectVal, ekStringSafePtr(&index->stringVal), lvalue /* create? */);
    if(lvalue)
    {
        ret = ekValueCreateRef(E, ref);
    }
    else
    {
        ret = *ref;
        ekValueAddRefNote(E, ret, "objectFuncIndex");
    }
    return ret;
}

void appendKeys(struct ekContext *E, ekDumpParams *params, ekMapEntry *entry)
{
    ekValue *child = (ekValue *)entry->valuePtr;
    if(params->tempInt)
    {
        ekStringConcat(E, &params->output, "\"");
    }
    else
    {
        ekStringConcat(E, &params->output, ", \"");
    }

    ekStringConcat(E, &params->output, entry->keyStr);
    ekStringConcat(E, &params->output, "\" : ");
    ekValueTypeSafeCall(child->type, Dump)(E, params, child);
    params->tempInt = 0;
}

static void objectFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    params->tempInt = 1;
    ekStringConcat(E, &params->output, "{ ");
    ekMapIterateP1(E, p->objectVal->hash, appendKeys, params);
    ekStringConcat(E, &params->output, " }");
}

void ekValueTypeRegisterObject(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "object");
    type->funcClear      = objectFuncClear;
    type->funcClone      = objectFuncClone;
    type->funcToBool     = objectFuncToBool;
    type->funcToInt      = objectFuncToInt;
    type->funcToFloat    = objectFuncToFloat;
    type->funcToString   = objectFuncToString;
    type->funcArithmetic = ekValueTypeFuncNotUsed;
    type->funcCmp        = ekValueTypeFuncNotUsed;
    type->funcIndex      = objectFuncIndex;
    type->funcDump       = objectFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_OBJECT);
}
