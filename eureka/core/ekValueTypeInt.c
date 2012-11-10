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
// EVT_INT Funcs

static void intFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    dst->intVal = src->intVal;
}

static ekBool intFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return (p->intVal) ? ekTrue : ekFalse;
}

static ekS32 intFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return p->intVal;
}

static ekF32 intFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return (ekF32)p->intVal;
}

static struct ekValue *intFuncToString(struct ekContext *E, struct ekValue *p)
{
    char temp[32];
    sprintf(temp, "%d", p->intVal);
    ekValueRemoveRefNote(E, p, "intFuncToString doesnt need int anymore");
    return ekValueCreateString(E, temp);
}

static struct ekValue *intFuncArithmetic(struct ekContext *E, struct ekValue *a, struct ekValue *b, ekValueArithmeticOp op)
{
    ekValue *c = NULL;
    ekValueAddRefNote(E, b, "intFuncArithmetic keep b during int conversion");
    b = ekValueToInt(E, b);
    switch(op)
    {
        case EVAO_ADD:
            c = ekValueCreateInt(E, a->intVal + b->intVal);
            break;
        case EVAO_SUB:
            c = ekValueCreateInt(E, a->intVal - b->intVal);
            break;
        case EVAO_MUL:
            c = ekValueCreateInt(E, a->intVal * b->intVal);
            break;
        case EVAO_DIV:
            if(!b->intVal)
            {
                ekContextSetError(E, EVE_RUNTIME, "divide by zero!");
            }
            else
            {
                c = ekValueCreateInt(E, a->intVal / b->intVal);
            }
            break;
    };
    ekValueRemoveRefNote(E, b, "intFuncArithmetic temp b done");
    return c;
}

static ekBool intFuncCmp(struct ekContext *E, struct ekValue *a, struct ekValue *b, int *cmpResult)
{
    if(b->type == EVT_FLOAT)
    {
        if((ekF32)a->intVal > b->floatVal)
        {
            *cmpResult = 1;
        }
        else if((ekF32)a->intVal < b->floatVal)
        {
            *cmpResult = -1;
        }
        else
        {
            *cmpResult = 0;
        }
        return ekTrue;
    }
    else if(b->type == EVT_INT)
    {
        *cmpResult = a->intVal - b->intVal;
        return ekTrue;
    }
    return ekFalse;
}

static void intFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    char temp[64];
    sprintf(temp, "%d", p->intVal);
    ekStringConcat(E, &params->output, temp);
}

static ekU32 ekiInt(struct ekContext *E, ekU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(E, argCount, "?", &v))
    {
        return ekContextArgsFailure(E, argCount, "int(value)");
    }

    ekArrayPush(E, &E->stack, ekValueToInt(E, v));
    return 1;
}

void ekValueTypeRegisterInt(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "int");
    type->funcClear      = ekValueTypeFuncNotUsed;
    type->funcClone      = intFuncClone;
    type->funcToBool     = intFuncToBool;
    type->funcToInt      = intFuncToInt;
    type->funcToFloat    = intFuncToFloat;
    type->funcToString   = intFuncToString;
    type->funcIter       = ekValueTypeFuncNotUsed;
    type->funcArithmetic = intFuncArithmetic;
    type->funcCmp        = intFuncCmp;
    type->funcIndex      = ekValueTypeFuncNotUsed;
    type->funcDump       = intFuncDump;
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_INT);

    ekContextAddIntrinsic(E, "int", ekiInt);
}
