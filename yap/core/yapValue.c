// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapValue.h"

#include "yapFrame.h"
#include "yapHash.h"
#include "yapObject.h"
#include "yapLexer.h"
#include "yapContext.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

static char *NULL_STRING_FORM = "(null)";

// ---------------------------------------------------------------------------

yapDumpParams *yapDumpParamsCreate(struct yapContext *Y)
{
    yapDumpParams *params = (yapDumpParams *)yapAlloc(sizeof(yapDumpParams));
    return params;
}

void yapDumpParamsDestroy(struct yapContext *Y, yapDumpParams *params)
{
    yapStringClear(Y, &params->output);
    yapStringClear(Y, &params->tempStr);
    yapFree(params);
}

// ---------------------------------------------------------------------------

yapValueType *yapValueTypeCreate(struct yapContext *Y, const char *name)
{
    yapValueType *type = yapAlloc(sizeof(yapValueType));
    strcpy(type->name, name);
    return type;
}

void yapValueTypeDestroy(struct yapContext *Y, yapValueType *type)
{
    if(type->funcDestroyUserData)
    {
        type->funcDestroyUserData(Y, type);
    }
    yapFree(type);
}

int yapValueTypeRegister(struct yapContext *Y, yapValueType *newType)
{
    int i;
    for(i=0; i<Y->types.count; i++)
    {
        yapValueType *t = Y->types.data[i];
        if(!strcmp(newType->name, t->name))
        {
            return YVT_INVALID;
        }
    }

    // If you are hitting one of these asserts, then your custom type isn't handling a required function.
    // If you're a C++ person, pretend the compiler is telling you that you forgot to implement a pure virtual.
    // If you don't want to do anything for a particular function, explicitly set it to yapValueTypeFuncNotUsed.
    yapAssert(newType->funcClear);
    yapAssert(newType->funcClone);
    yapAssert(newType->funcToBool);
    yapAssert(newType->funcToInt);
    yapAssert(newType->funcToFloat);
    yapAssert(newType->funcToString);
    yapAssert(newType->funcArithmetic);
    yapAssert(newType->funcCmp);
    yapAssert(newType->funcIndex);
    yapAssert(newType->funcDump);
    yapAssert(newType->funcDump != yapValueTypeFuncNotUsed); // required!

    newType->id = yapArrayPush(Y, &Y->types, newType);
    return newType->id;
}

// ---------------------------------------------------------------------------
// YVT_NULL Funcs

static struct yapValue *nullFuncToString(struct yapContext *Y, struct yapValue *p)
{
    return yapValueCreateKString(Y, NULL_STRING_FORM);
}

static void nullFuncDump(struct yapContext *Y, yapDumpParams *params, struct yapValue *p)
{
    yapStringConcat(Y, &params->output, NULL_STRING_FORM);
}

static void nullFuncRegister(struct yapContext *Y)
{
    yapValueType *type   = yapValueTypeCreate(Y, "null");
    type->funcClear      = yapValueTypeFuncNotUsed;
    type->funcClone      = yapValueTypeFuncNotUsed;
    type->funcToBool     = yapValueTypeFuncNotUsed;
    type->funcToInt      = yapValueTypeFuncNotUsed;
    type->funcToFloat    = yapValueTypeFuncNotUsed;
    type->funcToString   = nullFuncToString;
    type->funcArithmetic = yapValueTypeFuncNotUsed;
    type->funcCmp        = yapValueTypeFuncNotUsed;
    type->funcIndex      = yapValueTypeFuncNotUsed;
    type->funcDump       = nullFuncDump;
    yapValueTypeRegister(Y, type);
    yapAssert(type->id == YVT_NULL);
}

// ---------------------------------------------------------------------------
// YVT_BLOCK Funcs

static void blockFuncClear(struct yapContext *Y, struct yapValue *p)
{
    if(p->closureVars)
    {
        yapHashDestroy(Y, p->closureVars, (yapDestroyCB)yapValueRemoveRef);
    }
}

static void blockFuncClone(struct yapContext *Y, struct yapValue *dst, struct yapValue *src)
{
    dst->blockVal = src->blockVal;
}

static yBool blockFuncToBool(struct yapContext *Y, struct yapValue *p)
{
    return yTrue;
}

static yS32 blockFuncToInt(struct yapContext *Y, struct yapValue *p)
{
    return 1; // ?
}

static yF32 blockFuncToFloat(struct yapContext *Y, struct yapValue *p)
{
    return 1.0f; // ?
}

static void blockFuncDump(struct yapContext *Y, yapDumpParams *params, struct yapValue *p)
{
    char temp[64];
    sprintf(temp, "(block:0x%p)", p->blockVal);
    yapStringConcat(Y, &params->output, temp);
}

static void blockFuncRegister(struct yapContext *Y)
{
    yapValueType *type = yapValueTypeCreate(Y, "block");
    type->funcClear      = blockFuncClear;
    type->funcClone      = blockFuncClone;
    type->funcToBool     = blockFuncToBool;
    type->funcToInt      = blockFuncToInt;
    type->funcToFloat    = blockFuncToFloat;
    type->funcToString   = yapValueTypeFuncNotUsed;
    type->funcArithmetic = yapValueTypeFuncNotUsed;
    type->funcCmp        = yapValueTypeFuncNotUsed;
    type->funcIndex      = yapValueTypeFuncNotUsed;
    type->funcDump       = blockFuncDump;
    yapValueTypeRegister(Y, type);
    yapAssert(type->id == YVT_BLOCK);
}

// ---------------------------------------------------------------------------
// YVT_CFUNCTION Funcs

static void cfunctionFuncClone(struct yapContext *Y, struct yapValue *dst, struct yapValue *src)
{
    dst->cFuncVal = src->cFuncVal;
}

static yBool cfunctionFuncToBool(struct yapContext *Y, struct yapValue *p)
{
    return yTrue;
}

static yS32 cfunctionFuncToInt(struct yapContext *Y, struct yapValue *p)
{
    return 1; // ?
}

static yF32 cfunctionFuncToFloat(struct yapContext *Y, struct yapValue *p)
{
    return 1.0f; // ?
}

static void cfunctionFuncDump(struct yapContext *Y, yapDumpParams *params, struct yapValue *p)
{
    char temp[64];
    sprintf(temp, "(cfunction:0x%p)", p->cFuncVal);
    yapStringConcat(Y, &params->output, temp);
}

static void cfunctionFuncRegister(struct yapContext *Y)
{
    yapValueType *type = yapValueTypeCreate(Y, "cfunction");
    type->funcClear      = yapValueTypeFuncNotUsed;
    type->funcClone      = cfunctionFuncClone;
    type->funcToBool     = cfunctionFuncToBool;
    type->funcToInt      = cfunctionFuncToInt;
    type->funcToFloat    = cfunctionFuncToFloat;
    type->funcToString   = yapValueTypeFuncNotUsed;
    type->funcArithmetic = yapValueTypeFuncNotUsed;
    type->funcCmp        = yapValueTypeFuncNotUsed;
    type->funcIndex      = yapValueTypeFuncNotUsed;
    type->funcDump       = cfunctionFuncDump;
    yapValueTypeRegister(Y, type);
    yapAssert(type->id == YVT_CFUNCTION);
}

// ---------------------------------------------------------------------------
// YVT_INT Funcs

static void intFuncClone(struct yapContext *Y, struct yapValue *dst, struct yapValue *src)
{
    dst->intVal = src->intVal;
}

static yBool intFuncToBool(struct yapContext *Y, struct yapValue *p)
{
    return (p->intVal) ? yTrue : yFalse;
}

static yS32 intFuncToInt(struct yapContext *Y, struct yapValue *p)
{
    return p->intVal;
}

static yF32 intFuncToFloat(struct yapContext *Y, struct yapValue *p)
{
    return (yF32)p->intVal;
}

static struct yapValue *intFuncToString(struct yapContext *Y, struct yapValue *p)
{
    char temp[32];
    sprintf(temp, "%d", p->intVal);
    return yapValueCreateString(Y, temp);
}

static struct yapValue *intFuncArithmetic(struct yapContext *Y, struct yapValue *a, struct yapValue *b, yapValueArithmeticOp op)
{
    yapValue *c = NULL;
    yapValueAddRefNote(Y, b, "intFuncArithmetic keep b during int conversion");
    b = yapValueToInt(Y, b);
    switch(op)
    {
        case YVAO_ADD:
            c = yapValueCreateInt(Y, a->intVal + b->intVal);
            break;
        case YVAO_SUB:
            c = yapValueCreateInt(Y, a->intVal - b->intVal);
            break;
        case YVAO_MUL:
            c = yapValueCreateInt(Y, a->intVal * b->intVal);
            break;
        case YVAO_DIV:
            if(!b->intVal)
            {
                yapContextSetError(Y, YVE_RUNTIME, "divide by zero!");
            }
            else
            {
                c = yapValueCreateInt(Y, a->intVal / b->intVal);
            }
            break;
    };
    yapValueRemoveRefNote(Y, b, "intFuncArithmetic temp b done");
    return c;
}

static yBool intFuncCmp(struct yapContext *Y, struct yapValue *a, struct yapValue *b, int *cmpResult)
{
    if(b->type == YVT_FLOAT)
    {
        if((yF32)a->intVal > b->floatVal)
        {
            *cmpResult = 1;
        }
        else if((yF32)a->intVal < b->floatVal)
        {
            *cmpResult = -1;
        }
        else
        {
            *cmpResult = 0;
        }
        return yTrue;
    }
    else if(b->type == YVT_INT)
    {
        *cmpResult = a->intVal - b->intVal;
        return yTrue;
    }
    return yFalse;
}

static void intFuncDump(struct yapContext *Y, yapDumpParams *params, struct yapValue *p)
{
    char temp[64];
    sprintf(temp, "%d", p->intVal);
    yapStringConcat(Y, &params->output, temp);
}

static void intFuncRegister(struct yapContext *Y)
{
    yapValueType *type = yapValueTypeCreate(Y, "int");
    type->funcClear      = yapValueTypeFuncNotUsed;
    type->funcClone      = intFuncClone;
    type->funcToBool     = intFuncToBool;
    type->funcToInt      = intFuncToInt;
    type->funcToFloat    = intFuncToFloat;
    type->funcToString   = intFuncToString;
    type->funcArithmetic = intFuncArithmetic;
    type->funcCmp        = intFuncCmp;
    type->funcIndex      = yapValueTypeFuncNotUsed;
    type->funcDump       = intFuncDump;
    yapValueTypeRegister(Y, type);
    yapAssert(type->id == YVT_INT);
}

// ---------------------------------------------------------------------------
// YVT_FLOAT Funcs

static void floatFuncClone(struct yapContext *Y, struct yapValue *dst, struct yapValue *src)
{
    dst->floatVal = src->floatVal;
}

static yBool floatFuncToBool(struct yapContext *Y, struct yapValue *p)
{
    return (p->floatVal != 0.0f) ? yTrue : yFalse;
}

static yS32 floatFuncToInt(struct yapContext *Y, struct yapValue *p)
{
    return (yS32)p->floatVal;
}

static yF32 floatFuncToFloat(struct yapContext *Y, struct yapValue *p)
{
    return p->floatVal;
}

static struct yapValue *floatFuncToString(struct yapContext *Y, struct yapValue *p)
{
    char temp[64];
    sprintf(temp, "%f", p->floatVal);
    return yapValueCreateString(Y, temp);
}

static struct yapValue *floatFuncArithmetic(struct yapContext *Y, struct yapValue *a, struct yapValue *b, yapValueArithmeticOp op)
{
    yapValueAddRefNote(Y, b, "floatFuncArithmetic keep b during float conversion");
    b = yapValueToFloat(Y, b);
    switch(op)
    {
        case YVAO_ADD:
            a = yapValueCreateFloat(Y, a->floatVal + b->floatVal);
            break;
        case YVAO_SUB:
            a = yapValueCreateFloat(Y, a->floatVal - b->floatVal);
            break;
        case YVAO_MUL:
            a = yapValueCreateFloat(Y, a->floatVal * b->floatVal);
            break;
        case YVAO_DIV:
            if(b->floatVal == 0.0f)
            {
                yapContextSetError(Y, YVE_RUNTIME, "divide by zero!");
            }
            else
            {
                a = yapValueCreateFloat(Y, a->floatVal / b->floatVal);
            }
            break;
    };
    yapValueRemoveRefNote(Y, b, "floatFuncArithmetic temp b done");
    return a;
}

static yBool floatFuncCmp(struct yapContext *Y, struct yapValue *a, struct yapValue *b, int *cmpResult)
{
    if(b->type == YVT_INT)
    {
        if(a->floatVal > (yF32)b->intVal)
        {
            *cmpResult = 1;
        }
        else if(a->floatVal < (yF32)b->intVal)
        {
            *cmpResult = -1;
        }
        else
        {
            *cmpResult = 0;
        }
        return yTrue;
    }
    else if(b->type == YVT_FLOAT)
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
        return yTrue;
    }
    return yFalse;
}

static void floatFuncDump(struct yapContext *Y, yapDumpParams *params, struct yapValue *p)
{
    char temp[64];
    sprintf(temp, "%f", p->floatVal);
    yapStringConcat(Y, &params->output, temp);
}

static void floatFuncRegister(struct yapContext *Y)
{
    yapValueType *type = yapValueTypeCreate(Y, "float");
    type->funcClear      = yapValueTypeFuncNotUsed;
    type->funcClone      = floatFuncClone;
    type->funcToBool     = floatFuncToBool;
    type->funcToInt      = floatFuncToInt;
    type->funcToFloat    = floatFuncToFloat;
    type->funcToString   = floatFuncToString;
    type->funcArithmetic = floatFuncArithmetic;
    type->funcCmp        = floatFuncCmp;
    type->funcIndex      = yapValueTypeFuncNotUsed;
    type->funcDump       = floatFuncDump;
    yapValueTypeRegister(Y, type);
    yapAssert(type->id == YVT_FLOAT);
}

// ---------------------------------------------------------------------------
// YVT_STRING Funcs

static void stringFuncClear(struct yapContext *Y, struct yapValue *p)
{
    yapStringClear(Y, &p->stringVal);
}

static void stringFuncClone(struct yapContext *Y, struct yapValue *dst, struct yapValue *src)
{
    yapStringSetStr(Y, &dst->stringVal, &src->stringVal);
}

static yBool stringFuncToBool(struct yapContext *Y, struct yapValue *p)
{
    return (p->stringVal.len) ? yTrue : yFalse;
}

static yS32 stringFuncToInt(struct yapContext *Y, struct yapValue *p)
{
    yapToken t = { yapStringSafePtr(&p->stringVal), p->stringVal.len };
    return yapTokenToInt(Y, &t);
}

static yF32 stringFuncToFloat(struct yapContext *Y, struct yapValue *p)
{
    yapToken t = { yapStringSafePtr(&p->stringVal), p->stringVal.len };
    return yapTokenToFloat(Y, &t);
}

struct yapValue *stringFuncToString(struct yapContext *Y, struct yapValue *p)
{
    return p;
}

struct yapValue *stringFuncArithmetic(struct yapContext *Y, struct yapValue *a, struct yapValue *b, yapValueArithmeticOp op)
{
    yapValue *ret = NULL;
    if(op == YVAO_ADD)
    {
        yapValueAddRefNote(Y, a, "stringFuncArithmetic keep a during string conversion");
        yapValueAddRefNote(Y, b, "stringFuncArithmetic keep b during string conversion");
        a = yapValueToString(Y, a);
        b = yapValueToString(Y, b);
        ret = yapValueCreateString(Y, yapStringSafePtr(&a->stringVal));
        yapStringConcatStr(Y, &ret->stringVal, &a->stringVal);
        yapValueRemoveRefNote(Y, a, "stringFuncArithmetic temp a done");
        yapValueRemoveRefNote(Y, b, "stringFuncArithmetic temp b done");
    }
    else
    {
        yapTraceExecution(("stringFuncArithmetic(): cannot subtract, multiply, or divide strings!"));
    }
    return ret;
}

static yBool stringFuncCmp(struct yapContext *Y, struct yapValue *a, struct yapValue *b, int *cmpResult)
{
    if(b->type == YVT_STRING)
    {
        *cmpResult = yapStringCmpStr(Y, &a->stringVal, &b->stringVal);
        return yTrue;
    }
    return yFalse;
}

static void stringFuncDump(struct yapContext *Y, yapDumpParams *params, struct yapValue *p)
{
    yapStringConcatLen(Y, &params->output, "\"", 1);
    yapStringConcatStr(Y, &params->output, &p->stringVal);
    yapStringConcatLen(Y, &params->output, "\"", 1);
}

static void stringFuncRegister(struct yapContext *Y)
{
    yapValueType *type = yapValueTypeCreate(Y, "string");
    type->funcClear      = stringFuncClear;
    type->funcClone      = stringFuncClone;
    type->funcToBool     = stringFuncToBool;
    type->funcToInt      = stringFuncToInt;
    type->funcToFloat    = stringFuncToFloat;
    type->funcToString   = stringFuncToString;
    type->funcArithmetic = stringFuncArithmetic;
    type->funcCmp        = stringFuncCmp;
    type->funcIndex      = yapValueTypeFuncNotUsed;
    type->funcDump       = stringFuncDump;
    yapValueTypeRegister(Y, type);
    yapAssert(type->id == YVT_STRING);
}

// ---------------------------------------------------------------------------
// YVT_ARRAY Funcs

static void arrayFuncClear(struct yapContext *Y, struct yapValue *p)
{
    yapArrayDestroy(Y, p->arrayVal, (yapDestroyCB)yapValueRemoveRefHashed);
}

static void arrayFuncClone(struct yapContext *Y, struct yapValue *dst, struct yapValue *src)
{
    yapAssert(0 && "arrayFuncClone not implemented");
}

static yBool arrayFuncToBool(struct yapContext *Y, struct yapValue *p)
{
    return (p->arrayVal && p->arrayVal->count) ? yTrue : yFalse;
}

static yS32 arrayFuncToInt(struct yapContext *Y, struct yapValue *p)
{
    return (p->arrayVal) ? p->arrayVal->count : 0;
}

static yF32 arrayFuncToFloat(struct yapContext *Y, struct yapValue *p)
{
    return (p->arrayVal) ? (yF32)p->arrayVal->count : 0;
}

static struct yapValue *arrayFuncIndex(struct yapContext *Y, struct yapValue *value, struct yapValue *index, yBool lvalue)
{
    yapValue *ret = NULL;
    yapValue **ref = NULL;
    index = yapValueToInt(Y, index);
    if(index->intVal >= 0 && index->intVal < value->arrayVal->count)
    {
        ref = (yapValue **) & (value->arrayVal->data[index->intVal]);
        if(lvalue)
        {
            ret = yapValueCreateRef(Y, ref);
        }
        else
        {
            ret = *ref;
            yapValueAddRefNote(Y, ret, "arrayFuncIndex");
        }
    }
    else
    {
        yapContextSetError(Y, YVE_RUNTIME, "array index %d out of range", index->intVal);
    }
    return ret;
}

static void arrayFuncDump(struct yapContext *Y, yapDumpParams *params, struct yapValue *p)
{
    int i;
    yapStringConcat(Y, &params->output, "[ ");
    for(i=0; i<p->arrayVal->count; i++)
    {
        yapValue *child = (yapValue *)p->arrayVal->data[i];
        if(i > 0)
        {
            yapStringConcat(Y, &params->output, ", ");
        }
        yapValueTypeSafeCall(child->type, Dump)(Y, params, child);
    }
    yapStringConcat(Y, &params->output, " ]");
}

static void arrayFuncRegister(struct yapContext *Y)
{
    yapValueType *type = yapValueTypeCreate(Y, "array");
    type->funcClear      = arrayFuncClear;
    type->funcClone      = arrayFuncClone;
    type->funcToBool     = arrayFuncToBool;
    type->funcToInt      = arrayFuncToInt;
    type->funcToFloat    = arrayFuncToFloat;
    type->funcToString   = yapValueTypeFuncNotUsed;
    type->funcArithmetic = yapValueTypeFuncNotUsed;
    type->funcCmp        = yapValueTypeFuncNotUsed;
    type->funcIndex      = arrayFuncIndex;
    type->funcDump       = arrayFuncDump;
    yapValueTypeRegister(Y, type);
    yapAssert(type->id == YVT_ARRAY);
}

// ---------------------------------------------------------------------------
// YVT_OBJECT Funcs

static void objectFuncClear(struct yapContext *Y, struct yapValue *p)
{
    yapObjectDestroy(Y, p->objectVal);
}

static void objectFuncClone(struct yapContext *Y, struct yapValue *dst, struct yapValue *src)
{
    yapAssert(0 && "objectFuncClone not implemented");
}

static yBool objectFuncToBool(struct yapContext *Y, struct yapValue *p)
{
    return yTrue;
}

static yS32 objectFuncToInt(struct yapContext *Y, struct yapValue *p)
{
    return 1; // ?
}

static yF32 objectFuncToFloat(struct yapContext *Y, struct yapValue *p)
{
    return 1.0f; // ?
}

static struct yapValue *objectFuncToString(struct yapContext *Y, struct yapValue *p)
{
    char temp[32];
    sprintf(temp, "[object:%p]", p->objectVal);
    return yapValueCreateString(Y, temp);
}

static struct yapValue *objectFuncIndex(struct yapContext *Y, struct yapValue *value, struct yapValue *index, yBool lvalue)
{
    yapValue *ret = NULL;
    yapValue **ref = NULL;
    index = yapValueToString(Y, index);
    ref = yapObjectGetRef(Y, value->objectVal, yapStringSafePtr(&index->stringVal), lvalue /* create? */);
    if(lvalue)
    {
        ret = yapValueCreateRef(Y, ref);
    }
    else
    {
        ret = *ref;
        yapValueAddRefNote(Y, ret, "objectFuncIndex");
    }
    return ret;
}

void appendKeys(struct yapContext *Y, yapDumpParams *params, yapHashEntry *entry)
{
    yapValue *child = (yapValue *)entry->value;
    if(params->tempInt)
    {
        yapStringConcat(Y, &params->output, "\"");
    }
    else
    {
        yapStringConcat(Y, &params->output, ", \"");
    }

    yapStringConcat(Y, &params->output, entry->key);
    yapStringConcat(Y, &params->output, "\" : ");
    yapValueTypeSafeCall(child->type, Dump)(Y, params, child);
    params->tempInt = 0;
}

static void objectFuncDump(struct yapContext *Y, yapDumpParams *params, struct yapValue *p)
{
    params->tempInt = 1;
    yapStringConcat(Y, &params->output, "{ ");
    yapHashIterateP1(Y, p->objectVal->hash, (yapIterateCB1)appendKeys, params);
    yapStringConcat(Y, &params->output, " }");
}

static void objectFuncRegister(struct yapContext *Y)
{
    yapValueType *type = yapValueTypeCreate(Y, "object");
    type->funcClear      = objectFuncClear;
    type->funcClone      = objectFuncClone;
    type->funcToBool     = objectFuncToBool;
    type->funcToInt      = objectFuncToInt;
    type->funcToFloat    = objectFuncToFloat;
    type->funcToString   = objectFuncToString;
    type->funcArithmetic = yapValueTypeFuncNotUsed;
    type->funcCmp        = yapValueTypeFuncNotUsed;
    type->funcIndex      = objectFuncIndex;
    type->funcDump       = objectFuncDump;
    yapValueTypeRegister(Y, type);
    yapAssert(type->id == YVT_OBJECT);
}

// ---------------------------------------------------------------------------
// YVT_REF Funcs

static void refFuncClone(struct yapContext *Y, struct yapValue *dst, struct yapValue *src)
{
    dst->refVal = src->refVal;
}

static yBool refFuncToBool(struct yapContext *Y, struct yapValue *p)
{
    return (*p->refVal) ? yTrue : yFalse;
}

static yS32 refFuncToInt(struct yapContext *Y, struct yapValue *p)
{
    return 1; // ?
}

static yF32 refFuncToFloat(struct yapContext *Y, struct yapValue *p)
{
    return 1.0f; // ?
}

static void refFuncDump(struct yapContext *Y, yapDumpParams *params, struct yapValue *p)
{
    yapStringConcat(Y, &params->output, "(ref: ");
    yapValueTypeSafeCall((*p->refVal)->type, Dump)(Y, params, *p->refVal);
    yapStringConcat(Y, &params->output, ")");
}

static void refFuncRegister(struct yapContext *Y)
{
    yapValueType *type = yapValueTypeCreate(Y, "ref");
    type->funcClear      = yapValueTypeFuncNotUsed;
    type->funcClone      = refFuncClone;
    type->funcToBool     = refFuncToBool;
    type->funcToInt      = refFuncToInt;
    type->funcToFloat    = refFuncToFloat;
    type->funcToString   = yapValueTypeFuncNotUsed;
    type->funcArithmetic = yapValueTypeFuncNotUsed;
    type->funcCmp        = yapValueTypeFuncNotUsed;
    type->funcIndex      = yapValueTypeFuncNotUsed;
    type->funcDump       = refFuncDump;
    yapValueTypeRegister(Y, type);
    yapAssert(type->id == YVT_REF);
}

// ---------------------------------------------------------------------------

void yapValueTypeRegisterAllBasicTypes(struct yapContext *Y)
{
    nullFuncRegister(Y);
    blockFuncRegister(Y);
    cfunctionFuncRegister(Y);
    intFuncRegister(Y);
    floatFuncRegister(Y);
    stringFuncRegister(Y);
    arrayFuncRegister(Y);
    objectFuncRegister(Y);
    refFuncRegister(Y);
}

// ---------------------------------------------------------------------------

yapValue yapValueNull = {YVT_NULL};
yapValue *yapValueNullPtr = &yapValueNull;

yapValue *yapValueCreateInt(struct yapContext *Y, int v)
{
    yapValue *p = yapValueCreate(Y);
    p->type = YVT_INT;
    p->intVal = v;
    yapTraceValues(("yapValueCreateInt %p [%d]\n", p, v));
    return p;
}

yapValue *yapValueCreateFloat(struct yapContext *Y, yF32 v)
{
    yapValue *p = yapValueCreate(Y);
    p->type = YVT_FLOAT;
    p->floatVal = v;
    yapTraceValues(("yapValueCreateFloat %p [%f]\n", p, v));
    return p;
}

yapValue *yapValueCreateKString(struct yapContext *Y, const char *s)
{
    yapValue *p = yapValueCreate(Y);
    p->type = YVT_STRING;
    yapStringSetK(Y, &p->stringVal, s);
    yapTraceValues(("yapValueCreateKString %p\n", p));
    return p;
}

yapValue *yapValueCreateString(struct yapContext *Y, const char *s)
{
    yapValue *p = yapValueCreate(Y);
    p->type = YVT_STRING;
    yapStringSet(Y, &p->stringVal, s);
    yapTraceValues(("yapValueCreateString %p\n", p));
    return p;
}

yapValue *yapValueDonateString(struct yapContext *Y, char *s)
{
    yapValue *p = yapValueCreate(Y);
    p->type = YVT_STRING;
    yapStringDonate(Y, &p->stringVal, s);
    yapTraceValues(("yapValueDonateString %p\n", p));
    return p;
}

static void yapValueAddClosureVar(yapContext *Y, yapHash *closureVars, yapHashEntry *entry)
{
    yapValueAddRefNote(Y, entry->value, "+ref closure variable");
    yapHashSet(Y, closureVars, entry->key, entry->value);
}

void yapValueAddClosureVars(struct yapContext *Y, yapValue *p)
{
    yapFrame *frame;
    int frameIndex;

    if(p->type != YVT_BLOCK)
    {
        // TODO: add impossibly catastrophic error here? (can't happen?)
        return;
    }

    yapAssert(p->closureVars == NULL);

    for(frameIndex = Y->frames.count - 1; frameIndex >= 0; frameIndex--)
    {
        frame = Y->frames.data[frameIndex];
        if((frame->type & (YFT_CHUNK|YFT_FUNC)) == YFT_FUNC)  // we are inside of an actual function!
        {
            break;
        }
    }

    if(frameIndex >= 0)
    {
        for(; frameIndex < Y->frames.count; frameIndex++)
        {
            frame = Y->frames.data[frameIndex];
            if(frame->locals->count)
            {
                if(!p->closureVars)
                {
                    p->closureVars = yapHashCreate(Y, 0);
                }
                yapHashIterateP1(Y, frame->locals, (yapIterateCB1)yapValueAddClosureVar, p->closureVars);
            }
        }
    }
}

yapValue *yapValueCreateFunction(struct yapContext *Y, struct yapBlock *block)
{
    yapValue *p = yapValueCreate(Y);
    p->type = YVT_BLOCK;
    p->closureVars = NULL;
    p->blockVal = block;
    yapTraceValues(("yapValueCreateFunction %p\n", p));
    return p;
}

yapValue *yapValueCreateCFunction(struct yapContext *Y, yapCFunction func)
{
    yapValue *p = yapValueCreate(Y);
    p->type = YVT_CFUNCTION;
    p->cFuncVal = func;
    yapTraceValues(("yapValueCreateCFunction %p\n", p));
    return p;
}

yapValue *yapValueCreateRef(struct yapContext *Y, struct yapValue **ref)
{
    yapValue *p = yapValueCreate(Y);
    p->type = YVT_REF;
    p->refVal = ref;
    yapTraceValues(("yapValueCreateRef %p\n", p));
    return p;
}

#if 0
yapValue *yapValueCreateObject(struct yapContext *Y, struct yapObject *object)
{
    yapValue *p = yapValueCreate(Y);
    p->type = YVT_OBJECT;
    p->objectVal = object;
    yapTraceValues(("yapValueCreateObject %p\n", p));
    return p;
}
#endif

static yBool yapValueCheckRef(struct yapContext *Y, yapValue *ref, yapValue *p)
{
    if(!p)
    {
        yapContextSetError(Y, YVE_RUNTIME, "yapValueSetRefVal: empty stack!");
        return yFalse;
    }
    if(!ref)
    {
        yapContextSetError(Y, YVE_RUNTIME, "yapValueSetRefVal: empty stack!");
        return yFalse;
    }
    if(!(*ref->refVal))
    {
        yapContextSetError(Y, YVE_RUNTIME, "yapValueSetRefVal: missing ref!");
        return yFalse;
    }
    if(ref->type != YVT_REF)
    {
        yapContextSetError(Y, YVE_RUNTIME, "yapValueSetRefVal: value on top of stack, ref underneath");
        return yFalse;
    }
    return yTrue;
}

yBool yapValueSetRefVal(struct yapContext *Y, yapValue *ref, yapValue *p)
{
    if(!yapValueCheckRef(Y, ref, p))
    {
        return yFalse;
    }

    yapValueRemoveRefNote(Y, *(ref->refVal), "SetRefVal: forgetting previous val");
    *(ref->refVal) = p;
    yapValueAddRefNote(Y, p, "SetRefVal: taking ownership of val");

    yapTraceValues(("yapValueSetRefVal %p = %p\n", ref, p));
    return yTrue;
}

yBool yapValueTestInherits(struct yapContext *Y, yapValue *child, yapValue *parent)
{
    yapValue *p;

    if(child->type != YVT_OBJECT)
    {
        return yFalse;
    }

    if(parent->type != YVT_OBJECT)
    {
        return yFalse;
    }

    p = child->objectVal->isa;
    while(p && (p->type == YVT_OBJECT))
    {
        if(p->objectVal == parent->objectVal)
        {
            return yTrue;
        }

        p = p->objectVal->isa;
    }

    return yFalse;
}

// ---------------------------------------------------------------------------

yapValue *yapValueCreateArray(struct yapContext *Y)
{
    yapValue *p = yapValueCreate(Y);
    p->arrayVal = yapArrayCreate();
    p->type = YVT_ARRAY;
    return p;
}

void yapValueArrayPush(struct yapContext *Y, yapValue *p, yapValue *v)
{
#if 0
    if(p->type != YVT_ARRAY)
    {
        p = yapValuePersonalize(Y, p);
        yapValueClear(Y, p);
        p->arrayVal = yapArrayCreate();
        p->type = YVT_ARRAY;
    }
#endif
    yapAssert(p->type == YVT_ARRAY);

    yapArrayPush(Y, p->arrayVal, v);
}

// ---------------------------------------------------------------------------

yapValue *yapValueCreateObject(struct yapContext *Y, struct yapValue *isa, int argCount, yBool firstArgIsa)
{
    yapValue *p = yapValueCreate(Y);
    if(firstArgIsa)
    {
        yapAssert(argCount);
        yapAssert(isa == NULL);

        if(argCount)
        {
            isa = yapContextGetArg(Y, 0, argCount);
            if(!isa || (isa->type == YVT_NULL))
            {
                isa = NULL;
            }
            if(isa && (isa->type != YVT_OBJECT))
            {
                yapContextSetError(Y, YVE_RUNTIME, "objects can only inherit from objects");
                isa = NULL;
            }
        }
        else
        {
            isa = NULL;
        }
    }
    p->objectVal = yapObjectCreate(Y, isa);
    p->type = YVT_OBJECT;

    if(argCount)
    {
        int i = 0;
        if(firstArgIsa)
        {
            i++;
        }
        for(; i<argCount; i+=2)
        {
            yapValue **ref;
            yapValue *key = yapContextGetArg(Y, i, argCount);
            yapValue *val = yapValueNullPtr;
            int valueArg = i+1;
            if(valueArg < argCount)
            {
                val = yapContextGetArg(Y, valueArg, argCount);
            }
            key = yapValueToString(Y, key);
            ref = yapObjectGetRef(Y, p->objectVal, yapStringSafePtr(&key->stringVal), yTrue);
            *ref = val;
        }
        yapContextPopValues(Y, argCount);
    }
    return p;
}

void yapValueObjectSetMember(struct yapContext *Y, struct yapValue *object, const char *name, struct yapValue *value)
{
    yapValue **ref = NULL;
    yapAssert(object->type == YVT_OBJECT);
    ref = yapObjectGetRef(Y, object->objectVal, name, yTrue);
    yapAssert(ref);
    *ref = value;
}

// ---------------------------------------------------------------------------

void yapValueClear(struct yapContext *Y, yapValue *p)
{
    yapValueTypeSafeCall(p->type, Clear)(Y, p);

    memset(p, 0, sizeof(*p));
    p->type = YVT_NULL;
}

#if 0
void yapValueRelease(struct yapContext *Y, yapValue *p)
{
    yapTraceValues(("yapValueRelease %p\n", p));
    yapValueClear(Y, p);
    yapArrayPush(Y, &Y->freeValues, p);
}
#endif

yapValue *yapValueCreate(yapContext *Y)
{
    yapValue *value = yapAlloc(sizeof(yapValue));
    yapValueTraceRefs(Y, value, 1, "yapValueCreate");
    value->refs = 1;
    yapTraceValues(("yapValueCreate %p\n", value));
    return value;
}

void yapValueDestroy(struct yapContext *Y, yapValue *p)
{
    yapTraceValues(("yapValueFree %p\n", p));
    yapValueClear(Y, p);
    yapFree(p);
}

void yapValueAddRef(struct yapContext *Y, yapValue *p)
{
    if(p == yapValueNullPtr)
    {
        return;
    }
    
    yapAssert(p->refs);
    ++p->refs;
}

void yapValueRemoveRef(struct yapContext *Y, yapValue *p)
{
    if(p == yapValueNullPtr)
    {
        return;
    }

    --p->refs;
    if(p->refs == 0)
    {
        yapValueDestroy(Y, p);
    }
}

yS32 yapValueCmp(struct yapContext *Y, yapValue *a, yapValue *b)
{
    if(a == b)
    {
        return 0;    // this should also handle the NULL case
    }

    if(a && b)
    {
        yS32 ret = 0;
        if(yapValueTypeSafeCall(a->type, Cmp)(Y, a, b, &ret))
        {
            return ret;
        }
    }

    return (yS32)(a - b); // Fallback case: compare pointers for consistency
}

void yapValueCloneData(struct yapContext *Y, yapValue *dst, yapValue *src)
{
    dst->type = src->type;
    dst->constant = src->constant;
    yapValueTypeSafeCall(dst->type, Clone)(Y, dst, src);
}

yapValue *yapValueClone(struct yapContext *Y, yapValue *p)
{
    yapValue *n = yapValueCreate(Y);
    yapValueCloneData(Y, n, p);
    yapTraceValues(("yapValueClone %p -> %p\n", p, n));
    return n;
}

yapValue *yapValueAdd(struct yapContext *Y, yapValue *a, yapValue *b)
{
    yapValue *value = yapValueTypeSafeCall(a->type, Arithmetic)(Y, a, b, YVAO_ADD);
    if(!value)
    {
        yapTraceValues(("Don't know how to add types %s and %s\n", yapValueTypePtr(a->type)->name, yapValueTypePtr(b->type)->name));
    }
    return value;
}

yapValue *yapValueSub(struct yapContext *Y, yapValue *a, yapValue *b)
{
    yapValue *value = yapValueTypeSafeCall(a->type, Arithmetic)(Y, a, b, YVAO_SUB);
    if(!value)
    {
        yapTraceValues(("Don't know how to subtract types %s and %s\n", yapValueTypePtr(a->type)->name, yapValueTypePtr(b->type)->name));
    }
    return value;
}

yapValue *yapValueMul(struct yapContext *Y, yapValue *a, yapValue *b)
{
    yapValue *value = yapValueTypeSafeCall(a->type, Arithmetic)(Y, a, b, YVAO_MUL);
    if(!value)
    {
        yapTraceValues(("Don't know how to multiply types %s and %s\n", yapValueTypePtr(a->type)->name, yapValueTypePtr(b->type)->name));
    }
    return value;
}

yapValue *yapValueDiv(struct yapContext *Y, yapValue *a, yapValue *b)
{
    yapValue *value = yapValueTypeSafeCall(a->type, Arithmetic)(Y, a, b, YVAO_DIV);
    if(!value)
    {
        yapTraceValues(("Don't know how to divide types %s and %s\n", yapValueTypePtr(a->type)->name, yapValueTypePtr(b->type)->name));
    }
    return value;
}

yapValue *yapValueToBool(struct yapContext *Y, yapValue *p)
{
    yBool boolVal = yapValueTypeSafeCall(p->type, ToBool)(Y, p);
    yapValue *value = yapValueCreateInt(Y, boolVal);
    yapValueRemoveRefNote(Y, p, "yapValueToBool");
    return value;
}

yapValue *yapValueToInt(struct yapContext *Y, yapValue *p)
{
    yS32 intVal = yapValueTypeSafeCall(p->type, ToInt)(Y, p);
    yapValue *ret = yapValueCreateInt(Y, intVal);
    yapValueRemoveRefNote(Y, p, "yapValueToInt");
    return ret;
}

yapValue *yapValueToFloat(struct yapContext *Y, yapValue *p)
{
    yF32 floatVal = yapValueTypeSafeCall(p->type, ToFloat)(Y, p);
    yapValue *ret = yapValueCreateFloat(Y, floatVal);
    yapValueRemoveRefNote(Y, p, "yapValueToFloat");
    return ret;
}

yapValue *yapValueToString(struct yapContext *Y, yapValue *p)
{
    yapValue *value = yapValueTypeSafeCall(p->type, ToString)(Y, p);
    if(!value)
    {
        yapTraceExecution(("yapValueToString: unable to convert type '%s' to string\n", yapValueTypePtr(p->type)->name));
    }
    yapValueRemoveRefNote(Y, p, "yapValueToString");
    return value;
}

yapValue *yapValueStringFormat(struct yapContext *Y, yapValue *format, yS32 argCount)
{
    char *curr = (char *)yapStringSafePtr(&format->stringVal);
    char *next;

    yapValue *arg;
    int argIndex = 0;

    yapValue *ret = yapValueCreateString(Y, "");
    yapString *str = &ret->stringVal;

    while(curr && (next = strchr(curr, '%')))
    {
        // First, add in all of the stuff before the %
        yapStringConcatLen(Y, str, curr, (int)(next - curr));
        next++;

        switch(*next)
        {
            case '\0':
                curr = NULL;
                break;
            case '%':
                yapStringConcatLen(Y, str, "%", 1);
                break;
            case 's':
                arg = yapContextGetArg(Y, argIndex++, argCount);
                if(arg)
                {
                    arg = yapValueToString(Y, arg);
                    yapStringConcatStr(Y, str, &arg->stringVal);
                }
                break;
            case 'd':
                arg = yapContextGetArg(Y, argIndex++, argCount);
                if(arg)
                {
                    char temp[32];
                    arg = yapValueToInt(Y, arg);
                    sprintf(temp, "%d", arg->intVal);
                    yapStringConcat(Y, str, temp);
                }
                break;
            case 'f':
                arg = yapContextGetArg(Y, argIndex++, argCount);
                if(arg)
                {
                    char temp[32];
                    arg = yapValueToFloat(Y, arg);
                    sprintf(temp, "%f", arg->floatVal);
                    yapStringConcat(Y, str, temp);
                }
                break;
        };

        curr = next + 1;
    }

    // Add the remainder of the string, if any
    if(curr)
    {
        yapStringConcat(Y, str, curr);
    }

    yapContextPopValues(Y, argCount);
    return ret;
}

yapValue *yapValueIndex(struct yapContext *Y, yapValue *p, yapValue *index, yBool lvalue)
{
    return yapValueTypeSafeCall(p->type, Index)(Y, p, index, lvalue);
}

const char *yapValueTypeName(struct yapContext *Y, int type)
{
    if((type >= 0) && (type < Y->types.count))
    {
        yapValueType *valueType = ((yapValueType *)Y->types.data[type]);
        return valueType->name;
    }
    return "unknown";
}

void yapValueDump(struct yapContext *Y, yapDumpParams *params, yapValue *p)
{
    yapValueTypeSafeCall(p->type, Dump)(Y, params, p);
}

#ifdef YAP_TRACE_REFS
void yapValueTraceRefs(struct yapContext *Y, struct yapValue *p, int delta, const char *note)
{
    const char *destroyed = "";
    char tempPtr[32];
    int newRefs = p->refs;
    if(p == yapValueNullPtr)
    {
        sprintf(tempPtr, "0xYAP_NULL");
        note = "-- ignoring yapValueNull --";
    }
    else
    {
        sprintf(tempPtr, "%10p", p);
        newRefs += delta;
        if((delta < 0) && !newRefs)
        {
            destroyed = " - DESTROYED";
        }
    }
    yapTraceRefs(("\t\t\t\t\t\t\t\tREFS: yapValue %s [%2d delta -> %d]: %s%s\n", tempPtr, delta, newRefs, note, destroyed));
}

void yapValueRemoveRefArray(struct yapContext *Y, struct yapValue *p)
{
    yapValueTraceRefs(Y, p, -1, "array cleared");
    yapValueRemoveRef(Y, p);
}

void yapValueRemoveRefHashed(struct yapContext *Y, struct yapValue *p)
{
    yapValueTraceRefs(Y, p, -1, "hash cleared");
    yapValueRemoveRef(Y, p);
}
#endif
