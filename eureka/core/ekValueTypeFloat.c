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
// EVT_FLOAT Funcs

static void floatFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    dst->floatVal = src->floatVal;
}

static ekBool floatFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return (p->floatVal != 0.0f) ? ekTrue : ekFalse;
}

static ekS32 floatFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return (ekS32)p->floatVal;
}

static ekF32 floatFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return p->floatVal;
}

static struct ekValue *floatFuncToString(struct ekContext *E, struct ekValue *p)
{
    char temp[64];
    sprintf(temp, "%f", p->floatVal);
    ekValueRemoveRefNote(E, p, "floatFuncToString doesnt need float anymore");
    return ekValueCreateString(E, temp);
}

static struct ekValue *floatFuncArithmetic(struct ekContext *E, struct ekValue *a, struct ekValue *b, ekValueArithmeticOp op)
{
    ekValueAddRefNote(E, b, "floatFuncArithmetic keep b during float conversion");
    b = ekValueToFloat(E, b);
    switch(op)
    {
        case EVAO_ADD:
            a = ekValueCreateFloat(E, a->floatVal + b->floatVal);
            break;
        case EVAO_SUB:
            a = ekValueCreateFloat(E, a->floatVal - b->floatVal);
            break;
        case EVAO_MUL:
            a = ekValueCreateFloat(E, a->floatVal * b->floatVal);
            break;
        case EVAO_DIV:
            if(b->floatVal == 0.0f)
            {
                ekContextSetError(E, EVE_RUNTIME, "divide by zero!");
            }
            else
            {
                a = ekValueCreateFloat(E, a->floatVal / b->floatVal);
            }
            break;
    };
    ekValueRemoveRefNote(E, b, "floatFuncArithmetic temp b done");
    return a;
}

static ekBool floatFuncCmp(struct ekContext *E, struct ekValue *a, struct ekValue *b, ekS32 *cmpResult)
{
    if(b->type == EVT_INT)
    {
        if(a->floatVal > (ekF32)b->intVal)
        {
            *cmpResult = 1;
        }
        else if(a->floatVal < (ekF32)b->intVal)
        {
            *cmpResult = -1;
        }
        else
        {
            *cmpResult = 0;
        }
        return ekTrue;
    }
    else if(b->type == EVT_FLOAT)
    {
        if(a->floatVal > b->floatVal)
        {
            *cmpResult = 1;
        }
        else if(a->floatVal < b->floatVal)
        {
            *cmpResult = -1;
        }
        else
        {
            *cmpResult = 0;
        }
        return ekTrue;
    }
    return ekFalse;
}

static void floatFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    char temp[64];
    sprintf(temp, "%f", p->floatVal);
    ekStringConcat(E, &params->output, temp);
}

static ekU32 ekiFloat(struct ekContext *E, ekU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(E, argCount, "?", &v))
    {
        return ekContextArgsFailure(E, argCount, "float(value)");
    }

    ekArrayPush(E, &E->stack, ekValueToFloat(E, v));
    return 1;
}

void ekValueTypeRegisterFloat(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "float");
    type->funcClear      = ekValueTypeFuncNotUsed;
    type->funcClone      = floatFuncClone;
    type->funcToBool     = floatFuncToBool;
    type->funcToInt      = floatFuncToInt;
    type->funcToFloat    = floatFuncToFloat;
    type->funcToString   = floatFuncToString;
    type->funcIter       = ekValueTypeFuncNotUsed;
    type->funcArithmetic = floatFuncArithmetic;
    type->funcCmp        = floatFuncCmp;
    type->funcIndex      = ekValueTypeFuncNotUsed;
    type->funcDump       = floatFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_FLOAT);

    ekContextAddIntrinsic(E, "float", ekiFloat);
}
