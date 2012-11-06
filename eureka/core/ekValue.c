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

ekDumpParams *ekDumpParamsCreate(struct ekContext *E)
{
    ekDumpParams *params = (ekDumpParams *)ekAlloc(sizeof(ekDumpParams));
    return params;
}

void ekDumpParamsDestroy(struct ekContext *E, ekDumpParams *params)
{
    ekStringClear(E, &params->output);
    ekStringClear(E, &params->tempStr);
    ekFree(params);
}

// ---------------------------------------------------------------------------

ekValueType *ekValueTypeCreate(struct ekContext *E, const char *name)
{
    ekValueType *type = ekAlloc(sizeof(ekValueType));
    strcpy(type->name, name);
    return type;
}

void ekValueTypeDestroy(struct ekContext *E, ekValueType *type)
{
    if(type->funcDestroyUserData)
    {
        type->funcDestroyUserData(E, type);
    }
    ekFree(type);
}

int ekValueTypeRegister(struct ekContext *E, ekValueType *newType)
{
    int i;
    for(i=0; i<ekArraySize(E, &E->types); i++)
    {
        ekValueType *t = E->types[i];
        if(!strcmp(newType->name, t->name))
        {
            return EVT_INVALID;
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

    newType->id = ekArrayPush(E, &E->types, newType);
    return newType->id;
}

// ---------------------------------------------------------------------------
// EVT_NULL Funcs

static struct ekValue *nullFuncToString(struct ekContext *E, struct ekValue *p)
{
    return ekValueCreateKString(E, NULL_STRING_FORM);
}

static void nullFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    ekStringConcat(E, &params->output, NULL_STRING_FORM);
}

static void nullFuncRegister(struct ekContext *E)
{
    ekValueType *type   = ekValueTypeCreate(E, "null");
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
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_NULL);
}

// ---------------------------------------------------------------------------
// EVT_BLOCK Funcs

static void blockFuncClear(struct ekContext *E, struct ekValue *p)
{
    if(p->closureVars)
    {
        ekMapDestroy(E, p->closureVars, ekValueRemoveRef);
    }
}

static void blockFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    dst->blockVal = src->blockVal;
}

static ekBool blockFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return ekTrue;
}

static ekS32 blockFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return 1; // ?
}

static ekF32 blockFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return 1.0f; // ?
}

static void blockFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    char temp[64];
    sprintf(temp, "(block:0x%p)", p->blockVal);
    ekStringConcat(E, &params->output, temp);
}

static void blockFuncRegister(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "block");
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
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_BLOCK);
}

// ---------------------------------------------------------------------------
// EVT_CFUNCTION Funcs

static void cfunctionFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    dst->cFuncVal = src->cFuncVal;
}

static ekBool cfunctionFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return ekTrue;
}

static ekS32 cfunctionFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return 1; // ?
}

static ekF32 cfunctionFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return 1.0f; // ?
}

static void cfunctionFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    char temp[64];
    sprintf(temp, "(cfunction:0x%p)", p->cFuncVal);
    ekStringConcat(E, &params->output, temp);
}

static void cfunctionFuncRegister(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "cfunction");
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
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_CFUNCTION);
}

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

static void intFuncRegister(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "int");
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
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_INT);
}

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

static ekBool floatFuncCmp(struct ekContext *E, struct ekValue *a, struct ekValue *b, int *cmpResult)
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

static void floatFuncRegister(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "float");
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
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_FLOAT);
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

static void stringFuncRegister(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "string");
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
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_STRING);
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

static struct ekValue *arrayFuncIndex(struct ekContext *E, struct ekValue *value, struct ekValue *index, ekBool lvalue)
{
    ekValue *ret = NULL;
    ekValue **ref = NULL;
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

static void arrayFuncRegister(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "array");
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
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_ARRAY);
}

// ---------------------------------------------------------------------------
// EVT_OBJECT Funcs

static void objectFuncClear(struct ekContext *E, struct ekValue *p)
{
    ekObjectDestroy(E, p->objectVal);
}

static void objectFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    ekAssert(0 && "objectFuncClone not implemented");
}

static ekBool objectFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return ekTrue;
}

static ekS32 objectFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return 1; // ?
}

static ekF32 objectFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return 1.0f; // ?
}

static struct ekValue *objectFuncToString(struct ekContext *E, struct ekValue *p)
{
    char temp[32];
    sprintf(temp, "[object:%p]", p->objectVal);
    return ekValueCreateString(E, temp);
}

static struct ekValue *objectFuncIndex(struct ekContext *E, struct ekValue *value, struct ekValue *index, ekBool lvalue)
{
    ekValue *ret = NULL;
    ekValue **ref = NULL;
    index = ekValueToString(E, index);
    ref = ekObjectGetRef(E, value->objectVal, ekStringSafePtr(&index->stringVal), lvalue /* create? */);
    if(lvalue)
    {
        ret = ekValueCreateRef(E, ref);
    }
    else
    {
        ret = *ref;
        ekValueAddRefNote(E, ret, "objectFuncIndex");
    }
    return ret;
}

void appendKeys(struct ekContext *E, ekDumpParams *params, ekMapEntry *entry)
{
    ekValue *child = (ekValue *)entry->valuePtr;
    if(params->tempInt)
    {
        ekStringConcat(E, &params->output, "\"");
    }
    else
    {
        ekStringConcat(E, &params->output, ", \"");
    }

    ekStringConcat(E, &params->output, entry->keyStr);
    ekStringConcat(E, &params->output, "\" : ");
    ekValueTypeSafeCall(child->type, Dump)(E, params, child);
    params->tempInt = 0;
}

static void objectFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    params->tempInt = 1;
    ekStringConcat(E, &params->output, "{ ");
    ekMapIterateP1(E, p->objectVal->hash, appendKeys, params);
    ekStringConcat(E, &params->output, " }");
}

static void objectFuncRegister(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "object");
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
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_OBJECT);
}

// ---------------------------------------------------------------------------
// EVT_REF Funcs

static void refFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    dst->refVal = src->refVal;
}

static ekBool refFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return (*p->refVal) ? ekTrue : ekFalse;
}

static ekS32 refFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return 1; // ?
}

static ekF32 refFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return 1.0f; // ?
}

static void refFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    ekStringConcat(E, &params->output, "(ref: ");
    ekValueTypeSafeCall((*p->refVal)->type, Dump)(E, params, *p->refVal);
    ekStringConcat(E, &params->output, ")");
}

static void refFuncRegister(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "ref");
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
    ekValueTypeRegister(E, type);
    ekAssert(type->id == EVT_REF);
}

// ---------------------------------------------------------------------------

void ekValueTypeRegisterAllBasicTypes(struct ekContext *E)
{
    nullFuncRegister(E);
    blockFuncRegister(E);
    cfunctionFuncRegister(E);
    intFuncRegister(E);
    floatFuncRegister(E);
    stringFuncRegister(E);
    arrayFuncRegister(E);
    objectFuncRegister(E);
    refFuncRegister(E);
}

// ---------------------------------------------------------------------------

ekValue ekValueNull = {EVT_NULL};
ekValue *ekValueNullPtr = &ekValueNull;

ekValue *ekValueCreateInt(struct ekContext *E, int v)
{
    ekValue *p = ekValueCreate(E);
    p->type = EVT_INT;
    p->intVal = v;
    ekTraceValues(("ekValueCreateInt %p [%d]\n", p, v));
    return p;
}

ekValue *ekValueCreateFloat(struct ekContext *E, ekF32 v)
{
    ekValue *p = ekValueCreate(E);
    p->type = EVT_FLOAT;
    p->floatVal = v;
    ekTraceValues(("ekValueCreateFloat %p [%f]\n", p, v));
    return p;
}

ekValue *ekValueCreateKString(struct ekContext *E, const char *s)
{
    ekValue *p = ekValueCreate(E);
    p->type = EVT_STRING;
    ekStringSetK(E, &p->stringVal, s);
    ekTraceValues(("ekValueCreateKString %p\n", p));
    return p;
}

ekValue *ekValueCreateString(struct ekContext *E, const char *s)
{
    ekValue *p = ekValueCreate(E);
    p->type = EVT_STRING;
    ekStringSet(E, &p->stringVal, s);
    ekTraceValues(("ekValueCreateString %p\n", p));
    return p;
}

ekValue *ekValueDonateString(struct ekContext *E, char *s)
{
    ekValue *p = ekValueCreate(E);
    p->type = EVT_STRING;
    ekStringDonate(E, &p->stringVal, s);
    ekTraceValues(("ekValueDonateString %p\n", p));
    return p;
}

static void ekValueAddClosureVar(ekContext *E, ekMap *closureVars, ekMapEntry *entry)
{
    ekValueAddRefNote(E, entry->valuePtr, "+ref closure variable");
    ekMapGetS2P(E, closureVars, entry->keyStr) = entry->valuePtr;
}

void ekValueAddClosureVars(struct ekContext *E, ekValue *p)
{
    ekFrame *frame;
    int frameIndex;

    if(p->type != EVT_BLOCK)
    {
        // TODO: add impossibly catastrophic error here? (can't happen?)
        return;
    }

    ekAssert(p->closureVars == NULL);

    for(frameIndex = ekArraySize(E, &E->frames) - 1; frameIndex >= 0; frameIndex--)
    {
        frame = E->frames[frameIndex];
        if((frame->type & (EFT_CHUNK|EFT_FUNC)) == EFT_FUNC)  // we are inside of an actual function!
        {
            break;
        }
    }

    if(frameIndex >= 0)
    {
        for(; frameIndex < ekArraySize(E, &E->frames); frameIndex++)
        {
            frame = E->frames[frameIndex];
            if(frame->locals->count)
            {
                if(!p->closureVars)
                {
                    p->closureVars = ekMapCreate(E, EMKT_STRING);
                }
                ekMapIterateP1(E, frame->locals, ekValueAddClosureVar, p->closureVars);
            }
        }
    }
}

ekValue *ekValueCreateFunction(struct ekContext *E, struct ekBlock *block)
{
    ekValue *p = ekValueCreate(E);
    p->type = EVT_BLOCK;
    p->closureVars = NULL;
    p->blockVal = block;
    ekTraceValues(("ekValueCreateFunction %p\n", p));
    return p;
}

ekValue *ekValueCreateCFunction(struct ekContext *E, ekCFunction func)
{
    ekValue *p = ekValueCreate(E);
    p->type = EVT_CFUNCTION;
    p->cFuncVal = func;
    ekTraceValues(("ekValueCreateCFunction %p\n", p));
    return p;
}

ekValue *ekValueCreateRef(struct ekContext *E, struct ekValue **ref)
{
    ekValue *p = ekValueCreate(E);
    p->type = EVT_REF;
    p->refVal = ref;
    ekTraceValues(("ekValueCreateRef %p\n", p));
    return p;
}

static ekBool ekValueCheckRef(struct ekContext *E, ekValue *ref, ekValue *p)
{
    if(!p)
    {
        ekContextSetError(E, EVE_RUNTIME, "ekValueSetRefVal: empty stack!");
        return ekFalse;
    }
    if(!ref)
    {
        ekContextSetError(E, EVE_RUNTIME, "ekValueSetRefVal: empty stack!");
        return ekFalse;
    }
    if(!(*ref->refVal))
    {
        ekContextSetError(E, EVE_RUNTIME, "ekValueSetRefVal: missing ref!");
        return ekFalse;
    }
    if(ref->type != EVT_REF)
    {
        ekContextSetError(E, EVE_RUNTIME, "ekValueSetRefVal: value on top of stack, ref underneath");
        return ekFalse;
    }
    return ekTrue;
}

ekBool ekValueSetRefVal(struct ekContext *E, ekValue *ref, ekValue *p)
{
    if(!ekValueCheckRef(E, ref, p))
    {
        return ekFalse;
    }

    ekValueRemoveRefNote(E, *(ref->refVal), "SetRefVal: forgetting previous val");
    *(ref->refVal) = p;
    ekValueAddRefNote(E, p, "SetRefVal: taking ownership of val");

    ekTraceValues(("ekValueSetRefVal %p = %p\n", ref, p));
    return ekTrue;
}

ekBool ekValueTestInherits(struct ekContext *E, ekValue *child, ekValue *parent)
{
    ekValue *p;

    if(child->type != EVT_OBJECT)
    {
        return ekFalse;
    }

    if(parent->type != EVT_OBJECT)
    {
        return ekFalse;
    }

    p = child->objectVal->isa;
    while(p && (p->type == EVT_OBJECT))
    {
        if(p->objectVal == parent->objectVal)
        {
            return ekTrue;
        }

        p = p->objectVal->isa;
    }

    return ekFalse;
}

// ---------------------------------------------------------------------------

ekValue *ekValueCreateArray(struct ekContext *E)
{
    ekValue *p = ekValueCreate(E);
    p->arrayVal = NULL;
    p->type = EVT_ARRAY;
    return p;
}

void ekValueArrayPush(struct ekContext *E, ekValue *p, ekValue *v)
{
    ekAssert(p->type == EVT_ARRAY);

    ekArrayPush(E, &p->arrayVal, v);
}

// ---------------------------------------------------------------------------

ekValue *ekValueCreateObject(struct ekContext *E, struct ekValue *isa, int argCount, ekBool firstArgIsa)
{
    ekValue *p = ekValueCreate(E);
    if(firstArgIsa)
    {
        ekAssert(argCount);
        ekAssert(isa == NULL);

        if(argCount)
        {
            isa = ekContextGetArg(E, 0, argCount);
            if(!isa || (isa->type == EVT_NULL))
            {
                isa = NULL;
            }
            if(isa && (isa->type != EVT_OBJECT))
            {
                ekContextSetError(E, EVE_RUNTIME, "objects can only inherit from objects");
                isa = NULL;
            }
        }
        else
        {
            isa = NULL;
        }
    }
    p->objectVal = ekObjectCreate(E, isa);
    p->type = EVT_OBJECT;

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
            ekValue *key = ekContextGetArg(E, i, argCount);
            ekValue *val = ekValueNullPtr;
            int valueArg = i+1;
            if(valueArg < argCount)
            {
                val = ekContextGetArg(E, valueArg, argCount);
            }
            key = ekValueToString(E, key);
            ref = ekObjectGetRef(E, p->objectVal, ekStringSafePtr(&key->stringVal), ekTrue);
            *ref = val;
            ekValueAddRefNote(E, val, "ekValueCreateObject add member value");
        }
        ekContextPopValues(E, argCount);
    }
    return p;
}

void ekValueObjectSetMember(struct ekContext *E, struct ekValue *object, const char *name, struct ekValue *value)
{
    ekValue **ref = NULL;
    ekAssert(object->type == EVT_OBJECT);
    ref = ekObjectGetRef(E, object->objectVal, name, ekTrue);
    ekAssert(ref);
    *ref = value;
}

// ---------------------------------------------------------------------------

void ekValueClear(struct ekContext *E, ekValue *p)
{
    ekValueTypeSafeCall(p->type, Clear)(E, p);

    memset(p, 0, sizeof(*p));
    p->type = EVT_NULL;
}

#ifdef EUREKA_TRACE_REFS
static int sEurekaValueDebugCount = 0;

int ekValueDebugCount()
{
    return sEurekaValueDebugCount;
}
#endif

ekValue *ekValueCreate(ekContext *E)
{
    ekValue *value = ekAlloc(sizeof(ekValue));
    ekValueTraceRefs(E, value, 1, "ekValueCreate");
    value->refs = 1;
    ekTraceValues(("ekValueCreate %p\n", value));
#ifdef EUREKA_TRACE_REFS
    ++sEurekaValueDebugCount;
#endif
    return value;
}

void ekValueDestroy(struct ekContext *E, ekValue *p)
{
    ekTraceValues(("ekValueFree %p\n", p));
    ekValueClear(E, p);
    memset(p, 0xaaaaaaaa, sizeof(ekValue));
    ekFree(p);
#ifdef EUREKA_TRACE_REFS
    --sEurekaValueDebugCount;
#endif
}

void ekValueAddRef(struct ekContext *E, ekValue *p)
{
    if(p == ekValueNullPtr)
    {
        return;
    }

    ekAssert(p->refs);
    ++p->refs;
}

void ekValueRemoveRef(struct ekContext *E, ekValue *p)
{
    if(p == ekValueNullPtr)
    {
        return;
    }

    ekAssert(p->refs && (p->refs != 0xaaaaaaaa) && "refcount has gone rogue!");

    --p->refs;
    if(p->refs == 0)
    {
        ekValueDestroy(E, p);
    }
}

ekS32 ekValueCmp(struct ekContext *E, ekValue *a, ekValue *b)
{
    if(a == b)
    {
        return 0;    // this should also handle the NULL case
    }

    if(a && b)
    {
        ekS32 ret = 0;
        if(ekValueTypeSafeCall(a->type, Cmp)(E, a, b, &ret))
        {
            return ret;
        }
    }

    return (ekS32)(a - b); // Fallback case: compare pointers for consistency
}

void ekValueCloneData(struct ekContext *E, ekValue *dst, ekValue *src)
{
    dst->type = src->type;
    ekValueTypeSafeCall(dst->type, Clone)(E, dst, src);
}

ekValue *ekValueClone(struct ekContext *E, ekValue *p)
{
    ekValue *n = ekValueCreate(E);
    ekValueCloneData(E, n, p);
    ekTraceValues(("ekValueClone %p -> %p\n", p, n));
    return n;
}

ekValue *ekValueAdd(struct ekContext *E, ekValue *a, ekValue *b)
{
    ekValue *value = ekValueTypeSafeCall(a->type, Arithmetic)(E, a, b, EVAO_ADD);
    if(!value)
    {
        ekTraceValues(("Don't know how to add types %s and %s\n", ekValueTypePtr(a->type)->name, ekValueTypePtr(b->type)->name));
    }
    return value;
}

ekValue *ekValueSub(struct ekContext *E, ekValue *a, ekValue *b)
{
    ekValue *value = ekValueTypeSafeCall(a->type, Arithmetic)(E, a, b, EVAO_SUB);
    if(!value)
    {
        ekTraceValues(("Don't know how to subtract types %s and %s\n", ekValueTypePtr(a->type)->name, ekValueTypePtr(b->type)->name));
    }
    return value;
}

ekValue *ekValueMul(struct ekContext *E, ekValue *a, ekValue *b)
{
    ekValue *value = ekValueTypeSafeCall(a->type, Arithmetic)(E, a, b, EVAO_MUL);
    if(!value)
    {
        ekTraceValues(("Don't know how to multiply types %s and %s\n", ekValueTypePtr(a->type)->name, ekValueTypePtr(b->type)->name));
    }
    return value;
}

ekValue *ekValueDiv(struct ekContext *E, ekValue *a, ekValue *b)
{
    ekValue *value = ekValueTypeSafeCall(a->type, Arithmetic)(E, a, b, EVAO_DIV);
    if(!value)
    {
        ekTraceValues(("Don't know how to divide types %s and %s\n", ekValueTypePtr(a->type)->name, ekValueTypePtr(b->type)->name));
    }
    return value;
}

ekValue *ekValueToBool(struct ekContext *E, ekValue *p)
{
    ekBool boolVal = ekValueTypeSafeCall(p->type, ToBool)(E, p);
    ekValue *value = ekValueCreateInt(E, boolVal);
    ekValueRemoveRefNote(E, p, "ekValueToBool");
    return value;
}

ekValue *ekValueToInt(struct ekContext *E, ekValue *p)
{
    ekS32 intVal = ekValueTypeSafeCall(p->type, ToInt)(E, p);
    ekValue *ret = ekValueCreateInt(E, intVal);
    ekValueRemoveRefNote(E, p, "ekValueToInt");
    return ret;
}

ekValue *ekValueToFloat(struct ekContext *E, ekValue *p)
{
    ekF32 floatVal = ekValueTypeSafeCall(p->type, ToFloat)(E, p);
    ekValue *ret = ekValueCreateFloat(E, floatVal);
    ekValueRemoveRefNote(E, p, "ekValueToFloat");
    return ret;
}

ekValue *ekValueToString(struct ekContext *E, ekValue *p)
{
    ekValue *value = ekValueTypeSafeCall(p->type, ToString)(E, p);
    if(!value)
    {
        ekTraceExecution(("ekValueToString: unable to convert type '%s' to string\n", ekValueTypePtr(p->type)->name));
    }
    return value;
}

ekValue *ekValueStringFormat(struct ekContext *E, ekValue *format, ekS32 argCount)
{
    char *curr = (char *)ekStringSafePtr(&format->stringVal);
    char *next;

    ekValue *arg;
    int argIndex = 0;

    ekValue *ret = ekValueCreateString(E, "");
    ekString *str = &ret->stringVal;

    while(curr && (next = strchr(curr, '%')))
    {
        // First, add in all of the stuff before the %
        ekStringConcatLen(E, str, curr, (int)(next - curr));
        next++;

        switch(*next)
        {
            case '\0':
                curr = NULL;
                break;
            case '%':
                ekStringConcatLen(E, str, "%", 1);
                break;
            case 's':
                arg = ekContextGetArg(E, argIndex++, argCount);
                if(arg)
                {
                    ekValueAddRefNote(E, arg, "string conversion");
                    arg = ekValueToString(E, arg);
                    ekStringConcatStr(E, str, &arg->stringVal);
                    ekValueRemoveRefNote(E, arg, "StringFormat 's' done");
                }
                break;
            case 'd':
                arg = ekContextGetArg(E, argIndex++, argCount);
                if(arg)
                {
                    char temp[32];
                    ekValueAddRefNote(E, arg, "int conversion");
                    arg = ekValueToInt(E, arg);
                    sprintf(temp, "%d", arg->intVal);
                    ekStringConcat(E, str, temp);
                    ekValueRemoveRefNote(E, arg, "StringFormat 'd' done");
                }
                break;
            case 'f':
                arg = ekContextGetArg(E, argIndex++, argCount);
                if(arg)
                {
                    char temp[32];
                    ekValueAddRefNote(E, arg, "float conversion");
                    arg = ekValueToFloat(E, arg);
                    sprintf(temp, "%f", arg->floatVal);
                    ekStringConcat(E, str, temp);
                    ekValueRemoveRefNote(E, arg, "StringFormat 'f' done");
                }
                break;
        };

        curr = next + 1;
    }

    // Add the remainder of the string, if any
    if(curr)
    {
        ekStringConcat(E, str, curr);
    }

    ekContextPopValues(E, argCount);
    ekValueRemoveRefNote(E, format, "FORMAT format done");
    return ret;
}

ekValue *ekValueIndex(struct ekContext *E, ekValue *p, ekValue *index, ekBool lvalue)
{
    return ekValueTypeSafeCall(p->type, Index)(E, p, index, lvalue);
}

const char *ekValueTypeName(struct ekContext *E, int type)
{
    if((type >= 0) && (type < ekArraySize(E, &E->types)))
    {
        ekValueType *valueType = E->types[type];
        return valueType->name;
    }
    return "unknown";
}

void ekValueDump(struct ekContext *E, ekDumpParams *params, ekValue *p)
{
    ekValueTypeSafeCall(p->type, Dump)(E, params, p);
}

#ifdef EUREKA_TRACE_REFS
void ekValueTraceRefs(struct ekContext *E, struct ekValue *p, int delta, const char *note)
{
    const char *destroyed = "";
    char tempPtr[32];
    int newRefs = p->refs;
    if(p == ekValueNullPtr)
    {
        sprintf(tempPtr, "0xEK_NULL");
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

void ekValueRemoveRefArray(struct ekContext *E, struct ekValue *p)
{
    ekValueTraceRefs(E, p, -1, "array cleared");
    ekValueRemoveRef(E, p);
}

void ekValueRemoveRefHashed(struct ekContext *E, struct ekValue *p)
{
    ekValueTraceRefs(E, p, -1, "hash cleared");
    ekValueRemoveRef(E, p);
}
#endif
