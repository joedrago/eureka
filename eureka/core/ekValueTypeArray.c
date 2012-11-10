// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekValue.h"

#include "ekContext.h"
#include "ekFrame.h"
#include "ekLexer.h"
#include "ekMap.h"
#include "ekObject.h"
#include "ekValueType.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// EVT_ARRAY Intrinsics

static ekU32 arrayIntrinsicLength(struct ekContext *E, ekU32 argCount)
{
    ekValue *a = ekContextThis(E);
    ekValue *c = ekValueNullPtr;
    ekAssert(a && a->type == EVT_ARRAY);
    if(argCount)
    {
        return ekContextArgsFailure(E, argCount, "array.length() takes zero arguments");
    }

    c = ekValueCreateInt(E, ekArraySize(E, &a->arrayVal));
    ekValueRemoveRefNote(E, a, "length this done");
    ekArrayPush(E, &E->stack, c);
    return 1;
}

ekU32 arrayIntrinsicPush(struct ekContext *E, ekU32 argCount)
{
    int i;
    ekValue *a = ekContextThis(E);
    ekValue **values = NULL;
    if(!ekContextGetArgs(E, argCount, ".", &values))
    {
        return ekContextArgsFailure(E, argCount, "array.push(value [, ... value])");
    }

    for(i=0; i<ekArraySize(E, &values); i++)
    {
        ekValue *v = (ekValue *)values[i];
        ekValueArrayPush(E, a, v);
    }

    ekValueRemoveRefNote(E, a, "array_push a done");
    ekArrayDestroy(E, &values, NULL);
    return 0;
}

// ---------------------------------------------------------------------------
// EVT_ARRAY Funcs

static void arrayFuncClear(struct ekContext *E, struct ekValue *p)
{
    ekArrayDestroy(E, &p->arrayVal, (ekDestroyCB)ekValueRemoveRefHashed);
}

static void arrayFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    ekAssert(0 && "arrayFuncClone not implemented");
}

static ekBool arrayFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return (p->arrayVal && ekArraySize(E, &p->arrayVal)) ? ekTrue : ekFalse;
}

static ekS32 arrayFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return (p->arrayVal) ? ekArraySize(E, &p->arrayVal) : 0;
}

static ekF32 arrayFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return (p->arrayVal) ? (ekF32)ekArraySize(E, &p->arrayVal) : 0;
}

static ekU32 arrayIterator(struct ekContext *E, ekU32 argCount)
{
    ekFrame *frame = ekArrayTop(E, &E->frames);
    ekValue *array;
    ekValue *index;
    ekAssert(frame->closure && frame->closure->closureVars);
    array = ekMapGetS2P(E, frame->closure->closureVars, "array");
    index = ekMapGetS2P(E, frame->closure->closureVars, "index");
    ekAssert((array->type == EVT_ARRAY) && (index->type == EVT_INT));
    ekAssert(argCount == 0);
    ekContextPopValues(E, argCount);

    if(index->intVal < ekArraySize(E, &array->arrayVal))
    {
        ekValue *v = array->arrayVal[index->intVal];
        ekValueAddRefNote(E, v, "array_iterator using value");
        ekArrayPush(E, &E->stack, v);
        ekArrayPush(E, &E->stack, ekValueCreateInt(E, index->intVal++));
        return 2;
    }

    ekArrayPush(E, &E->stack, ekValueNullPtr);
    return 1;
}

static ekU32 arrayCreateIterator(struct ekContext *E, ekU32 argCount)
{
    ekValue *a = NULL;
    ekValue *closure;
    if(!ekContextGetArgs(E, argCount, "a", &a))
    {
        return ekContextArgsFailure(E, argCount, "iter(array)");
    }
    closure = ekValueCreateCFunction(E, arrayIterator);
    closure->closureVars = ekMapCreate(E, EMKT_STRING);
    ekMapGetS2P(E, closure->closureVars, "array") = a;
    ekMapGetS2P(E, closure->closureVars, "index") = ekValueCreateInt(E, 0);
    ekArrayPush(E, &E->stack, closure);
    return 1;
}

static ekCFunction *arrayFuncIter(struct ekContext *E, struct ekValue *p)
{
    return arrayCreateIterator;
}

static struct ekValue *arrayFuncIndex(struct ekContext *E, struct ekValue *value, struct ekValue *index, ekBool lvalue)
{
    ekValue *ret = NULL;
    ekValue **ref = NULL;

    if(ret = ekValueTypeGetIntrinsic(E, value->type, index, lvalue)) // Intrinsic functions have priority over regular indexing
    {
        return ret;
    }
    if(ekContextGetError(E)) // An error might be set as a side effect of GetIntrinsic
    {
        return 0;
    }

    ekValueAddRefNote(E, index, "keep index around after int conversion");
    index = ekValueToInt(E, index);
    if(index->intVal >= 0 && index->intVal < ekArraySize(E, &value->arrayVal))
    {
        ref = (ekValue **) & (value->arrayVal[index->intVal]);
        if(lvalue)
        {
            ret = ekValueCreateRef(E, ref);
        }
        else
        {
            ret = *ref;
            ekValueAddRefNote(E, ret, "arrayFuncIndex");
        }
    }
    else
    {
        ekContextSetError(E, EVE_RUNTIME, "array index %d out of range", index->intVal);
    }
    ekValueRemoveRefNote(E, index, "temp index (int) done");
    return ret;
}

static void arrayFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    int i;
    ekStringConcat(E, &params->output, "[ ");
    for(i=0; i<ekArraySize(E, &p->arrayVal); i++)
    {
        ekValue *child = (ekValue *)p->arrayVal[i];
        if(i > 0)
        {
            ekStringConcat(E, &params->output, ", ");
        }
        ekValueTypeSafeCall(child->type, Dump)(E, params, child);
    }
    ekStringConcat(E, &params->output, " ]");
}

void ekValueTypeRegisterArray(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "array");
    type->funcClear      = arrayFuncClear;
    type->funcClone      = arrayFuncClone;
    type->funcToBool     = arrayFuncToBool;
    type->funcToInt      = arrayFuncToInt;
    type->funcToFloat    = arrayFuncToFloat;
    type->funcToString   = ekValueTypeFuncNotUsed;
    type->funcIter       = arrayFuncIter;
    type->funcArithmetic = ekValueTypeFuncNotUsed;
    type->funcCmp        = ekValueTypeFuncNotUsed;
    type->funcIndex      = arrayFuncIndex;
    type->funcDump       = arrayFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_ARRAY);

    ekValueTypeAddIntrinsic(E, type, "length", arrayIntrinsicLength);
    ekValueTypeAddIntrinsic(E, type, "push", arrayIntrinsicPush);
}