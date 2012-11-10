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
// EVT_STRING Intrinsics

static ekU32 stringIntrinsicLength(struct ekContext *E, ekU32 argCount)
{
    ekValue *s = ekContextThis(E);
    ekValue *c = ekValueNullPtr;
    ekAssert(s && s->type == EVT_STRING);
    if(argCount)
    {
        return ekContextArgsFailure(E, argCount, "string.length() takes no arguments");
    }

    c = ekValueCreateInt(E, s->stringVal.len);
    ekValueRemoveRefNote(E, s, "length this done");
    ekArrayPush(E, &E->stack, c);
    return 1;
}

// ---------------------------------------------------------------------------
// EVT_STRING Funcs

static void stringFuncClear(struct ekContext *E, struct ekValue *p)
{
    ekStringClear(E, &p->stringVal);
}

static void stringFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    ekStringSetStr(E, &dst->stringVal, &src->stringVal);
}

static ekBool stringFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return (p->stringVal.len) ? ekTrue : ekFalse;
}

static ekS32 stringFuncToInt(struct ekContext *E, struct ekValue *p)
{
    ekToken t = { ekStringSafePtr(&p->stringVal), p->stringVal.len };
    return ekTokenToInt(E, &t);
}

static ekF32 stringFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    ekToken t = { ekStringSafePtr(&p->stringVal), p->stringVal.len };
    return ekTokenToFloat(E, &t);
}

struct ekValue *stringFuncToString(struct ekContext *E, struct ekValue *p)
{
    // Implicit 'create' of new value (addref p), followed by 'destroy' of old value (removeref p)
    return p;
}

struct ekValue *stringFuncArithmetic(struct ekContext *E, struct ekValue *a, struct ekValue *b, ekValueArithmeticOp op)
{
    ekValue *ret = NULL;
    if(op == EVAO_ADD)
    {
        ekValueAddRefNote(E, a, "stringFuncArithmetic keep a during string conversion");
        ekValueAddRefNote(E, b, "stringFuncArithmetic keep b during string conversion");
        a = ekValueToString(E, a);
        b = ekValueToString(E, b);
        ret = ekValueCreateString(E, ekStringSafePtr(&a->stringVal));
        ekStringConcatStr(E, &ret->stringVal, &a->stringVal);
        ekValueRemoveRefNote(E, a, "stringFuncArithmetic temp a done");
        ekValueRemoveRefNote(E, b, "stringFuncArithmetic temp b done");
    }
    else
    {
        ekTraceExecution(("stringFuncArithmetic(): cannot subtract, multiply, or divide strings!"));
    }
    return ret;
}

static ekBool stringFuncCmp(struct ekContext *E, struct ekValue *a, struct ekValue *b, int *cmpResult)
{
    if(b->type == EVT_STRING)
    {
        *cmpResult = ekStringCmpStr(E, &a->stringVal, &b->stringVal);
        return ekTrue;
    }
    return ekFalse;
}

static void stringFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    ekStringConcatLen(E, &params->output, "\"", 1);
    ekStringConcatStr(E, &params->output, &p->stringVal);
    ekStringConcatLen(E, &params->output, "\"", 1);
}

static ekU32 ekiString(struct ekContext *E, ekU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(E, argCount, "?", &v))
    {
        return ekContextArgsFailure(E, argCount, "string(value)");
    }

    ekArrayPush(E, &E->stack, ekValueToString(E, v));
    return 1;
}

void ekValueTypeRegisterString(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "string");
    type->funcClear      = stringFuncClear;
    type->funcClone      = stringFuncClone;
    type->funcToBool     = stringFuncToBool;
    type->funcToInt      = stringFuncToInt;
    type->funcToFloat    = stringFuncToFloat;
    type->funcToString   = stringFuncToString;
    type->funcIter       = ekValueTypeFuncNotUsed;
    type->funcArithmetic = stringFuncArithmetic;
    type->funcCmp        = stringFuncCmp;
    type->funcIndex      = ekValueTypeFuncNotUsed;
    type->funcDump       = stringFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_STRING);

    ekValueTypeAddIntrinsic(E, type, "length", stringIntrinsicLength);
    ekContextAddIntrinsic(E, "string", ekiString);
}