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
// EVT_ARRAY Global Intrinsics

static ekU32 arrayIntrinsicPush(struct ekContext * E, ekU32 argCount)
{
    ekS32 i;
    ekValue * a = NULL;
    ekValue ** values = NULL;
    if (!ekContextGetArgs(E, argCount, "a.", &a, &values)) {
        return ekContextArgsFailure(E, argCount, "push([array] a [, ... value])");
    }

    for (i=0; i < ekArraySize(E, &values); i++) {
        ekValue * v = (ekValue *)values[i];
        ekValueArrayPush(E, a, v);
    }

    ekArrayDestroy(E, &values, NULL);
    return 0;
}

static ekU32 arrayIntrinsicPop(struct ekContext * E, ekU32 argCount)
{
    ekValue * a = NULL;
    ekValue * v;
    if (!ekContextGetArgs(E, argCount, "a", &a)) {
        return ekContextArgsFailure(E, argCount, "pop([array] a)");
    }

    if (ekArraySize(E, &a->arrayVal)) {
        v = ekArrayPop(E, &a->arrayVal);
    } else {
        v = ekValueNullPtr;
    }

    ekArrayPush(E, &E->stack, v);
    return 1;
}

static ekU32 arrayIntrinsicUnshift(struct ekContext * E, ekU32 argCount)
{
    ekS32 i;
    ekValue * a = NULL;
    ekValue ** values = NULL;
    if (!ekContextGetArgs(E, argCount, "a.", &a, &values)) {
        return ekContextArgsFailure(E, argCount, "unshift([array] a [, ... value])");
    }

    if (ekArraySize(E, &values)) {
        for (i = ekArraySize(E, &values) - 1; i >= 0; --i) {
            ekValue * v = (ekValue *)values[i];
            ekArrayUnshift(E, &a->arrayVal, v);
        }
    }

    ekArrayDestroy(E, &values, NULL);
    return 0;
}

static ekU32 arrayIntrinsicShift(struct ekContext * E, ekU32 argCount)
{
    ekValue * a = NULL;
    ekValue * v;
    if (!ekContextGetArgs(E, argCount, "a", &a)) {
        return ekContextArgsFailure(E, argCount, "shift([array] a)");
    }

    if (ekArraySize(E, &a->arrayVal)) {
        v = a->arrayVal[0];
        ekArrayErase(E, &a->arrayVal, 0);
    } else {
        v = ekValueNullPtr;
    }

    ekArrayPush(E, &E->stack, v);
    return 1;
}

static ekU32 arrayIntrinsicSort(struct ekContext * E, ekU32 argCount)
{
    ekValue * a = NULL;
    ekValue * newa;
    int i;
    int size;
    if (!ekContextGetArgs(E, argCount, "a", &a)) {
        return ekContextArgsFailure(E, argCount, "sort([array] a)");
    }

    // Let's do the dumbest stable insertion sort ever. We'll walk backwards
    // in our new array performing Cmp until we find a home, and then Insert.

    newa = ekValueCreateArray(E);
    size = ekArraySize(E, &a->arrayVal);
    for (i = 0; i < size; ++i) {
        int insertIndex = ekArraySize(E, &newa->arrayVal);
        for (; insertIndex > 0; --insertIndex) {
            if (ekValueCmp(E, newa->arrayVal[insertIndex - 1], a->arrayVal[i]) < 0) {
                break;
            }
        }
        ekArrayInsert(E, &newa->arrayVal, insertIndex, a->arrayVal[i]);
    }

    ekArrayPush(E, &E->stack, newa);
    return 1;
}

// ---------------------------------------------------------------------------
// EVT_ARRAY Funcs

static void arrayFuncClear(struct ekContext * E, struct ekValue * p)
{
    ekArrayDestroy(E, &p->arrayVal, NULL);
}

static void arrayFuncClone(struct ekContext * E, struct ekValue * dst, struct ekValue * src)
{
    ekAssert(0 && "arrayFuncClone not implemented");
}

static void arrayFuncMark(struct ekContext * E, struct ekValue * p)
{
    if (p->arrayVal) {
        int i;
        for (i = 0; i < ekArraySize(E, &p->arrayVal); i++) {
            ekValue * child = (ekValue *)p->arrayVal[i];
            ekValueMark(E, child);
        }
    }
}

static ekBool arrayFuncToBool(struct ekContext * E, struct ekValue * p)
{
    return (p->arrayVal && ekArraySize(E, &p->arrayVal)) ? ekTrue : ekFalse;
}

static ekS32 arrayFuncToInt(struct ekContext * E, struct ekValue * p)
{
    return (p->arrayVal) ? ekArraySize(E, &p->arrayVal) : 0;
}

static ekF32 arrayFuncToFloat(struct ekContext * E, struct ekValue * p)
{
    return (p->arrayVal) ? (ekF32)ekArraySize(E, &p->arrayVal) : 0;
}

static ekU32 arrayIterator(struct ekContext * E, ekU32 argCount)
{
    ekFrame * frame = ekArrayTop(E, &E->frames);
    ekValue * array;
    ekValue * index;
    ekAssert(frame->closure && frame->closure->closureVars);
    array = ekMapGetS2P(E, frame->closure->closureVars, "array");
    index = ekMapGetS2P(E, frame->closure->closureVars, "index");
    ekAssert((array->type == EVT_ARRAY) && (index->type == EVT_INT));
    ekAssert(argCount == 0);
    ekContextPopValues(E, argCount);

    if (index->intVal < ekArraySize(E, &array->arrayVal)) {
        ekValue * v = array->arrayVal[index->intVal];
        ekArrayPush(E, &E->stack, v);
        ekArrayPush(E, &E->stack, ekValueCreateInt(E, index->intVal++));
        return 2;
    }

    ekArrayPush(E, &E->stack, ekValueNullPtr);
    return 1;
}

static ekU32 arrayCreateIterator(struct ekContext * E, ekU32 argCount)
{
    ekValue * a = NULL;
    ekValue * closure;
    if (!ekContextGetArgs(E, argCount, "a", &a)) {
        return ekContextArgsFailure(E, argCount, "iter(array)");
    }
    closure = ekValueCreateCFunction(E, arrayIterator);
    closure->closureVars = ekMapCreate(E, EMKT_STRING);
    ekMapGetS2P(E, closure->closureVars, "array") = a;
    ekMapGetS2P(E, closure->closureVars, "index") = ekValueCreateInt(E, 0);
    ekArrayPush(E, &E->stack, closure);
    return 1;
}

static ekCFunction * arrayFuncIter(struct ekContext * E, struct ekValue * p)
{
    return arrayCreateIterator;
}

static struct ekValue * arrayFuncReverse(struct ekContext * E, struct ekValue * p)
{
    ekValue * reversed = ekValueCreateArray(E);
    ekS32 size = ekArraySize(E, &p->arrayVal);
    if (size) {
        ekS32 i;
        for (i = size - 1; i >= 0; --i) {
            ekArrayPush(E, &reversed->arrayVal, p->arrayVal[i]);
        }
    }
    return reversed;
}

static struct ekValue * arrayFuncIndex(struct ekContext * E, struct ekValue * value, struct ekValue * index, ekBool lvalue)
{
    ekValue * ret = NULL;
    ekValue ** ref = NULL;

    if ((ret = ekValueTypeGetIntrinsic(E, value->type, index, lvalue)) != 0) { // Intrinsic functions have priority over regular indexing
        return ret;
    }
    if (ekContextGetError(E)) { // An error might be set as a side effect of GetIntrinsic
        return 0;
    }

    index = ekValueToInt(E, index);
    if ((index->intVal >= 0) && (index->intVal < ekArraySize(E, &value->arrayVal))) {
        ref = (ekValue **)&(value->arrayVal[index->intVal]);
        if (lvalue) {
            ret = ekValueCreateRef(E, ref);
        } else {
            ret = *ref;
        }
    } else {
        ekContextSetError(E, EVE_RUNTIME, "array index %d out of range", index->intVal);
    }
    return ret;
}

ekS32 arrayFuncLength(struct ekContext * E, struct ekValue * p)
{
    return ekArraySize(E, &p->arrayVal);
}

static void arrayFuncDump(struct ekContext * E, ekDumpParams * params, struct ekValue * p)
{
    ekS32 i;
    ekStringConcat(E, &params->output, "[ ");
    for (i=0; i < ekArraySize(E, &p->arrayVal); i++) {
        ekValue * child = (ekValue *)p->arrayVal[i];
        if (i > 0) {
            ekStringConcat(E, &params->output, ", ");
        }
        ekValueTypeSafeCall(child->type, Dump) (E, params, child);
    }
    ekStringConcat(E, &params->output, " ]");
}

void ekValueTypeRegisterArray(struct ekContext * E)
{
    ekValueType * type = ekValueTypeCreate(E, "array", 'a');
    type->funcClear      = arrayFuncClear;
    type->funcClone      = arrayFuncClone;
    type->funcMark       = arrayFuncMark;
    type->funcToBool     = arrayFuncToBool;
    type->funcToInt      = arrayFuncToInt;
    type->funcToFloat    = arrayFuncToFloat;
    type->funcReverse    = arrayFuncReverse;
    type->funcIter       = arrayFuncIter;
    type->funcLength     = arrayFuncLength;
    type->funcIndex      = arrayFuncIndex;
    type->funcDump       = arrayFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_ARRAY);

    ekContextAddIntrinsic(E, "push", arrayIntrinsicPush);
    ekContextAddIntrinsic(E, "pop", arrayIntrinsicPop);
    ekContextAddIntrinsic(E, "unshift", arrayIntrinsicUnshift);
    ekContextAddIntrinsic(E, "shift", arrayIntrinsicShift);
    ekContextAddIntrinsic(E, "sort", arrayIntrinsicSort);
}
