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
#include "ekObject.h"
#include "ekValueType.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// EVT_OBJECT Funcs

static void objectFuncClear(struct ekContext * E, struct ekValue * p)
{
    ekObjectDestroy(E, p->objectVal);
}

static void objectFuncClone(struct ekContext * E, struct ekValue * dst, struct ekValue * src)
{
    ekAssert(0 && "objectFuncClone not implemented");
}

static void objectFuncMark(struct ekContext * E, struct ekValue * p)
{
    ekObjectMark(E, p->objectVal);
}

static ekBool objectFuncToBool(struct ekContext * E, struct ekValue * p)
{
    return ekTrue;
}

static ekS32 objectFuncToInt(struct ekContext * E, struct ekValue * p)
{
    return 1; // ?
}

static ekF32 objectFuncToFloat(struct ekContext * E, struct ekValue * p)
{
    return 1.0f; // ?
}

static struct ekValue * objectFuncToString(struct ekContext * E, struct ekValue * p)
{
    char temp[32];
    sprintf(temp, "[object:%p]", p->objectVal);
    return ekValueCreateString(E, temp);
}

static ekU32 objectIterator(struct ekContext * E, ekU32 argCount)
{
    ekFrame * frame = ekArrayTop(E, &E->frames);
    ekValue * m;
    ekValue * keys;
    ekValue * index;
    ekValue * v;
    ekAssert(frame->closure && frame->closure->closureVars);
    m = ekMapGetS2P(E, frame->closure->closureVars, "map");
    keys = ekMapGetS2P(E, frame->closure->closureVars, "keys");
    index = ekMapGetS2P(E, frame->closure->closureVars, "index");
    ekAssert((m->type == EVT_OBJECT) && (keys->type == EVT_ARRAY) && (index->type == EVT_INT));
    ekAssert(argCount == 0);
    ekContextPopValues(E, argCount);

    if (index->intVal < ekArraySize(E, &keys->arrayVal)) {
        ekValue * key = keys->arrayVal[index->intVal++];
        ekArrayPush(E, &E->stack, key);
        v = ekValueIndex(E, m, key, ekFalse); // should addref the value
        if (!v) {
            v = ekValueNullPtr;
        }
        ekArrayPush(E, &E->stack, v);
        return 2;
    }

    ekArrayPush(E, &E->stack, ekValueNullPtr);
    return 1;
}

static void ekAppendKey(struct ekContext * E, ekValue * arrayVal, ekMapEntry * entry)
{
    ekValue * keyVal = ekValueCreateString(E, entry->keyStr);
    ekValueArrayPush(E, arrayVal, keyVal);
}

static ekU32 keys(struct ekContext * E, ekU32 argCount)
{
    ekValue * object;
    ekValue * arrayVal = ekValueCreateArray(E);

    if (!ekContextGetArgs(E, argCount, "o", &object)) {
        return ekContextArgsFailure(E, argCount, "keys([map/object] o)");
    }

    ekMapIterateP1(E, object->objectVal->hash, ekAppendKey, arrayVal);

    ekArrayPush(E, &E->stack, arrayVal);
    return 1;
}

static ekU32 objectCreateIterator(struct ekContext * E, ekU32 argCount)
{
    ekValue * m = NULL;
    ekValue * closure;
    ekValue * keys;
    if (!ekContextGetArgs(E, argCount, "o", &m)) {
        return ekContextArgsFailure(E, argCount, "pairs(map)");
    }
    closure = ekValueCreateCFunction(E, objectIterator);
    closure->closureVars = ekMapCreate(E, EMKT_STRING);
    keys = ekValueCreateArray(E);
    ekMapIterateP1(E, m->objectVal->hash, ekAppendKey, keys);
    ekMapGetS2P(E, closure->closureVars, "map") = m;
    ekMapGetS2P(E, closure->closureVars, "keys") = keys;
    ekMapGetS2P(E, closure->closureVars, "index") = ekValueCreateInt(E, 0);
    ekArrayPush(E, &E->stack, closure);
    return 1;
}

static ekCFunction * objectFuncIter(struct ekContext * E, struct ekValue * p)
{
    return objectCreateIterator;
}

static struct ekValue * objectFuncIndex(struct ekContext * E, struct ekValue * value, struct ekValue * index, ekBool lvalue)
{
    ekValue * ret = NULL;
    ekValue ** ref = NULL;
    index = ekValueToString(E, index);
    ref = ekObjectGetRef(E, value->objectVal, ekStringSafePtr(&index->stringVal), lvalue /* create? */);
    if (lvalue) {
        ret = ekValueCreateRef(E, ref);
    } else {
        ret = *ref;
    }
    return ret;
}

void appendKeys(struct ekContext * E, ekDumpParams * params, ekMapEntry * entry)
{
    ekValue * child = (ekValue *)entry->valuePtr;
    if (params->tempInt) {
        ekStringConcat(E, &params->output, "\"");
    } else {
        ekStringConcat(E, &params->output, ", \"");
    }

    ekStringConcat(E, &params->output, entry->keyStr);
    ekStringConcat(E, &params->output, "\" : ");
    ekValueTypeSafeCall(child->type, Dump) (E, params, child);
    params->tempInt = 0;
}

static void objectFuncDump(struct ekContext * E, ekDumpParams * params, struct ekValue * p)
{
    params->tempInt = 1;
    ekStringConcat(E, &params->output, "{ ");
    ekMapIterateP1(E, p->objectVal->hash, appendKeys, params);
    ekStringConcat(E, &params->output, " }");
}

// ---------------------------------------------------------------------------

static ekU32 inherit(struct ekContext * E, ekU32 argCount)
{
    ekValue * v;
    v = ekValueCreateObject(E, NULL, argCount, ekTrue);
    ekArrayPush(E, &E->stack, v);
    return 1;
}

static ekU32 prototype(struct ekContext * E, ekU32 argCount)
{
    ekValue * object = NULL;
    ekValue * newPrototype = NULL;
    if (!ekContextGetArgs(E, argCount, "o|o", &object, &newPrototype)) {
        return ekContextArgsFailure(E, argCount, "prototype(object [, newPrototypetype])");
    }

    if (object && newPrototype) {
        object->objectVal->prototype = newPrototype;
    }

    if (object && object->objectVal->prototype) {
        ekArrayPush(E, &E->stack, object->objectVal->prototype);
    } else {
        ekArrayPush(E, &E->stack, &ekValueNull);
    }
    return 1;
}

void ekValueTypeRegisterObject(struct ekContext * E)
{
    ekValueType * type = ekValueTypeCreate(E, "object", 'o');
    type->funcClear      = objectFuncClear;
    type->funcClone      = objectFuncClone;
    type->funcMark       = objectFuncMark;
    type->funcToBool     = objectFuncToBool;
    type->funcToInt      = objectFuncToInt;
    type->funcToFloat    = objectFuncToFloat;
    type->funcToString   = objectFuncToString;
    type->funcIter       = objectFuncIter;
    type->funcIndex      = objectFuncIndex;
    type->funcDump       = objectFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_OBJECT);

    ekContextAddIntrinsic(E, "keys", keys);
    ekContextAddIntrinsic(E, "inherit", inherit);
    ekContextAddIntrinsic(E, "prototype", prototype);
}
