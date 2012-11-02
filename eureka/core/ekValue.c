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

static char *NULL_STRING_FORM = "(null)";

// ---------------------------------------------------------------------------

ekDumpParams *ekDumpParamsCreate(struct ekContext *Y)
{
    ekDumpParams *params = (ekDumpParams *)ekAlloc(sizeof(ekDumpParams));
    return params;
}

void ekDumpParamsDestroy(struct ekContext *Y, ekDumpParams *params)
{
    ekStringClear(Y, &params->output);
    ekStringClear(Y, &params->tempStr);
    ekFree(params);
}

// ---------------------------------------------------------------------------

ekValueType *ekValueTypeCreate(struct ekContext *Y, const char *name)
{
    ekValueType *type = ekAlloc(sizeof(ekValueType));
    strcpy(type->name, name);
    return type;
}

void ekValueTypeDestroy(struct ekContext *Y, ekValueType *type)
{
    if(type->funcDestroyUserData)
    {
        type->funcDestroyUserData(Y, type);
    }
    ekFree(type);
}

int ekValueTypeRegister(struct ekContext *Y, ekValueType *newType)
{
    int i;
    for(i=0; i<ekArraySize(Y, &Y->types); i++)
    {
        ekValueType *t = Y->types[i];
        if(!strcmp(newType->name, t->name))
        {
            return YVT_INVALID;
        }
    }

    // If you are hitting one of these asserts, then your custom type isn't handling a required function.
    // If you're a C++ person, pretend the compiler is telling you that you forgot to implement a pure virtual.
    // If you don't want to do anything for a particular function, explicitly set it to ekValueTypeFuncNotUsed.
    ekAssert(newType->funcClear);
    ekAssert(newType->funcClone);
    ekAssert(newType->funcToBool);
    ekAssert(newType->funcToInt);
    ekAssert(newType->funcToFloat);
    ekAssert(newType->funcToString);
    ekAssert(newType->funcArithmetic);
    ekAssert(newType->funcCmp);
    ekAssert(newType->funcIndex);
    ekAssert(newType->funcDump);
    ekAssert(newType->funcDump != ekValueTypeFuncNotUsed); // required!

    newType->id = ekArrayPush(Y, &Y->types, newType);
    return newType->id;
}

// ---------------------------------------------------------------------------
// YVT_NULL Funcs

static struct ekValue *nullFuncToString(struct ekContext *Y, struct ekValue *p)
{
    return ekValueCreateKString(Y, NULL_STRING_FORM);
}

static void nullFuncDump(struct ekContext *Y, ekDumpParams *params, struct ekValue *p)
{
    ekStringConcat(Y, &params->output, NULL_STRING_FORM);
}

static void nullFuncRegister(struct ekContext *Y)
{
    ekValueType *type   = ekValueTypeCreate(Y, "null");
    type->funcClear      = ekValueTypeFuncNotUsed;
    type->funcClone      = ekValueTypeFuncNotUsed;
    type->funcToBool     = ekValueTypeFuncNotUsed;
    type->funcToInt      = ekValueTypeFuncNotUsed;
    type->funcToFloat    = ekValueTypeFuncNotUsed;
    type->funcToString   = nullFuncToString;
    type->funcArithmetic = ekValueTypeFuncNotUsed;
    type->funcCmp        = ekValueTypeFuncNotUsed;
    type->funcIndex      = ekValueTypeFuncNotUsed;
    type->funcDump       = nullFuncDump;
    ekValueTypeRegister(Y, type);
    ekAssert(type->id == YVT_NULL);
}

// ---------------------------------------------------------------------------
// YVT_BLOCK Funcs

static void blockFuncClear(struct ekContext *Y, struct ekValue *p)
{
    if(p->closureVars)
    {
        ekMapDestroy(Y, p->closureVars, ekValueRemoveRef);
    }
}

static void blockFuncClone(struct ekContext *Y, struct ekValue *dst, struct ekValue *src)
{
    dst->blockVal = src->blockVal;
}

static yBool blockFuncToBool(struct ekContext *Y, struct ekValue *p)
{
    return yTrue;
}

static yS32 blockFuncToInt(struct ekContext *Y, struct ekValue *p)
{
    return 1; // ?
}

static yF32 blockFuncToFloat(struct ekContext *Y, struct ekValue *p)
{
    return 1.0f; // ?
}

static void blockFuncDump(struct ekContext *Y, ekDumpParams *params, struct ekValue *p)
{
    char temp[64];
    sprintf(temp, "(block:0x%p)", p->blockVal);
    ekStringConcat(Y, &params->output, temp);
}

static void blockFuncRegister(struct ekContext *Y)
{
    ekValueType *type = ekValueTypeCreate(Y, "block");
    type->funcClear      = blockFuncClear;
    type->funcClone      = blockFuncClone;
    type->funcToBool     = blockFuncToBool;
    type->funcToInt      = blockFuncToInt;
    type->funcToFloat    = blockFuncToFloat;
    type->funcToString   = ekValueTypeFuncNotUsed;
    type->funcArithmetic = ekValueTypeFuncNotUsed;
    type->funcCmp        = ekValueTypeFuncNotUsed;
    type->funcIndex      = ekValueTypeFuncNotUsed;
    type->funcDump       = blockFuncDump;
    ekValueTypeRegister(Y, type);
    ekAssert(type->id == YVT_BLOCK);
}

// ---------------------------------------------------------------------------
// YVT_CFUNCTION Funcs

static void cfunctionFuncClone(struct ekContext *Y, struct ekValue *dst, struct ekValue *src)
{
    dst->cFuncVal = src->cFuncVal;
}

static yBool cfunctionFuncToBool(struct ekContext *Y, struct ekValue *p)
{
    return yTrue;
}

static yS32 cfunctionFuncToInt(struct ekContext *Y, struct ekValue *p)
{
    return 1; // ?
}

static yF32 cfunctionFuncToFloat(struct ekContext *Y, struct ekValue *p)
{
    return 1.0f; // ?
}

static void cfunctionFuncDump(struct ekContext *Y, ekDumpParams *params, struct ekValue *p)
{
    char temp[64];
    sprintf(temp, "(cfunction:0x%p)", p->cFuncVal);
    ekStringConcat(Y, &params->output, temp);
}

static void cfunctionFuncRegister(struct ekContext *Y)
{
    ekValueType *type = ekValueTypeCreate(Y, "cfunction");
    type->funcClear      = ekValueTypeFuncNotUsed;
    type->funcClone      = cfunctionFuncClone;
    type->funcToBool     = cfunctionFuncToBool;
    type->funcToInt      = cfunctionFuncToInt;
    type->funcToFloat    = cfunctionFuncToFloat;
    type->funcToString   = ekValueTypeFuncNotUsed;
    type->funcArithmetic = ekValueTypeFuncNotUsed;
    type->funcCmp        = ekValueTypeFuncNotUsed;
    type->funcIndex      = ekValueTypeFuncNotUsed;
    type->funcDump       = cfunctionFuncDump;
    ekValueTypeRegister(Y, type);
    ekAssert(type->id == YVT_CFUNCTION);
}

// ---------------------------------------------------------------------------
// YVT_INT Funcs

static void intFuncClone(struct ekContext *Y, struct ekValue *dst, struct ekValue *src)
{
    dst->intVal = src->intVal;
}

static yBool intFuncToBool(struct ekContext *Y, struct ekValue *p)
{
    return (p->intVal) ? yTrue : yFalse;
}

static yS32 intFuncToInt(struct ekContext *Y, struct ekValue *p)
{
    return p->intVal;
}

static yF32 intFuncToFloat(struct ekContext *Y, struct ekValue *p)
{
    return (yF32)p->intVal;
}

static struct ekValue *intFuncToString(struct ekContext *Y, struct ekValue *p)
{
    char temp[32];
    sprintf(temp, "%d", p->intVal);
    ekValueRemoveRefNote(Y, p, "intFuncToString doesnt need int anymore");
    return ekValueCreateString(Y, temp);
}

static struct ekValue *intFuncArithmetic(struct ekContext *Y, struct ekValue *a, struct ekValue *b, ekValueArithmeticOp op)
{
    ekValue *c = NULL;
    ekValueAddRefNote(Y, b, "intFuncArithmetic keep b during int conversion");
    b = ekValueToInt(Y, b);
    switch(op)
    {
        case YVAO_ADD:
            c = ekValueCreateInt(Y, a->intVal + b->intVal);
            break;
        case YVAO_SUB:
            c = ekValueCreateInt(Y, a->intVal - b->intVal);
            break;
        case YVAO_MUL:
            c = ekValueCreateInt(Y, a->intVal * b->intVal);
            break;
        case YVAO_DIV:
            if(!b->intVal)
            {
                ekContextSetError(Y, YVE_RUNTIME, "divide by zero!");
            }
            else
            {
                c = ekValueCreateInt(Y, a->intVal / b->intVal);
            }
            break;
    };
    ekValueRemoveRefNote(Y, b, "intFuncArithmetic temp b done");
    return c;
}

static yBool intFuncCmp(struct ekContext *Y, struct ekValue *a, struct ekValue *b, int *cmpResult)
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

static void intFuncDump(struct ekContext *Y, ekDumpParams *params, struct ekValue *p)
{
    char temp[64];
    sprintf(temp, "%d", p->intVal);
    ekStringConcat(Y, &params->output, temp);
}

static void intFuncRegister(struct ekContext *Y)
{
    ekValueType *type = ekValueTypeCreate(Y, "int");
    type->funcClear      = ekValueTypeFuncNotUsed;
    type->funcClone      = intFuncClone;
    type->funcToBool     = intFuncToBool;
    type->funcToInt      = intFuncToInt;
    type->funcToFloat    = intFuncToFloat;
    type->funcToString   = intFuncToString;
    type->funcArithmetic = intFuncArithmetic;
    type->funcCmp        = intFuncCmp;
    type->funcIndex      = ekValueTypeFuncNotUsed;
    type->funcDump       = intFuncDump;
    ekValueTypeRegister(Y, type);
    ekAssert(type->id == YVT_INT);
}

// ---------------------------------------------------------------------------
// YVT_FLOAT Funcs

static void floatFuncClone(struct ekContext *Y, struct ekValue *dst, struct ekValue *src)
{
    dst->floatVal = src->floatVal;
}

static yBool floatFuncToBool(struct ekContext *Y, struct ekValue *p)
{
    return (p->floatVal != 0.0f) ? yTrue : yFalse;
}

static yS32 floatFuncToInt(struct ekContext *Y, struct ekValue *p)
{
    return (yS32)p->floatVal;
}

static yF32 floatFuncToFloat(struct ekContext *Y, struct ekValue *p)
{
    return p->floatVal;
}

static struct ekValue *floatFuncToString(struct ekContext *Y, struct ekValue *p)
{
    char temp[64];
    sprintf(temp, "%f", p->floatVal);
    ekValueRemoveRefNote(Y, p, "floatFuncToString doesnt need float anymore");
    return ekValueCreateString(Y, temp);
}

static struct ekValue *floatFuncArithmetic(struct ekContext *Y, struct ekValue *a, struct ekValue *b, ekValueArithmeticOp op)
{
    ekValueAddRefNote(Y, b, "floatFuncArithmetic keep b during float conversion");
    b = ekValueToFloat(Y, b);
    switch(op)
    {
        case YVAO_ADD:
            a = ekValueCreateFloat(Y, a->floatVal + b->floatVal);
            break;
        case YVAO_SUB:
            a = ekValueCreateFloat(Y, a->floatVal - b->floatVal);
            break;
        case YVAO_MUL:
            a = ekValueCreateFloat(Y, a->floatVal * b->floatVal);
            break;
        case YVAO_DIV:
            if(b->floatVal == 0.0f)
            {
                ekContextSetError(Y, YVE_RUNTIME, "divide by zero!");
            }
            else
            {
                a = ekValueCreateFloat(Y, a->floatVal / b->floatVal);
            }
            break;
    };
    ekValueRemoveRefNote(Y, b, "floatFuncArithmetic temp b done");
    return a;
}

static yBool floatFuncCmp(struct ekContext *Y, struct ekValue *a, struct ekValue *b, int *cmpResult)
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

static void floatFuncDump(struct ekContext *Y, ekDumpParams *params, struct ekValue *p)
{
    char temp[64];
    sprintf(temp, "%f", p->floatVal);
    ekStringConcat(Y, &params->output, temp);
}

static void floatFuncRegister(struct ekContext *Y)
{
    ekValueType *type = ekValueTypeCreate(Y, "float");
    type->funcClear      = ekValueTypeFuncNotUsed;
    type->funcClone      = floatFuncClone;
    type->funcToBool     = floatFuncToBool;
    type->funcToInt      = floatFuncToInt;
    type->funcToFloat    = floatFuncToFloat;
    type->funcToString   = floatFuncToString;
    type->funcArithmetic = floatFuncArithmetic;
    type->funcCmp        = floatFuncCmp;
    type->funcIndex      = ekValueTypeFuncNotUsed;
    type->funcDump       = floatFuncDump;
    ekValueTypeRegister(Y, type);
    ekAssert(type->id == YVT_FLOAT);
}

// ---------------------------------------------------------------------------
// YVT_STRING Funcs

static void stringFuncClear(struct ekContext *Y, struct ekValue *p)
{
    ekStringClear(Y, &p->stringVal);
}

static void stringFuncClone(struct ekContext *Y, struct ekValue *dst, struct ekValue *src)
{
    ekStringSetStr(Y, &dst->stringVal, &src->stringVal);
}

static yBool stringFuncToBool(struct ekContext *Y, struct ekValue *p)
{
    return (p->stringVal.len) ? yTrue : yFalse;
}

static yS32 stringFuncToInt(struct ekContext *Y, struct ekValue *p)
{
    ekToken t = { ekStringSafePtr(&p->stringVal), p->stringVal.len };
    return ekTokenToInt(Y, &t);
}

static yF32 stringFuncToFloat(struct ekContext *Y, struct ekValue *p)
{
    ekToken t = { ekStringSafePtr(&p->stringVal), p->stringVal.len };
    return ekTokenToFloat(Y, &t);
}

struct ekValue *stringFuncToString(struct ekContext *Y, struct ekValue *p)
{
    // Implicit 'create' of new value (addref p), followed by 'destroy' of old value (removeref p)
    return p;
}

struct ekValue *stringFuncArithmetic(struct ekContext *Y, struct ekValue *a, struct ekValue *b, ekValueArithmeticOp op)
{
    ekValue *ret = NULL;
    if(op == YVAO_ADD)
    {
        ekValueAddRefNote(Y, a, "stringFuncArithmetic keep a during string conversion");
        ekValueAddRefNote(Y, b, "stringFuncArithmetic keep b during string conversion");
        a = ekValueToString(Y, a);
        b = ekValueToString(Y, b);
        ret = ekValueCreateString(Y, ekStringSafePtr(&a->stringVal));
        ekStringConcatStr(Y, &ret->stringVal, &a->stringVal);
        ekValueRemoveRefNote(Y, a, "stringFuncArithmetic temp a done");
        ekValueRemoveRefNote(Y, b, "stringFuncArithmetic temp b done");
    }
    else
    {
        ekTraceExecution(("stringFuncArithmetic(): cannot subtract, multiply, or divide strings!"));
    }
    return ret;
}

static yBool stringFuncCmp(struct ekContext *Y, struct ekValue *a, struct ekValue *b, int *cmpResult)
{
    if(b->type == YVT_STRING)
    {
        *cmpResult = ekStringCmpStr(Y, &a->stringVal, &b->stringVal);
        return yTrue;
    }
    return yFalse;
}

static void stringFuncDump(struct ekContext *Y, ekDumpParams *params, struct ekValue *p)
{
    ekStringConcatLen(Y, &params->output, "\"", 1);
    ekStringConcatStr(Y, &params->output, &p->stringVal);
    ekStringConcatLen(Y, &params->output, "\"", 1);
}

static void stringFuncRegister(struct ekContext *Y)
{
    ekValueType *type = ekValueTypeCreate(Y, "string");
    type->funcClear      = stringFuncClear;
    type->funcClone      = stringFuncClone;
    type->funcToBool     = stringFuncToBool;
    type->funcToInt      = stringFuncToInt;
    type->funcToFloat    = stringFuncToFloat;
    type->funcToString   = stringFuncToString;
    type->funcArithmetic = stringFuncArithmetic;
    type->funcCmp        = stringFuncCmp;
    type->funcIndex      = ekValueTypeFuncNotUsed;
    type->funcDump       = stringFuncDump;
    ekValueTypeRegister(Y, type);
    ekAssert(type->id == YVT_STRING);
}

// ---------------------------------------------------------------------------
// YVT_ARRAY Funcs

static void arrayFuncClear(struct ekContext *Y, struct ekValue *p)
{
    ekArrayDestroy(Y, &p->arrayVal, (ekDestroyCB)ekValueRemoveRefHashed);
}

static void arrayFuncClone(struct ekContext *Y, struct ekValue *dst, struct ekValue *src)
{
    ekAssert(0 && "arrayFuncClone not implemented");
}

static yBool arrayFuncToBool(struct ekContext *Y, struct ekValue *p)
{
    return (p->arrayVal && ekArraySize(Y, &p->arrayVal)) ? yTrue : yFalse;
}

static yS32 arrayFuncToInt(struct ekContext *Y, struct ekValue *p)
{
    return (p->arrayVal) ? ekArraySize(Y, &p->arrayVal) : 0;
}

static yF32 arrayFuncToFloat(struct ekContext *Y, struct ekValue *p)
{
    return (p->arrayVal) ? (yF32)ekArraySize(Y, &p->arrayVal) : 0;
}

static struct ekValue *arrayFuncIndex(struct ekContext *Y, struct ekValue *value, struct ekValue *index, yBool lvalue)
{
    ekValue *ret = NULL;
    ekValue **ref = NULL;
    ekValueAddRefNote(Y, index, "keep index around after int conversion");
    index = ekValueToInt(Y, index);
    if(index->intVal >= 0 && index->intVal < ekArraySize(Y, &value->arrayVal))
    {
        ref = (ekValue **) & (value->arrayVal[index->intVal]);
        if(lvalue)
        {
            ret = ekValueCreateRef(Y, ref);
        }
        else
        {
            ret = *ref;
            ekValueAddRefNote(Y, ret, "arrayFuncIndex");
        }
    }
    else
    {
        ekContextSetError(Y, YVE_RUNTIME, "array index %d out of range", index->intVal);
    }
    ekValueRemoveRefNote(Y, index, "temp index (int) done");
    return ret;
}

static void arrayFuncDump(struct ekContext *Y, ekDumpParams *params, struct ekValue *p)
{
    int i;
    ekStringConcat(Y, &params->output, "[ ");
    for(i=0; i<ekArraySize(Y, &p->arrayVal); i++)
    {
        ekValue *child = (ekValue *)p->arrayVal[i];
        if(i > 0)
        {
            ekStringConcat(Y, &params->output, ", ");
        }
        ekValueTypeSafeCall(child->type, Dump)(Y, params, child);
    }
    ekStringConcat(Y, &params->output, " ]");
}

static void arrayFuncRegister(struct ekContext *Y)
{
    ekValueType *type = ekValueTypeCreate(Y, "array");
    type->funcClear      = arrayFuncClear;
    type->funcClone      = arrayFuncClone;
    type->funcToBool     = arrayFuncToBool;
    type->funcToInt      = arrayFuncToInt;
    type->funcToFloat    = arrayFuncToFloat;
    type->funcToString   = ekValueTypeFuncNotUsed;
    type->funcArithmetic = ekValueTypeFuncNotUsed;
    type->funcCmp        = ekValueTypeFuncNotUsed;
    type->funcIndex      = arrayFuncIndex;
    type->funcDump       = arrayFuncDump;
    ekValueTypeRegister(Y, type);
    ekAssert(type->id == YVT_ARRAY);
}

// ---------------------------------------------------------------------------
// YVT_OBJECT Funcs

static void objectFuncClear(struct ekContext *Y, struct ekValue *p)
{
    ekObjectDestroy(Y, p->objectVal);
}

static void objectFuncClone(struct ekContext *Y, struct ekValue *dst, struct ekValue *src)
{
    ekAssert(0 && "objectFuncClone not implemented");
}

static yBool objectFuncToBool(struct ekContext *Y, struct ekValue *p)
{
    return yTrue;
}

static yS32 objectFuncToInt(struct ekContext *Y, struct ekValue *p)
{
    return 1; // ?
}

static yF32 objectFuncToFloat(struct ekContext *Y, struct ekValue *p)
{
    return 1.0f; // ?
}

static struct ekValue *objectFuncToString(struct ekContext *Y, struct ekValue *p)
{
    char temp[32];
    sprintf(temp, "[object:%p]", p->objectVal);
    return ekValueCreateString(Y, temp);
}

static struct ekValue *objectFuncIndex(struct ekContext *Y, struct ekValue *value, struct ekValue *index, yBool lvalue)
{
    ekValue *ret = NULL;
    ekValue **ref = NULL;
    index = ekValueToString(Y, index);
    ref = ekObjectGetRef(Y, value->objectVal, ekStringSafePtr(&index->stringVal), lvalue /* create? */);
    if(lvalue)
    {
        ret = ekValueCreateRef(Y, ref);
    }
    else
    {
        ret = *ref;
        ekValueAddRefNote(Y, ret, "objectFuncIndex");
    }
    return ret;
}

void appendKeys(struct ekContext *Y, ekDumpParams *params, ekMapEntry *entry)
{
    ekValue *child = (ekValue *)entry->valuePtr;
    if(params->tempInt)
    {
        ekStringConcat(Y, &params->output, "\"");
    }
    else
    {
        ekStringConcat(Y, &params->output, ", \"");
    }

    ekStringConcat(Y, &params->output, entry->keyStr);
    ekStringConcat(Y, &params->output, "\" : ");
    ekValueTypeSafeCall(child->type, Dump)(Y, params, child);
    params->tempInt = 0;
}

static void objectFuncDump(struct ekContext *Y, ekDumpParams *params, struct ekValue *p)
{
    params->tempInt = 1;
    ekStringConcat(Y, &params->output, "{ ");
    ekMapIterateP1(Y, p->objectVal->hash, appendKeys, params);
    ekStringConcat(Y, &params->output, " }");
}

static void objectFuncRegister(struct ekContext *Y)
{
    ekValueType *type = ekValueTypeCreate(Y, "object");
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
    ekValueTypeRegister(Y, type);
    ekAssert(type->id == YVT_OBJECT);
}

// ---------------------------------------------------------------------------
// YVT_REF Funcs

static void refFuncClone(struct ekContext *Y, struct ekValue *dst, struct ekValue *src)
{
    dst->refVal = src->refVal;
}

static yBool refFuncToBool(struct ekContext *Y, struct ekValue *p)
{
    return (*p->refVal) ? yTrue : yFalse;
}

static yS32 refFuncToInt(struct ekContext *Y, struct ekValue *p)
{
    return 1; // ?
}

static yF32 refFuncToFloat(struct ekContext *Y, struct ekValue *p)
{
    return 1.0f; // ?
}

static void refFuncDump(struct ekContext *Y, ekDumpParams *params, struct ekValue *p)
{
    ekStringConcat(Y, &params->output, "(ref: ");
    ekValueTypeSafeCall((*p->refVal)->type, Dump)(Y, params, *p->refVal);
    ekStringConcat(Y, &params->output, ")");
}

static void refFuncRegister(struct ekContext *Y)
{
    ekValueType *type = ekValueTypeCreate(Y, "ref");
    type->funcClear      = ekValueTypeFuncNotUsed;
    type->funcClone      = refFuncClone;
    type->funcToBool     = refFuncToBool;
    type->funcToInt      = refFuncToInt;
    type->funcToFloat    = refFuncToFloat;
    type->funcToString   = ekValueTypeFuncNotUsed;
    type->funcArithmetic = ekValueTypeFuncNotUsed;
    type->funcCmp        = ekValueTypeFuncNotUsed;
    type->funcIndex      = ekValueTypeFuncNotUsed;
    type->funcDump       = refFuncDump;
    ekValueTypeRegister(Y, type);
    ekAssert(type->id == YVT_REF);
}

// ---------------------------------------------------------------------------

void ekValueTypeRegisterAllBasicTypes(struct ekContext *Y)
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

ekValue ekValueNull = {YVT_NULL};
ekValue *ekValueNullPtr = &ekValueNull;

ekValue *ekValueCreateInt(struct ekContext *Y, int v)
{
    ekValue *p = ekValueCreate(Y);
    p->type = YVT_INT;
    p->intVal = v;
    ekTraceValues(("ekValueCreateInt %p [%d]\n", p, v));
    return p;
}

ekValue *ekValueCreateFloat(struct ekContext *Y, yF32 v)
{
    ekValue *p = ekValueCreate(Y);
    p->type = YVT_FLOAT;
    p->floatVal = v;
    ekTraceValues(("ekValueCreateFloat %p [%f]\n", p, v));
    return p;
}

ekValue *ekValueCreateKString(struct ekContext *Y, const char *s)
{
    ekValue *p = ekValueCreate(Y);
    p->type = YVT_STRING;
    ekStringSetK(Y, &p->stringVal, s);
    ekTraceValues(("ekValueCreateKString %p\n", p));
    return p;
}

ekValue *ekValueCreateString(struct ekContext *Y, const char *s)
{
    ekValue *p = ekValueCreate(Y);
    p->type = YVT_STRING;
    ekStringSet(Y, &p->stringVal, s);
    ekTraceValues(("ekValueCreateString %p\n", p));
    return p;
}

ekValue *ekValueDonateString(struct ekContext *Y, char *s)
{
    ekValue *p = ekValueCreate(Y);
    p->type = YVT_STRING;
    ekStringDonate(Y, &p->stringVal, s);
    ekTraceValues(("ekValueDonateString %p\n", p));
    return p;
}

static void ekValueAddClosureVar(ekContext *Y, ekMap *closureVars, ekMapEntry *entry)
{
    ekValueAddRefNote(Y, entry->valuePtr, "+ref closure variable");
    ekMapGetS2P(Y, closureVars, entry->keyStr) = entry->valuePtr;
}

void ekValueAddClosureVars(struct ekContext *Y, ekValue *p)
{
    ekFrame *frame;
    int frameIndex;

    if(p->type != YVT_BLOCK)
    {
        // TODO: add impossibly catastrophic error here? (can't happen?)
        return;
    }

    ekAssert(p->closureVars == NULL);

    for(frameIndex = ekArraySize(Y, &Y->frames) - 1; frameIndex >= 0; frameIndex--)
    {
        frame = Y->frames[frameIndex];
        if((frame->type & (YFT_CHUNK|YFT_FUNC)) == YFT_FUNC)  // we are inside of an actual function!
        {
            break;
        }
    }

    if(frameIndex >= 0)
    {
        for(; frameIndex < ekArraySize(Y, &Y->frames); frameIndex++)
        {
            frame = Y->frames[frameIndex];
            if(frame->locals->count)
            {
                if(!p->closureVars)
                {
                    p->closureVars = ekMapCreate(Y, YMKT_STRING);
                }
                ekMapIterateP1(Y, frame->locals, ekValueAddClosureVar, p->closureVars);
            }
        }
    }
}

ekValue *ekValueCreateFunction(struct ekContext *Y, struct ekBlock *block)
{
    ekValue *p = ekValueCreate(Y);
    p->type = YVT_BLOCK;
    p->closureVars = NULL;
    p->blockVal = block;
    ekTraceValues(("ekValueCreateFunction %p\n", p));
    return p;
}

ekValue *ekValueCreateCFunction(struct ekContext *Y, ekCFunction func)
{
    ekValue *p = ekValueCreate(Y);
    p->type = YVT_CFUNCTION;
    p->cFuncVal = func;
    ekTraceValues(("ekValueCreateCFunction %p\n", p));
    return p;
}

ekValue *ekValueCreateRef(struct ekContext *Y, struct ekValue **ref)
{
    ekValue *p = ekValueCreate(Y);
    p->type = YVT_REF;
    p->refVal = ref;
    ekTraceValues(("ekValueCreateRef %p\n", p));
    return p;
}

static yBool ekValueCheckRef(struct ekContext *Y, ekValue *ref, ekValue *p)
{
    if(!p)
    {
        ekContextSetError(Y, YVE_RUNTIME, "ekValueSetRefVal: empty stack!");
        return yFalse;
    }
    if(!ref)
    {
        ekContextSetError(Y, YVE_RUNTIME, "ekValueSetRefVal: empty stack!");
        return yFalse;
    }
    if(!(*ref->refVal))
    {
        ekContextSetError(Y, YVE_RUNTIME, "ekValueSetRefVal: missing ref!");
        return yFalse;
    }
    if(ref->type != YVT_REF)
    {
        ekContextSetError(Y, YVE_RUNTIME, "ekValueSetRefVal: value on top of stack, ref underneath");
        return yFalse;
    }
    return yTrue;
}

yBool ekValueSetRefVal(struct ekContext *Y, ekValue *ref, ekValue *p)
{
    if(!ekValueCheckRef(Y, ref, p))
    {
        return yFalse;
    }

    ekValueRemoveRefNote(Y, *(ref->refVal), "SetRefVal: forgetting previous val");
    *(ref->refVal) = p;
    ekValueAddRefNote(Y, p, "SetRefVal: taking ownership of val");

    ekTraceValues(("ekValueSetRefVal %p = %p\n", ref, p));
    return yTrue;
}

yBool ekValueTestInherits(struct ekContext *Y, ekValue *child, ekValue *parent)
{
    ekValue *p;

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

ekValue *ekValueCreateArray(struct ekContext *Y)
{
    ekValue *p = ekValueCreate(Y);
    p->arrayVal = NULL;
    p->type = YVT_ARRAY;
    return p;
}

void ekValueArrayPush(struct ekContext *Y, ekValue *p, ekValue *v)
{
    ekAssert(p->type == YVT_ARRAY);

    ekArrayPush(Y, &p->arrayVal, v);
}

// ---------------------------------------------------------------------------

ekValue *ekValueCreateObject(struct ekContext *Y, struct ekValue *isa, int argCount, yBool firstArgIsa)
{
    ekValue *p = ekValueCreate(Y);
    if(firstArgIsa)
    {
        ekAssert(argCount);
        ekAssert(isa == NULL);

        if(argCount)
        {
            isa = ekContextGetArg(Y, 0, argCount);
            if(!isa || (isa->type == YVT_NULL))
            {
                isa = NULL;
            }
            if(isa && (isa->type != YVT_OBJECT))
            {
                ekContextSetError(Y, YVE_RUNTIME, "objects can only inherit from objects");
                isa = NULL;
            }
        }
        else
        {
            isa = NULL;
        }
    }
    p->objectVal = ekObjectCreate(Y, isa);
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
            ekValue **ref;
            ekValue *key = ekContextGetArg(Y, i, argCount);
            ekValue *val = ekValueNullPtr;
            int valueArg = i+1;
            if(valueArg < argCount)
            {
                val = ekContextGetArg(Y, valueArg, argCount);
            }
            key = ekValueToString(Y, key);
            ref = ekObjectGetRef(Y, p->objectVal, ekStringSafePtr(&key->stringVal), yTrue);
            *ref = val;
            ekValueAddRefNote(Y, val, "ekValueCreateObject add member value");
        }
        ekContextPopValues(Y, argCount);
    }
    return p;
}

void ekValueObjectSetMember(struct ekContext *Y, struct ekValue *object, const char *name, struct ekValue *value)
{
    ekValue **ref = NULL;
    ekAssert(object->type == YVT_OBJECT);
    ref = ekObjectGetRef(Y, object->objectVal, name, yTrue);
    ekAssert(ref);
    *ref = value;
}

// ---------------------------------------------------------------------------

void ekValueClear(struct ekContext *Y, ekValue *p)
{
    ekValueTypeSafeCall(p->type, Clear)(Y, p);

    memset(p, 0, sizeof(*p));
    p->type = YVT_NULL;
}

#ifdef EUREKA_TRACE_REFS
static int sEurekaValueDebugCount = 0;

int ekValueDebugCount()
{
    return sEurekaValueDebugCount;
}
#endif

ekValue *ekValueCreate(ekContext *Y)
{
    ekValue *value = ekAlloc(sizeof(ekValue));
    ekValueTraceRefs(Y, value, 1, "ekValueCreate");
    value->refs = 1;
    ekTraceValues(("ekValueCreate %p\n", value));
#ifdef EUREKA_TRACE_REFS
    ++sEurekaValueDebugCount;
#endif
    return value;
}

void ekValueDestroy(struct ekContext *Y, ekValue *p)
{
    ekTraceValues(("ekValueFree %p\n", p));
    ekValueClear(Y, p);
    ekFree(p);
#ifdef EUREKA_TRACE_REFS
    --sEurekaValueDebugCount;
#endif
}

void ekValueAddRef(struct ekContext *Y, ekValue *p)
{
    if(p == ekValueNullPtr)
    {
        return;
    }

    ekAssert(p->refs);
    ++p->refs;
}

void ekValueRemoveRef(struct ekContext *Y, ekValue *p)
{
    if(p == ekValueNullPtr)
    {
        return;
    }

    --p->refs;
    if(p->refs == 0)
    {
        ekValueDestroy(Y, p);
    }
}

yS32 ekValueCmp(struct ekContext *Y, ekValue *a, ekValue *b)
{
    if(a == b)
    {
        return 0;    // this should also handle the NULL case
    }

    if(a && b)
    {
        yS32 ret = 0;
        if(ekValueTypeSafeCall(a->type, Cmp)(Y, a, b, &ret))
        {
            return ret;
        }
    }

    return (yS32)(a - b); // Fallback case: compare pointers for consistency
}

void ekValueCloneData(struct ekContext *Y, ekValue *dst, ekValue *src)
{
    dst->type = src->type;
    ekValueTypeSafeCall(dst->type, Clone)(Y, dst, src);
}

ekValue *ekValueClone(struct ekContext *Y, ekValue *p)
{
    ekValue *n = ekValueCreate(Y);
    ekValueCloneData(Y, n, p);
    ekTraceValues(("ekValueClone %p -> %p\n", p, n));
    return n;
}

ekValue *ekValueAdd(struct ekContext *Y, ekValue *a, ekValue *b)
{
    ekValue *value = ekValueTypeSafeCall(a->type, Arithmetic)(Y, a, b, YVAO_ADD);
    if(!value)
    {
        ekTraceValues(("Don't know how to add types %s and %s\n", ekValueTypePtr(a->type)->name, ekValueTypePtr(b->type)->name));
    }
    return value;
}

ekValue *ekValueSub(struct ekContext *Y, ekValue *a, ekValue *b)
{
    ekValue *value = ekValueTypeSafeCall(a->type, Arithmetic)(Y, a, b, YVAO_SUB);
    if(!value)
    {
        ekTraceValues(("Don't know how to subtract types %s and %s\n", ekValueTypePtr(a->type)->name, ekValueTypePtr(b->type)->name));
    }
    return value;
}

ekValue *ekValueMul(struct ekContext *Y, ekValue *a, ekValue *b)
{
    ekValue *value = ekValueTypeSafeCall(a->type, Arithmetic)(Y, a, b, YVAO_MUL);
    if(!value)
    {
        ekTraceValues(("Don't know how to multiply types %s and %s\n", ekValueTypePtr(a->type)->name, ekValueTypePtr(b->type)->name));
    }
    return value;
}

ekValue *ekValueDiv(struct ekContext *Y, ekValue *a, ekValue *b)
{
    ekValue *value = ekValueTypeSafeCall(a->type, Arithmetic)(Y, a, b, YVAO_DIV);
    if(!value)
    {
        ekTraceValues(("Don't know how to divide types %s and %s\n", ekValueTypePtr(a->type)->name, ekValueTypePtr(b->type)->name));
    }
    return value;
}

ekValue *ekValueToBool(struct ekContext *Y, ekValue *p)
{
    yBool boolVal = ekValueTypeSafeCall(p->type, ToBool)(Y, p);
    ekValue *value = ekValueCreateInt(Y, boolVal);
    ekValueRemoveRefNote(Y, p, "ekValueToBool");
    return value;
}

ekValue *ekValueToInt(struct ekContext *Y, ekValue *p)
{
    yS32 intVal = ekValueTypeSafeCall(p->type, ToInt)(Y, p);
    ekValue *ret = ekValueCreateInt(Y, intVal);
    ekValueRemoveRefNote(Y, p, "ekValueToInt");
    return ret;
}

ekValue *ekValueToFloat(struct ekContext *Y, ekValue *p)
{
    yF32 floatVal = ekValueTypeSafeCall(p->type, ToFloat)(Y, p);
    ekValue *ret = ekValueCreateFloat(Y, floatVal);
    ekValueRemoveRefNote(Y, p, "ekValueToFloat");
    return ret;
}

ekValue *ekValueToString(struct ekContext *Y, ekValue *p)
{
    ekValue *value = ekValueTypeSafeCall(p->type, ToString)(Y, p);
    if(!value)
    {
        ekTraceExecution(("ekValueToString: unable to convert type '%s' to string\n", ekValueTypePtr(p->type)->name));
    }
    return value;
}

ekValue *ekValueStringFormat(struct ekContext *Y, ekValue *format, yS32 argCount)
{
    char *curr = (char *)ekStringSafePtr(&format->stringVal);
    char *next;

    ekValue *arg;
    int argIndex = 0;

    ekValue *ret = ekValueCreateString(Y, "");
    ekString *str = &ret->stringVal;

    while(curr && (next = strchr(curr, '%')))
    {
        // First, add in all of the stuff before the %
        ekStringConcatLen(Y, str, curr, (int)(next - curr));
        next++;

        switch(*next)
        {
            case '\0':
                curr = NULL;
                break;
            case '%':
                ekStringConcatLen(Y, str, "%", 1);
                break;
            case 's':
                arg = ekContextGetArg(Y, argIndex++, argCount);
                if(arg)
                {
                    ekValueAddRefNote(Y, arg, "string conversion");
                    arg = ekValueToString(Y, arg);
                    ekStringConcatStr(Y, str, &arg->stringVal);
                    ekValueRemoveRefNote(Y, arg, "StringFormat 's' done");
                }
                break;
            case 'd':
                arg = ekContextGetArg(Y, argIndex++, argCount);
                if(arg)
                {
                    char temp[32];
                    ekValueAddRefNote(Y, arg, "int conversion");
                    arg = ekValueToInt(Y, arg);
                    sprintf(temp, "%d", arg->intVal);
                    ekStringConcat(Y, str, temp);
                    ekValueRemoveRefNote(Y, arg, "StringFormat 'd' done");
                }
                break;
            case 'f':
                arg = ekContextGetArg(Y, argIndex++, argCount);
                if(arg)
                {
                    char temp[32];
                    ekValueAddRefNote(Y, arg, "float conversion");
                    arg = ekValueToFloat(Y, arg);
                    sprintf(temp, "%f", arg->floatVal);
                    ekStringConcat(Y, str, temp);
                    ekValueRemoveRefNote(Y, arg, "StringFormat 'f' done");
                }
                break;
        };

        curr = next + 1;
    }

    // Add the remainder of the string, if any
    if(curr)
    {
        ekStringConcat(Y, str, curr);
    }

    ekContextPopValues(Y, argCount);
    ekValueRemoveRefNote(Y, format, "FORMAT format done");
    return ret;
}

ekValue *ekValueIndex(struct ekContext *Y, ekValue *p, ekValue *index, yBool lvalue)
{
    return ekValueTypeSafeCall(p->type, Index)(Y, p, index, lvalue);
}

const char *ekValueTypeName(struct ekContext *Y, int type)
{
    if((type >= 0) && (type < ekArraySize(Y, &Y->types)))
    {
        ekValueType *valueType = Y->types[type];
        return valueType->name;
    }
    return "unknown";
}

void ekValueDump(struct ekContext *Y, ekDumpParams *params, ekValue *p)
{
    ekValueTypeSafeCall(p->type, Dump)(Y, params, p);
}

#ifdef EUREKA_TRACE_REFS
void ekValueTraceRefs(struct ekContext *Y, struct ekValue *p, int delta, const char *note)
{
    const char *destroyed = "";
    char tempPtr[32];
    int newRefs = p->refs;
    if(p == ekValueNullPtr)
    {
        sprintf(tempPtr, "0xEUREKA_NULL");
        note = "-- ignoring ekValueNull --";
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
    ekTraceRefs(("\t\t\t\t\t\t\t\tREFS: ekValue %s [%2d delta -> %d]: %s%s\n", tempPtr, delta, newRefs, note, destroyed));
}

void ekValueRemoveRefArray(struct ekContext *Y, struct ekValue *p)
{
    ekValueTraceRefs(Y, p, -1, "array cleared");
    ekValueRemoveRef(Y, p);
}

void ekValueRemoveRefHashed(struct ekContext *Y, struct ekValue *p)
{
    ekValueTraceRefs(Y, p, -1, "hash cleared");
    ekValueRemoveRef(Y, p);
}
#endif
