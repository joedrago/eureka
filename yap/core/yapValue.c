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
#include "yapVariable.h"
#include "yapContext.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

static char *NULL_STRING_FORM = "(null)";

// ---------------------------------------------------------------------------
// Helper funcs

yapClosureVariable *yapClosureVariableCreate(struct yapContext *Y, const char *name, yapVariable *variable)
{
    yapClosureVariable *cv = yapAlloc(sizeof(yapClosureVariable));
    cv->name = yapStrdup(Y, name);
    cv->variable = variable;
    return cv;
}

void yapClosureVariableDestroy(struct yapContext *Y, yapClosureVariable *cv)
{
    free(cv->name);
    free(cv);
}

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
        type->funcDestroyUserData(Y, type);
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
    yapAssert(newType->funcMark);
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

static struct yapValue * nullFuncToString(struct yapContext *Y, struct yapValue *p)
{
    return yapValueSetKString(Y, yapValueAcquire(Y), NULL_STRING_FORM);
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
    type->funcMark       = yapValueTypeFuncNotUsed;
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
        yapArrayDestroy(Y, p->closureVars, (yapDestroyCB)yapClosureVariableDestroy);
}

static void blockFuncClone(struct yapContext *Y, struct yapValue *dst, struct yapValue *src)
{
    dst->blockVal = src->blockVal;
}

static void blockFuncMark(struct yapContext *Y, struct yapValue *value)
{
    if(value->closureVars)
    {
        int i;
        for(i=0; i<value->closureVars->count; i++)
        {
            yapClosureVariable *cv = (yapClosureVariable *)value->closureVars->data[i];
            yapVariableMark(Y, cv->variable);
        }
    }
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
    type->funcMark       = blockFuncMark;
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
    type->funcMark       = yapValueTypeFuncNotUsed;
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

static struct yapValue * intFuncToString(struct yapContext *Y, struct yapValue *p)
{
    char temp[32];
    sprintf(temp, "%d", p->intVal);
    return yapValueSetString(Y, p, temp);
}

static struct yapValue * intFuncArithmetic(struct yapContext *Y, struct yapValue *a, struct yapValue *b, yapValueArithmeticOp op)
{
    b = yapValueToInt(Y, b);
    switch(op)
    {
    case YVAO_ADD:
        a = yapValueSetInt(Y, a, a->intVal + b->intVal);
        break;
    case YVAO_SUB:
        a = yapValueSetInt(Y, a, a->intVal - b->intVal);
        break;
    case YVAO_MUL:
        a = yapValueSetInt(Y, a, a->intVal * b->intVal);
        break;
    case YVAO_DIV:
        if(!b->intVal)
        {
            yapContextSetError(Y, YVE_RUNTIME, "divide by zero!");
            return NULL;
        }
        a = yapValueSetInt(Y, a, a->intVal / b->intVal);
        break;
    };
    return a;
}

static yBool intFuncCmp(struct yapContext *Y, struct yapValue *a, struct yapValue *b, int *cmpResult)
{
    if(b->type == YVT_FLOAT)
    {
        if((yF32)a->intVal > b->floatVal)
            *cmpResult = 1;
        else if((yF32)a->intVal < b->floatVal)
            *cmpResult = -1;
        else
            *cmpResult = 0;
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
    type->funcMark       = yapValueTypeFuncNotUsed;
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

static struct yapValue * floatFuncToString(struct yapContext *Y, struct yapValue *p)
{
    char temp[64];
    sprintf(temp, "%f", p->floatVal);
    return yapValueSetString(Y, p, temp);
}

static struct yapValue * floatFuncArithmetic(struct yapContext *Y, struct yapValue *a, struct yapValue *b, yapValueArithmeticOp op)
{
    b = yapValueToFloat(Y, b);
    switch(op)
    {
    case YVAO_ADD:
        a = yapValueSetFloat(Y, a, a->floatVal + b->floatVal);
        break;
    case YVAO_SUB:
        a = yapValueSetFloat(Y, a, a->floatVal - b->floatVal);
        break;
    case YVAO_MUL:
        a = yapValueSetFloat(Y, a, a->floatVal * b->floatVal);
        break;
    case YVAO_DIV:
        if(b->floatVal == 0.0f)
        {
            yapContextSetError(Y, YVE_RUNTIME, "divide by zero!");
            return NULL;
        }
        a = yapValueSetFloat(Y, a, a->floatVal / b->floatVal);
        break;
    };
    return a;
}

static yBool floatFuncCmp(struct yapContext *Y, struct yapValue *a, struct yapValue *b, int *cmpResult)
{
    if(b->type == YVT_INT)
    {
        if(a->floatVal > (yF32)b->intVal)
            *cmpResult = 1;
        else if(a->floatVal < (yF32)b->intVal)
            *cmpResult = -1;
        else
            *cmpResult = 0;
        return yTrue;
    }
    else if(b->type == YVT_FLOAT)
    {
        if(a->floatVal > b->floatVal)
            *cmpResult = 1;
        else if(a->floatVal < b->floatVal)
            *cmpResult = -1;
        else
            *cmpResult = 0;
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
    type->funcMark       = yapValueTypeFuncNotUsed;
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

struct yapValue * stringFuncToString(struct yapContext *Y, struct yapValue *p)
{
    return p;
}

struct yapValue * stringFuncArithmetic(struct yapContext *Y, struct yapValue *a, struct yapValue *b, yapValueArithmeticOp op)
{
    yapValue *ret = NULL;
    if(op == YVAO_ADD)
    {
        a = yapValueToString(Y, a);
        b = yapValueToString(Y, b);
        ret = yapValueSetString(Y, yapValueAcquire(Y), yapStringSafePtr(&a->stringVal));
        yapStringConcatStr(Y, &ret->stringVal, &a->stringVal);
    }
    else
    {
        printf("stringFuncArithmetic(): cannot subtract, multiply, or divide strings!\n");
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
    type->funcMark       = yapValueTypeFuncNotUsed;
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
    yapArrayDestroy(Y, p->arrayVal, NULL);
}

static void arrayFuncClone(struct yapContext *Y, struct yapValue *dst, struct yapValue *src)
{
    yapAssert(0 && "arrayFuncClone not implemented");
}

static void arrayFuncMark(struct yapContext *Y, struct yapValue *value)
{
    int i;
    for(i = 0; i < value->arrayVal->count; i++)
    {
        yapValue *child = (yapValue *)value->arrayVal->data[i];
        yapValueMark(Y, child);
    }
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

static struct yapValue * arrayFuncIndex(struct yapContext *Y, struct yapValue *value, struct yapValue *index, yBool lvalue)
{
    yapValue *ret = NULL;
    yapValue **ref = NULL;
    index = yapValueToInt(Y, index);
    if(index->intVal >= 0 && index->intVal < value->arrayVal->count)
    {
        ref = (yapValue **) & (value->arrayVal->data[index->intVal]);
        if(lvalue)
            ret = yapValueSetRef(Y, yapValueAcquire(Y), ref);
        else
            ret = *ref;
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
            yapStringConcat(Y, &params->output, ", ");
        yapValueTypeSafeCall(child->type, Dump)(Y, params, child);
    }
    yapStringConcat(Y, &params->output, " ]");
}

static void arrayFuncRegister(struct yapContext *Y)
{
    yapValueType *type = yapValueTypeCreate(Y, "array");
    type->funcClear      = arrayFuncClear;
    type->funcClone      = arrayFuncClone;
    type->funcMark       = arrayFuncMark;
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

static void objectFuncMark(struct yapContext *Y, struct yapValue *value)
{
    yapObjectMark(Y, value->objectVal);
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

static struct yapValue * objectFuncToString(struct yapContext *Y, struct yapValue *p)
{
    char temp[32];
    sprintf(temp, "[object:%p]", p->objectVal);
    return yapValueSetString(Y, yapValueAcquire(Y), temp);
}

static struct yapValue * objectFuncIndex(struct yapContext *Y, struct yapValue *value, struct yapValue *index, yBool lvalue)
{
    yapValue *ret = NULL;
    yapValue **ref = NULL;
    index = yapValueToString(Y, index);
    ref = yapObjectGetRef(Y, value->objectVal, yapStringSafePtr(&index->stringVal), lvalue /* create? */);
    if(lvalue)
        ret = yapValueSetRef(Y, yapValueAcquire(Y), ref);
    else
        ret = *ref;
    return ret;
}

void appendKeys(struct yapContext *Y, yapDumpParams *params, yapHashEntry *entry)
{
    yapValue *child = (yapValue *)entry->value;
    if(params->tempInt)
        yapStringConcat(Y, &params->output, "\"");
    else
        yapStringConcat(Y, &params->output, ", \"");

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
    type->funcMark       = objectFuncMark;
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

static void refFuncMark(struct yapContext *Y, struct yapValue *value)
{
    if(*value->refVal)
    {
        yapValueMark(Y, *value->refVal);
    }
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
    type->funcMark       = refFuncMark;
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

yapValue *yapValueSetInt(struct yapContext *Y, yapValue *p, int v)
{
    p = yapValuePersonalize(Y, p);
    yapValueClear(Y, p);
    p->type = YVT_INT;
    p->intVal = v;
    p->used = yTrue;
    yapTrace(("yapValueSetInt %p [%d]\n", p, v));
    return p;
}

yapValue *yapValueSetFloat(struct yapContext *Y, yapValue *p, yF32 v)
{
    p = yapValuePersonalize(Y, p);
    yapValueClear(Y, p);
    p->type = YVT_FLOAT;
    p->floatVal = v;
    p->used = yTrue;
    yapTrace(("yapValueSetFloat %p [%f]\n", p, v));
    return p;
}

yapValue *yapValueSetKString(struct yapContext *Y, yapValue *p, const char *s)
{
    p = yapValuePersonalize(Y, p);
    yapValueClear(Y, p);
    p->type = YVT_STRING;
    yapStringSetK(Y, &p->stringVal, s);
    p->used = yTrue;
    yapTrace(("yapValueSetKString %p\n", p));
    return p;
}

yapValue *yapValueSetString(struct yapContext *Y, yapValue *p, const char *s)
{
    p = yapValuePersonalize(Y, p);
    yapValueClear(Y, p);
    p->type = YVT_STRING;
    yapStringSet(Y, &p->stringVal, s);
    p->used = yTrue;
    yapTrace(("yapValueSetString %p\n", p));
    return p;
}

yapValue *yapValueDonateString(struct yapContext *Y, yapValue *p, char *s)
{
    p = yapValuePersonalize(Y, p);
    yapValueClear(Y, p);
    p->type = YVT_STRING;
    yapStringDonate(Y, &p->stringVal, s);
    p->used = yTrue;
    yapTrace(("yapValueDonateString %p\n", p));
    return p;
}

static void yapValueAddClosureVar(yapContext *Y, yapArray *closureVars, yapHashEntry *entry)
{
    yapClosureVariable *cv = yapClosureVariableCreate(Y, entry->key, entry->value);
    yapArrayPush(Y, closureVars, cv);
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
        if((frame->type & (YFT_CHUNK|YFT_FUNC)) == YFT_FUNC) // we are inside of an actual function!
            break;
    }

    if(frameIndex >= 0)
    {
        for( ; frameIndex < Y->frames.count; frameIndex++)
        {
            frame = Y->frames.data[frameIndex];
            if(frame->locals->count)
            {
                int i;
                if(!p->closureVars)
                    p->closureVars = yapArrayCreate();
                yapHashIterateP1(Y, frame->locals, (yapIterateCB1)yapValueAddClosureVar, p->closureVars);
            }
        }
    }
}

yapValue *yapValueSetFunction(struct yapContext *Y, yapValue *p, struct yapBlock *block)
{
    p = yapValuePersonalize(Y, p);
    yapValueClear(Y, p);
    p->type = YVT_BLOCK;
    p->closureVars = NULL;
    p->blockVal = block;
    p->used = yTrue;
    yapTrace(("yapValueSetFunction %p\n", p));
    return p;
}

yapValue *yapValueSetCFunction(struct yapContext *Y, yapValue *p, yapCFunction func)
{
    p = yapValuePersonalize(Y, p);
    yapValueClear(Y, p);
    p->type = YVT_CFUNCTION;
    p->cFuncVal = func;
    p->used = yTrue;
    yapTrace(("yapValueSetCFunction %p\n", p));
    return p;
}

yapValue *yapValueSetRef(struct yapContext *Y, yapValue *p, struct yapValue **ref)
{
    p = yapValuePersonalize(Y, p);
    yapValueClear(Y, p);
    p->type = YVT_REF;
    p->refVal = ref;
    p->used = yTrue;
    yapTrace(("yapValueSetRef %p\n", p));
    return p;
}

yapValue *yapValueSetObject(struct yapContext *Y, yapValue *p, struct yapObject *object)
{
    p = yapValuePersonalize(Y, p);
    yapValueClear(Y, p);
    p->type = YVT_OBJECT;
    p->objectVal = object;
    p->used = yTrue;
    yapTrace(("yapValueSetObject %p\n", p));
    return p;
}

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
        return yFalse;

    *(ref->refVal) = p;
    p->used = yTrue;

    yapTrace(("yapValueSetRefVal %p = %p\n", ref, p));
    return yTrue;
}

yBool yapValueSetRefInherits(struct yapContext *Y, yapValue *ref, yapValue *p)
{
    if(!yapValueCheckRef(Y, ref, p))
        return yFalse;

    if(*(ref->refVal) == &yapValueNull)
    {
        *(ref->refVal) = yapValueObjectCreate(Y, p, 0);
    }

    if((*(ref->refVal))->type != YVT_OBJECT)
    {
        yapContextSetError(Y, YVE_RUNTIME, "yapValueSetRefInherits: non-objects cannot inherit");
        return yFalse;
    }

    (*(ref->refVal))->objectVal->isa = p;
    p->used = yTrue;

    yapTrace(("yapValueSetRefInherits %p = %p\n", ref, p));
    return yTrue;
}

// ---------------------------------------------------------------------------

yapValue *yapValueArrayCreate(struct yapContext *Y)
{
    yapValue *p = yapValueAcquire(Y);
    p->arrayVal = yapArrayCreate();
    p->type = YVT_ARRAY;
    p->used = yTrue;
    return p;
}

void yapValueArrayPush(struct yapContext *Y, yapValue *p, yapValue *v)
{
    if(p->type != YVT_ARRAY)
    {
        p = yapValuePersonalize(Y, p);
        yapValueClear(Y, p);
        p->arrayVal = yapArrayCreate();
        p->type = YVT_ARRAY;
    }

    yapArrayPush(Y, p->arrayVal, v);
    v->used = yTrue;
}

// ---------------------------------------------------------------------------

yapValue *yapValueObjectCreate(struct yapContext *Y, struct yapValue *isa, int argCount)
{
    yapValue *p = yapValueAcquire(Y);
    p->objectVal = yapObjectCreate(Y, isa);
    p->type = YVT_OBJECT;
    p->used = yTrue;

    if(argCount)
    {
        int i;
        for(i=0; i<argCount; i+=2)
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

void yapValueRelease(struct yapContext *Y, yapValue *p)
{
    yapTrace(("yapValueRelease %p\n", p));
    yapValueClear(Y, p);
    yapArrayPush(Y, &Y->freeValues, p);
}

void yapValueDestroy(struct yapContext *Y, yapValue *p)
{
    yapTrace(("yapValueFree %p\n", p));
    yapValueClear(Y, p);
    yapFree(p);
}

static yapValue *yapValueCreate(yapContext *Y)
{
    yapValue *value = yapAlloc(sizeof(yapValue));
    yapTrace(("yapValueCreate %p\n", value));
    return value;
}

yapValue *yapValueAcquire(struct yapContext *Y)
{
    yapValue *value = yapArrayPop(Y, &Y->freeValues);
    if(!value)
    {
        value = yapValueCreate(Y);
    };
    yapArrayPush(Y, &Y->usedValues, value);
    yapTrace(("yapValueAcquire %p\n", value));
    return value;
}

yS32 yapValueCmp(struct yapContext *Y, yapValue *a, yapValue *b)
{
    if(a == b)
        return 0; // this should also handle the NULL case

    if(a && b)
    {
        yS32 ret = 0;
        if(yapValueTypeSafeCall(a->type, Cmp)(Y, a, b, &ret))
            return ret;
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
    yapValue *n = yapValueAcquire(Y);
    yapValueCloneData(Y, n, p);
    yapTrace(("yapValueClone %p -> %p\n", p, n));
    return n;
}

yapValue *yapValuePersonalize(struct yapContext *Y, yapValue *p)
{
    if(p == &yapValueNull)
        return yapValueAcquire(Y);

    if(p->used)
        return yapValueClone(Y, p);

    return p;
}

void yapValueMark(struct yapContext *Y, yapValue *value)
{
    if(value->type == YVT_NULL)
        return;

    if(value->used)
        return;

    value->used = yTrue;

    yapValueTypeSafeCall(value->type, Mark)(Y, value);
}

yapValue *yapValueAdd(struct yapContext *Y, yapValue *a, yapValue *b)
{
    yapValue *value = yapValueTypeSafeCall(a->type, Arithmetic)(Y, a, b, YVAO_ADD);
    if(!value)
        yapTrace(("Don't know how to add types %s and %s\n", yapValueTypePtr(a->type)->name, yapValueTypePtr(b->type)->name));
    return value;
}

yapValue *yapValueSub(struct yapContext *Y, yapValue *a, yapValue *b)
{
    yapValue *value = yapValueTypeSafeCall(a->type, Arithmetic)(Y, a, b, YVAO_SUB);
    if(!value)
        yapTrace(("Don't know how to subtract types %s and %s\n", yapValueTypePtr(a->type)->name, yapValueTypePtr(b->type)->name));
    return value;
}

yapValue *yapValueMul(struct yapContext *Y, yapValue *a, yapValue *b)
{
    yapValue *value = yapValueTypeSafeCall(a->type, Arithmetic)(Y, a, b, YVAO_MUL);
    if(!value)
        yapTrace(("Don't know how to multiply types %s and %s\n", yapValueTypePtr(a->type)->name, yapValueTypePtr(b->type)->name));
    return value;
}

yapValue *yapValueDiv(struct yapContext *Y, yapValue *a, yapValue *b)
{
    yapValue *value = yapValueTypeSafeCall(a->type, Arithmetic)(Y, a, b, YVAO_DIV);
    if(!value)
        yapTrace(("Don't know how to divide types %s and %s\n", yapValueTypePtr(a->type)->name, yapValueTypePtr(b->type)->name));
    return value;
}

yapValue *yapValueToBool(struct yapContext *Y, yapValue *p)
{
    yBool boolVal = yapValueTypeSafeCall(p->type, ToBool)(Y, p);
    return yapValueSetInt(Y, yapValueAcquire(Y), boolVal);
}

yapValue *yapValueToInt(struct yapContext *Y, yapValue *p)
{
    yS32 intVal = yapValueTypeSafeCall(p->type, ToInt)(Y, p);
    return yapValueSetInt(Y, yapValueAcquire(Y), intVal);
}

yapValue *yapValueToFloat(struct yapContext *Y, yapValue *p)
{
    yF32 floatVal = yapValueTypeSafeCall(p->type, ToFloat)(Y, p);
    return yapValueSetFloat(Y, yapValueAcquire(Y), floatVal);
}

yapValue *yapValueToString(struct yapContext *Y, yapValue *p)
{
    yapValue *value = yapValueTypeSafeCall(p->type, ToString)(Y, p);
    if(!value)
    {
        printf("yapValueToString: unable to convert type '%s' to string\n", yapValueTypePtr(p->type)->name);
    }
    return value;
}

yapValue *yapValueStringFormat(struct yapContext *Y, yapValue *format, yS32 argCount)
{
    char *curr = (char *)yapStringSafePtr(&format->stringVal);
    char *next;

    yapValue *arg;
    int argIndex = 0;

    yapValue *ret = yapValueSetString(Y, yapValueAcquire(Y), "");
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
        yapStringConcat(Y, str, curr);

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
        yapValueType *valueType = ((yapValueType*)Y->types.data[type]);
        return valueType->name;
    }
    return "unknown";
}

void yapValueDump(struct yapContext *Y, yapDumpParams *params, yapValue *p)
{
    yapValueTypeSafeCall(p->type, Dump)(Y, params, p);
}
