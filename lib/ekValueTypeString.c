// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekValue.h"

#include "ekContext.h"
#include "ekLexer.h"
#include "ekValueType.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// EVT_STRING Intrinsics

// TODO: add substr, join, [others]

static ekU32 stringIntrinsicSplit(struct ekContext *E, ekU32 argCount)
{
    ekValue *sep = NULL;
    ekValue *str = NULL;
    ekValue *a;
    if(!ekContextGetArgs(E, argCount, "ss", &str, &sep))
    {
        return ekContextArgsFailure(E, argCount, "split([string] str, [string] seps)");
    }

    a = ekValueCreateArray(E);
    {
        const char *front = ekValueSafeStr(str);
        const char *seps = ekValueSafeStr(sep);
        while(*front)
        {
            ekS32 split = strcspn(front, seps);
            if(split > 0)
            {
                ekValue *newString = ekValueCreateStringLen(E, front, split);
                ekArrayPush(E, &a->arrayVal, newString);
            }
            front += split + 1;
        }
    }

    ekArrayPush(E, &E->stack, a);
    ekValueRemoveRefNote(E, sep, "string split sep done");
    ekValueRemoveRefNote(E, str, "string split str done");
    return 1;
}

static ekU32 stringIntrinsicJoin(struct ekContext *E, ekU32 argCount)
{
    ekValue *a = NULL;
    ekValue *sep = NULL;
    ekValue *str;
    ekS32 i;
    if(!ekContextGetArgs(E, argCount, "as", &a, &sep))
    {
        return ekContextArgsFailure(E, argCount, "join([array] a, [string] sep)");
    }

    str = ekValueCreateString(E, "");
    for(i = 0; i < ekArraySize(E, &a->arrayVal); ++i)
    {
        ekValue *v = a->arrayVal[i];
        if(i)
        {
            ekStringConcat(E, &str->stringVal, ekValueSafeStr(sep));
        }
        ekValueAddRefNote(E, v, "converting to string, but keeping in array");
        v = ekValueToString(E, v);
        ekStringConcat(E, &str->stringVal, ekValueSafeStr(v));
        ekValueRemoveRefNote(E, v, "done with temp string");
    }

    ekArrayPush(E, &E->stack, str);
    ekValueRemoveRefNote(E, a, "string split a done");
    ekValueRemoveRefNote(E, sep, "string split sep done");
    return 1;
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

static ekU32 stringIntrinsicChomp(struct ekContext *E, ekU32 argCount)
{
    ekValue *str = NULL;
    ekValue *newstr = ekValueNullPtr;
    char *c;
    int len;

    if(!ekContextGetArgs(E, argCount, "s", &str))
    {
        return ekContextArgsFailure(E, argCount, "chomp([string] str)");
    }


    len = str->stringVal.len;
    if(str->stringVal.text && str->stringVal.len)
    {
        char *end = &str->stringVal.text[len-1];
        while(len && ((*end == '\n') || (*end == '\r')))
        {
            --len;
            --end;
        }
        *(end+1) = 0;
    }

    if(len != str->stringVal.len)
    {
        newstr = ekValueCreateStringLen(E, str->stringVal.text, len);
    }
    else
    {
        newstr = str;
        ekValueAddRefNote(E, str, "chomp not changing string; reuse");
    }

    ekArrayPush(E, &E->stack, newstr);
    ekValueRemoveRefNote(E, str, "string split str done");
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

static struct ekValue *stringFuncReverse(struct ekContext *E, struct ekValue *p)
{
    if(p->stringVal.len)
    {
        ekValue *reversed = ekValueCreateString(E, ekStringSafePtr(&p->stringVal));
        char *front = reversed->stringVal.text;
        char *back  = reversed->stringVal.text + reversed->stringVal.len - 1;
        while(back > front)
        {
            char tmp = *front;
            *front = *back;
            *back = tmp;

            ++front;
            --back;
        }
        ekValueRemoveRefNote(E, p, "reverse string done with input");
        return reversed;
    }

    // Continue using the empty string value
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

static ekBool stringFuncCmp(struct ekContext *E, struct ekValue *a, struct ekValue *b, ekS32 *cmpResult)
{
    if(b->type == EVT_STRING)
    {
        *cmpResult = ekStringCmpStr(E, &a->stringVal, &b->stringVal);
        return ekTrue;
    }
    return ekFalse;
}

ekS32 stringFuncLength(struct ekContext *E, struct ekValue *p)
{
    return p->stringVal.len;
}

static void stringFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    ekStringConcatLen(E, &params->output, "\"", 1);
    ekStringConcatStr(E, &params->output, &p->stringVal);
    ekStringConcatLen(E, &params->output, "\"", 1);
}

void ekValueTypeRegisterString(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "string", 's');
    type->funcClear      = stringFuncClear;
    type->funcClone      = stringFuncClone;
    type->funcToBool     = stringFuncToBool;
    type->funcToInt      = stringFuncToInt;
    type->funcToFloat    = stringFuncToFloat;
    type->funcToString   = stringFuncToString;
    type->funcReverse    = stringFuncReverse;
    type->funcArithmetic = stringFuncArithmetic;
    type->funcCmp        = stringFuncCmp;
    type->funcLength     = stringFuncLength;
    type->funcDump       = stringFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_STRING);

    ekContextAddIntrinsic(E, "string", ekiString);
    ekContextAddIntrinsic(E, "split", stringIntrinsicSplit);
    ekContextAddIntrinsic(E, "join", stringIntrinsicJoin);
    ekContextAddIntrinsic(E, "chomp", stringIntrinsicChomp);
}
