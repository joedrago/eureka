#include "yapValue.h"

#include "yapFrame.h"
#include "yapHash.h"
#include "yapIntrinsics.h"
#include "yapObject.h"
#include "yapLexer.h"
#include "yapVariable.h"
#include "yapVM.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>

// Enabling this allows for arrays to return their length by doing a.length(), etc
// #define ENABLE_ARRAY_OO_FUNCTIONS

static char *NULL_STRING_FORM = "[null]";

// ---------------------------------------------------------------------------
// Helper funcs

// adds a yapCFunction pointer to a hash
static void addFunc(yapHash *yh, const char *name, yapCFunction func)
{
    void **ref = yapHashLookup(yh, name, yTrue);
    yapAssert(ref);
    *ref = (void*)func;
}

// TODO: Make a real string class that isn't terrible
static char *concat(char *a, char *b)
{
    yU32 newLen = (yU32)strlen(a) + (yU32)strlen(b);
    char *newString = yapAlloc(newLen + 1);
    strcpy(newString, a); // TODO: make this smarter
    strcat(newString, b);
    return newString;
}

static yapValue *yapValueConcat(struct yapVM *vm, yapValue *a, yapValue *b)
{
    a = yapValueToString(vm, a);
    b = yapValueToString(vm, b);
    a = yapValueDonateString(vm, a, concat(a->stringVal, b->stringVal));
    return a;
}

// ---------------------------------------------------------------------------

yapClosureVariable *yapClosureVariableCreate(const char *name, yapVariable *variable)
{
    yapClosureVariable *cv = yapAlloc(sizeof(yapClosureVariable));
    cv->name = yapStrdup(name);
    cv->variable = variable;
    return cv;
}

void yapClosureVariableDestroy(yapClosureVariable *cv)
{
    free(cv->name);
    free(cv);
}

// ---------------------------------------------------------------------------

yapValueType *yapValueTypeCreate(const char *name)
{
    yapValueType *type = yapAlloc(sizeof(yapValueType));
    strcpy(type->name, name);
    return type;
}

void yapValueTypeDestroy(yapValueType *type)
{
    if(type->funcDestroyUserData)
        type->funcDestroyUserData(type);
    yapFree(type);
}

int yapValueTypeRegister(struct yapVM *vm, yapValueType *newType)
{
    int i;
    for(i=0; i<vm->types.count; i++)
    {
        yapValueType *t = vm->types.data[i];
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

    newType->id = yapArrayPush(&vm->types, newType);
    return newType->id;
}

// ---------------------------------------------------------------------------
// YVT_NULL Funcs

static struct yapValue * nullFuncToString(struct yapVM *vm, struct yapValue *p)
{
    return yapValueSetKString(vm, yapValueAcquire(vm), NULL_STRING_FORM);
}

static void nullFuncRegister(struct yapVM *vm)
{
    yapValueType *type   = yapValueTypeCreate("null");
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
    yapValueTypeRegister(vm, type);
    yapAssert(type->id == YVT_NULL);
}

// ---------------------------------------------------------------------------
// YVT_BLOCK Funcs

static void blockFuncClear(struct yapValue *p)
{
    if(p->closureVars)
        yapArrayDestroy(p->closureVars, (yapDestroyCB)yapClosureVariableDestroy);
}

static void blockFuncClone(struct yapVM *vm, struct yapValue *dst, struct yapValue *src)
{
    dst->blockVal = src->blockVal;
}

static void blockFuncMark(struct yapVM *vm, struct yapValue *value)
{
    if(value->closureVars)
    {
        int i;
        for(i=0; i<value->closureVars->count; i++)
        {
            yapClosureVariable *cv = (yapClosureVariable *)value->closureVars->data[i];
            yapVariableMark(vm, cv->variable);
        }
    }
}

static yBool blockFuncToBool(struct yapValue *p)
{
    return yTrue;
}

static yS32 blockFuncToInt(struct yapValue *p)
{
    return 1; // ?
}

static yF32 blockFuncToFloat(struct yapValue *p)
{
    return 1.0f; // ?
}

static void blockFuncRegister(struct yapVM *vm)
{
    yapValueType *type = yapValueTypeCreate("block");
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
    yapValueTypeRegister(vm, type);
    yapAssert(type->id == YVT_BLOCK);
}

// ---------------------------------------------------------------------------
// YVT_CFUNCTION Funcs

static void cfunctionFuncClone(struct yapVM *vm, struct yapValue *dst, struct yapValue *src)
{
    dst->cFuncVal = src->cFuncVal;
}

static yBool cfunctionFuncToBool(struct yapValue *p)
{
    return yTrue;
}

static yS32 cfunctionFuncToInt(struct yapValue *p)
{
    return 1; // ?
}

static yF32 cfunctionFuncToFloat(struct yapValue *p)
{
    return 1.0f; // ?
}

static void cfunctionFuncRegister(struct yapVM *vm)
{
    yapValueType *type = yapValueTypeCreate("cfunction");
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
    yapValueTypeRegister(vm, type);
    yapAssert(type->id == YVT_CFUNCTION);
}

// ---------------------------------------------------------------------------
// YVT_INT Funcs

static void intFuncClone(struct yapVM *vm, struct yapValue *dst, struct yapValue *src)
{
    dst->intVal = src->intVal;
}

static yBool intFuncToBool(struct yapValue *p)
{
    return (p->intVal) ? yTrue : yFalse;
}

static yS32 intFuncToInt(struct yapValue *p)
{
    return p->intVal;
}

static yF32 intFuncToFloat(struct yapValue *p)
{
    return (yF32)p->intVal;
}

static struct yapValue * intFuncToString(struct yapVM *vm, struct yapValue *p)
{
    char temp[32];
    sprintf(temp, "%d", p->intVal);
    return yapValueSetString(vm, p, temp);
}

static struct yapValue * intFuncArithmetic(struct yapVM *vm, struct yapValue *a, struct yapValue *b, yapValueArithmeticOp op)
{
    b = yapValueToInt(vm, b);
    switch(op)
    {
    case YVAO_ADD:
        a = yapValueSetInt(vm, a, a->intVal + b->intVal);
        break;
    case YVAO_SUB:
        a = yapValueSetInt(vm, a, a->intVal - b->intVal);
        break;
    case YVAO_MUL:
        a = yapValueSetInt(vm, a, a->intVal * b->intVal);
        break;
    case YVAO_DIV:
        if(!b->intVal)
        {
            yapVMSetError(vm, YVE_RUNTIME, "divide by zero!");
            return NULL;
        }
        a = yapValueSetInt(vm, a, a->intVal / b->intVal);
        break;
    };
    return a;
}

static yBool intFuncCmp(struct yapVM *vm, struct yapValue *a, struct yapValue *b, int *cmpResult)
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

static void intFuncRegister(struct yapVM *vm)
{
    yapValueType *type = yapValueTypeCreate("int");
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
    yapValueTypeRegister(vm, type);
    yapAssert(type->id == YVT_INT);
}

// ---------------------------------------------------------------------------
// YVT_FLOAT Funcs

static void floatFuncClone(struct yapVM *vm, struct yapValue *dst, struct yapValue *src)
{
    dst->floatVal = src->floatVal;
}

static yBool floatFuncToBool(struct yapValue *p)
{
    return (p->floatVal != 0.0f) ? yTrue : yFalse;
}

static yS32 floatFuncToInt(struct yapValue *p)
{
    return (yS32)p->floatVal;
}

static yF32 floatFuncToFloat(struct yapValue *p)
{
    return p->floatVal;
}

static struct yapValue * floatFuncToString(struct yapVM *vm, struct yapValue *p)
{
    char temp[64];
    sprintf(temp, "%f", p->floatVal);
    return yapValueSetString(vm, p, temp);
}

static struct yapValue * floatFuncArithmetic(struct yapVM *vm, struct yapValue *a, struct yapValue *b, yapValueArithmeticOp op)
{
    b = yapValueToFloat(vm, b);
    switch(op)
    {
    case YVAO_ADD:
        a = yapValueSetFloat(vm, a, a->floatVal + b->floatVal);
        break;
    case YVAO_SUB:
        a = yapValueSetFloat(vm, a, a->floatVal - b->floatVal);
        break;
    case YVAO_MUL:
        a = yapValueSetFloat(vm, a, a->floatVal * b->floatVal);
        break;
    case YVAO_DIV:
        if(b->floatVal == 0.0f)
        {
            yapVMSetError(vm, YVE_RUNTIME, "divide by zero!");
            return NULL;
        }
        a = yapValueSetFloat(vm, a, a->floatVal / b->floatVal);
        break;
    };
    return a;
}

static yBool floatFuncCmp(struct yapVM *vm, struct yapValue *a, struct yapValue *b, int *cmpResult)
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

static void floatFuncRegister(struct yapVM *vm)
{
    yapValueType *type = yapValueTypeCreate("float");
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
    yapValueTypeRegister(vm, type);
    yapAssert(type->id == YVT_FLOAT);
}

// ---------------------------------------------------------------------------
// YVT_STRING Funcs

static void stringFuncClear(struct yapValue *p)
{
    if(!p->constant)
        yapFree(p->stringVal);
}

static void stringFuncClone(struct yapVM *vm, struct yapValue *dst, struct yapValue *src)
{
    if(src->constant)
        dst->stringVal = src->stringVal;
    else
        dst->stringVal = yapStrdup(src->stringVal);
}

static yBool stringFuncToBool(struct yapValue *p)
{
    return (p->stringVal[0] != 0) ? yTrue : yFalse;
}

static yS32 stringFuncToInt(struct yapValue *p)
{
    yapToken t = { p->stringVal, strlen(p->stringVal) };
    return yapTokenToInt(&t);
}

static yF32 stringFuncToFloat(struct yapValue *p)
{
    yapToken t = { p->stringVal, strlen(p->stringVal) };
    return yapTokenToFloat(&t);
}

struct yapValue * stringFuncToString(struct yapVM *vm, struct yapValue *p)
{
    return p;
}

struct yapValue * stringFuncArithmetic(struct yapVM *vm, struct yapValue *a, struct yapValue *b, yapValueArithmeticOp op)
{
    if(op == YVAO_ADD)
        return yapValueConcat(vm, a, b);
    printf("stringFuncArithmetic(): cannot subtract, multiply, or divide strings!\n");
    return NULL;
}

static yBool stringFuncCmp(struct yapVM *vm, struct yapValue *a, struct yapValue *b, int *cmpResult)
{
    if(b->type == YVT_STRING)
    {
        *cmpResult = strcmp(a->stringVal, b->stringVal);
        return yTrue;
    }
    return yFalse;
}

static void stringFuncRegister(struct yapVM *vm)
{
    yapValueType *type = yapValueTypeCreate("string");
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
    yapValueTypeRegister(vm, type);
    yapAssert(type->id == YVT_STRING);
}

// ---------------------------------------------------------------------------
// YVT_ARRAY Funcs

static void arrayFuncClear(struct yapValue *p)
{
    yapArrayDestroy(p->arrayVal, NULL);
}

static void arrayFuncClone(struct yapVM *vm, struct yapValue *dst, struct yapValue *src)
{
    yapAssert(0 && "arrayFuncClone not implemented");
}

static void arrayFuncMark(struct yapVM *vm, struct yapValue *value)
{
    int i;
    for(i = 0; i < value->arrayVal->count; i++)
    {
        yapValue *child = (yapValue *)value->arrayVal->data[i];
        yapValueMark(vm, child);
    }
}

static yBool arrayFuncToBool(struct yapValue *p)
{
    return (p->arrayVal && p->arrayVal->count) ? yTrue : yFalse;
}

static yS32 arrayFuncToInt(struct yapValue *p)
{
    return (p->arrayVal) ? p->arrayVal->count : 0;
}

static yF32 arrayFuncToFloat(struct yapValue *p)
{
    return (p->arrayVal) ? (yF32)p->arrayVal->count : 0;
}

static struct yapValue * arrayFuncIndex(struct yapVM *vm, struct yapValue *value, struct yapValue *index, yBool lvalue)
{
    yapValue *ret = NULL;

#ifdef ENABLE_ARRAY_OO_FUNCTIONS
    if(index->type == YVT_STRING)
    {
        yapHash *yh = (yapHash *)((yapValueType*)vm->types.data[value->type])->userData;
        void **funcRef = yapHashLookup(yh, index->stringVal, yFalse);
        if(funcRef)
        {
            if(lvalue)
            {
                yapVMSetError(vm, YVE_RUNTIME, "Cannot use array function as an lvalue");
            }
            else
            {
                ret = yapValueSetCFunction(vm, yapValueAcquire(vm), *funcRef);
            }
        }
    }
    if(!ret && (vm->errorType == YVE_NONE))
#endif
    {
        yapValue **ref = NULL;
        index = yapValueToInt(vm, index);
        if(index->intVal >= 0 && index->intVal < value->arrayVal->count)
        {
            ref = (yapValue **) & (value->arrayVal->data[index->intVal]);
            if(lvalue)
                ret = yapValueSetRef(vm, yapValueAcquire(vm), ref);
            else
                ret = *ref;
        }
        else
        {
            yapVMSetError(vm, YVE_RUNTIME, "array index %d out of range", index->intVal);
        }
    }
    return ret;
}

#ifdef ENABLE_ARRAY_OO_FUNCTIONS
static void arrayFuncDestroyUserData(struct yapValueType *valueType)
{
    if(valueType->userData)
        yapHashDestroy((yapHash*)valueType->userData, NULL);
}
#endif

static void arrayFuncRegister(struct yapVM *vm)
{
    yapValueType *type = yapValueTypeCreate("array");
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
    yapValueTypeRegister(vm, type);
    yapAssert(type->id == YVT_ARRAY);

#ifdef ENABLE_ARRAY_OO_FUNCTIONS
    type->funcDestroyUserData = arrayFuncDestroyUserData;
    type->userData = yapHashCreate(0);
    addFunc(type->userData, "push", array_push);
    addFunc(type->userData, "length", array_length);
#endif
}

// ---------------------------------------------------------------------------
// YVT_OBJECT Funcs

static void objectFuncClear(struct yapValue *p)
{
    yapObjectDestroy(p->objectVal);
}

static void objectFuncClone(struct yapVM *vm, struct yapValue *dst, struct yapValue *src)
{
    yapAssert(0 && "objectFuncClone not implemented");
}

static void objectFuncMark(struct yapVM *vm, struct yapValue *value)
{
    yapObjectMark(vm, value->objectVal);
}

static yBool objectFuncToBool(struct yapValue *p)
{
    return yTrue;
}

static yS32 objectFuncToInt(struct yapValue *p)
{
    return 1; // ?
}

static yF32 objectFuncToFloat(struct yapValue *p)
{
    return 1.0f; // ?
}

static struct yapValue * objectFuncToString(struct yapVM *vm, struct yapValue *p)
{
    char temp[32];
    sprintf(temp, "[object:%p]", p->objectVal);
    return yapValueSetString(vm, yapValueAcquire(vm), temp);
}

static struct yapValue * objectFuncIndex(struct yapVM *vm, struct yapValue *value, struct yapValue *index, yBool lvalue)
{
    yapValue *ret = NULL;
    yapValue **ref = NULL;
    index = yapValueToString(vm, index);
    ref = yapObjectGetRef(vm, value->objectVal, index->stringVal, lvalue /* create? */);
    if(lvalue)
        ret = yapValueSetRef(vm, yapValueAcquire(vm), ref);
    else
        ret = *ref;
    return ret;
}

static void objectFuncRegister(struct yapVM *vm)
{
    yapValueType *type = yapValueTypeCreate("object");
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
    yapValueTypeRegister(vm, type);
    yapAssert(type->id == YVT_OBJECT);
}

// ---------------------------------------------------------------------------
// YVT_REF Funcs

static void refFuncClone(struct yapVM *vm, struct yapValue *dst, struct yapValue *src)
{
    dst->refVal = src->refVal;
}

static void refFuncMark(struct yapVM *vm, struct yapValue *value)
{
    if(*value->refVal)
    {
        yapValueMark(vm, *value->refVal);
    }
}

static yBool refFuncToBool(struct yapValue *p)
{
    return (*p->refVal) ? yTrue : yFalse;
}

static yS32 refFuncToInt(struct yapValue *p)
{
    return 1; // ?
}

static yF32 refFuncToFloat(struct yapValue *p)
{
    return 1.0f; // ?
}

static void refFuncRegister(struct yapVM *vm)
{
    yapValueType *type = yapValueTypeCreate("ref");
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
    yapValueTypeRegister(vm, type);
    yapAssert(type->id == YVT_REF);
}

// ---------------------------------------------------------------------------

void yapValueTypeRegisterAllBasicTypes(struct yapVM *vm)
{
    nullFuncRegister(vm);
    blockFuncRegister(vm);
    cfunctionFuncRegister(vm);
    intFuncRegister(vm);
    floatFuncRegister(vm);
    stringFuncRegister(vm);
    arrayFuncRegister(vm);
    objectFuncRegister(vm);
    refFuncRegister(vm);
}

// ---------------------------------------------------------------------------

yapValue yapValueNull = {YVT_NULL};
yapValue *yapValueNullPtr = &yapValueNull;

yapValue *yapValueSetInt(struct yapVM *vm, yapValue *p, int v)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(vm, p);
    p->type = YVT_INT;
    p->intVal = v;
    p->used = yTrue;
    yapTrace(("yapValueSetInt %p [%d]\n", p, v));
    return p;
}

yapValue *yapValueSetFloat(struct yapVM *vm, yapValue *p, yF32 v)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(vm, p);
    p->type = YVT_FLOAT;
    p->floatVal = v;
    p->used = yTrue;
    yapTrace(("yapValueSetFloat %p [%f]\n", p, v));
    return p;
}

yapValue *yapValueSetKString(struct yapVM *vm, yapValue *p, char *s)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(vm, p);
    p->type = YVT_STRING;
    p->stringVal = s;
    p->constant = yTrue;
    p->used = yTrue;
    yapTrace(("yapValueSetKString %p\n", p));
    return p;
}

yapValue *yapValueSetString(struct yapVM *vm, yapValue *p, char *s)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(vm, p);
    p->type = YVT_STRING;
    p->stringVal = yapStrdup(s);
    p->constant = yFalse;
    p->used = yTrue;
    yapTrace(("yapValueSetString %p\n", p));
    return p;
}

yapValue *yapValueDonateString(struct yapVM *vm, yapValue *p, char *s)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(vm, p);
    p->type = YVT_STRING;
    p->stringVal = s;
    p->constant = yFalse;
    p->used = yTrue;
    yapTrace(("yapValueDonateString %p\n", p));
    return p;
}

static void yapValueAddClosureVar(yapArray *closureVars, yapHashEntry *entry)
{
    yapClosureVariable *cv = yapClosureVariableCreate(entry->key, entry->value);
    yapArrayPush(closureVars, cv);
}

void yapValueAddClosureVars(struct yapVM *vm, yapValue *p)
{
    yapFrame *frame;
    int frameIndex;

    if(p->type != YVT_BLOCK)
    {
        // TODO: add impossibly catastrophic error here? (can't happen?)
        return;
    }

    yapAssert(p->closureVars == NULL);

    for(frameIndex = vm->frames.count - 1; frameIndex >= 0; frameIndex--)
    {
        frame = vm->frames.data[frameIndex];
        if((frame->type & (YFT_CHUNK|YFT_FUNC)) == YFT_FUNC) // we are inside of an actual function!
            break;
    }

    if(frameIndex >= 0)
    {
        for( ; frameIndex < vm->frames.count; frameIndex++)
        {
            frame = vm->frames.data[frameIndex];
            if(frame->locals->count)
            {
                int i;
                if(!p->closureVars)
                    p->closureVars = yapArrayCreate();
                yapHashIterateP1(frame->locals, (yapIterateCB1)yapValueAddClosureVar, p->closureVars);
            }
        }
    }
}

yapValue *yapValueSetFunction(struct yapVM *vm, yapValue *p, struct yapBlock *block)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(vm, p);
    p->type = YVT_BLOCK;
    p->closureVars = NULL;
    p->blockVal = block;
    p->used = yTrue;
    yapTrace(("yapValueSetFunction %p\n", p));
    return p;
}

yapValue *yapValueSetCFunction(struct yapVM *vm, yapValue *p, yapCFunction func)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(vm, p);
    p->type = YVT_CFUNCTION;
    p->cFuncVal = func;
    p->used = yTrue;
    yapTrace(("yapValueSetCFunction %p\n", p));
    return p;
}

yapValue *yapValueSetRef(struct yapVM *vm, yapValue *p, struct yapValue **ref)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(vm, p);
    p->type = YVT_REF;
    p->refVal = ref;
    p->used = yTrue;
    yapTrace(("yapValueSetRef %p\n", p));
    return p;
}

yapValue *yapValueSetObject(struct yapVM *vm, yapValue *p, struct yapObject *object)
{
    p = yapValuePersonalize(vm, p);
    yapValueClear(vm, p);
    p->type = YVT_OBJECT;
    p->objectVal = object;
    p->used = yTrue;
    yapTrace(("yapValueSetObject %p\n", p));
    return p;
}

static yBool yapValueCheckRef(struct yapVM *vm, yapValue *ref, yapValue *p)
{
    if(!p)
    {
        yapVMSetError(vm, YVE_RUNTIME, "yapValueSetRefVal: empty stack!");
        return yFalse;
    }
    if(!ref)
    {
        yapVMSetError(vm, YVE_RUNTIME, "yapValueSetRefVal: empty stack!");
        return yFalse;
    }
    if(!(*ref->refVal))
    {
        yapVMSetError(vm, YVE_RUNTIME, "yapValueSetRefVal: missing ref!");
        return yFalse;
    }
    if(ref->type != YVT_REF)
    {
        yapVMSetError(vm, YVE_RUNTIME, "yapValueSetRefVal: value on top of stack, ref underneath");
        return yFalse;
    }
    return yTrue;
}

yBool yapValueSetRefVal(struct yapVM *vm, yapValue *ref, yapValue *p)
{
    if(!yapValueCheckRef(vm, ref, p))
        return yFalse;

    *(ref->refVal) = p;
    p->used = yTrue;

    yapTrace(("yapValueSetRefVal %p = %p\n", ref, p));
    return yTrue;
}

yBool yapValueSetRefInherits(struct yapVM *vm, yapValue *ref, yapValue *p)
{
    if(!yapValueCheckRef(vm, ref, p))
        return yFalse;

    if(*(ref->refVal) == &yapValueNull)
    {
        *(ref->refVal) = yapValueObjectCreate(vm, p, 0);
    }

    if((*(ref->refVal))->type != YVT_OBJECT)
    {
        yapVMSetError(vm, YVE_RUNTIME, "yapValueSetRefInherits: non-objects cannot inherit");
        return yFalse;
    }

    (*(ref->refVal))->objectVal->isa = p;
    p->used = yTrue;

    yapTrace(("yapValueSetRefInherits %p = %p\n", ref, p));
    return yTrue;
}

// ---------------------------------------------------------------------------

yapValue *yapValueArrayCreate(struct yapVM *vm)
{
    yapValue *p = yapValueAcquire(vm);
    p->arrayVal = yapArrayCreate();
    p->type = YVT_ARRAY;
    p->used = yTrue;
    return p;
}

void yapValueArrayPush(yapVM *vm, yapValue *p, yapValue *v)
{
    if(p->type != YVT_ARRAY)
    {
        p = yapValuePersonalize(vm, p);
        yapValueClear(vm, p);
        p->arrayVal = yapArrayCreate();
        p->type = YVT_ARRAY;
    }

    yapArrayPush(p->arrayVal, v);
    v->used = yTrue;
}

// ---------------------------------------------------------------------------

yapValue *yapValueObjectCreate(struct yapVM *vm, struct yapValue *isa, int argCount)
{
    yapValue *p = yapValueAcquire(vm);
    p->objectVal = yapObjectCreate(vm, isa);
    p->type = YVT_OBJECT;
    p->used = yTrue;

    if(argCount)
    {
        int i;
        for(i=0; i<argCount; i+=2)
        {
            yapValue **ref;
            yapValue *key = yapVMGetArg(vm, i, argCount);
            yapValue *val = yapValueNullPtr;
            int valueArg = i+1;
            if(valueArg < argCount)
            {
                val = yapVMGetArg(vm, valueArg, argCount);
            }
            key = yapValueToString(vm, key);
            ref = yapObjectGetRef(vm, p->objectVal, key->stringVal, yTrue);
            *ref = val;
        }
        yapVMPopValues(vm, argCount);
    }
    return p;
}

void yapValueObjectSetMember(struct yapVM *vm, struct yapValue *object, const char *name, struct yapValue *value)
{
    yapValue **ref = NULL;
    yapAssert(object->type == YVT_OBJECT);
    ref = yapObjectGetRef(vm, object->objectVal, name, yTrue);
    yapAssert(ref);
    *ref = value;
}

// ---------------------------------------------------------------------------

void yapValueClear(struct yapVM *vm, yapValue *p)
{
    yapValueTypeSafeCall(p->type, Clear)(p);

    memset(p, 0, sizeof(*p));
    p->type = YVT_NULL;
}

void yapValueRelease(yapVM *vm, yapValue *p)
{
    yapTrace(("yapValueRelease %p\n", p));
    yapValueClear(vm, p);
    yapArrayPush(&vm->freeValues, p);
}

void yapValueDestroy(yapVM *vm, yapValue *p)
{
    yapTrace(("yapValueFree %p\n", p));
    yapValueClear(vm, p);
    yapFree(p);
}

static yapValue *yapValueCreate()
{
    yapValue *value = yapAlloc(sizeof(yapValue));
    yapTrace(("yapValueCreate %p\n", value));
    return value;
}

yapValue *yapValueAcquire(struct yapVM *vm)
{
    yapValue *value = yapArrayPop(&vm->freeValues);
    if(!value)
    {
        value = yapValueCreate();
    };
    yapArrayPush(&vm->usedValues, value);
    yapTrace(("yapValueAcquire %p\n", value));
    return value;
}

yS32 yapValueCmp(struct yapVM *vm, yapValue *a, yapValue *b)
{
    if(a == b)
        return 0; // this should also handle the NULL case

    if(a && b)
    {
        yS32 ret = 0;
        if(yapValueTypeSafeCall(a->type, Cmp)(vm, a, b, &ret))
            return ret;
    }

    return (yS32)(a - b); // Fallback case: compare pointers for consistency
}

void yapValueCloneData(struct yapVM *vm, yapValue *dst, yapValue *src)
{
    dst->type = src->type;
    dst->constant = src->constant;
    yapValueTypeSafeCall(dst->type, Clone)(vm, dst, src);
}

yapValue *yapValueClone(struct yapVM *vm, yapValue *p)
{
    yapValue *n = yapValueAcquire(vm);
    yapValueCloneData(vm, n, p);
    yapTrace(("yapValueClone %p -> %p\n", p, n));
    return n;
}

yapValue *yapValuePersonalize(struct yapVM *vm, yapValue *p)
{
    if(p == &yapValueNull)
        return yapValueAcquire(vm);

    if(p->used)
        return yapValueClone(vm, p);

    return p;
}

void yapValueMark(struct yapVM *vm, yapValue *value)
{
    if(value->type == YVT_NULL)
        return;

    if(value->used)
        return;

    value->used = yTrue;

    yapValueTypeSafeCall(value->type, Mark)(vm, value);
}

yapValue *yapValueAdd(struct yapVM *vm, yapValue *a, yapValue *b)
{
    yapValue *value = yapValueTypeSafeCall(a->type, Arithmetic)(vm, a, b, YVAO_ADD);
    if(!value)
        yapTrace(("Don't know how to add types %s and %s\n", yapValueTypePtr(a->type)->name, yapValueTypePtr(b->type)->name));
    return value;
}

yapValue *yapValueSub(struct yapVM *vm, yapValue *a, yapValue *b)
{
    yapValue *value = yapValueTypeSafeCall(a->type, Arithmetic)(vm, a, b, YVAO_SUB);
    if(!value)
        yapTrace(("Don't know how to subtract types %s and %s\n", yapValueTypePtr(a->type)->name, yapValueTypePtr(b->type)->name));
    return value;
}

yapValue *yapValueMul(struct yapVM *vm, yapValue *a, yapValue *b)
{
    yapValue *value = yapValueTypeSafeCall(a->type, Arithmetic)(vm, a, b, YVAO_MUL);
    if(!value)
        yapTrace(("Don't know how to multiply types %s and %s\n", yapValueTypePtr(a->type)->name, yapValueTypePtr(b->type)->name));
    return value;
}

yapValue *yapValueDiv(struct yapVM *vm, yapValue *a, yapValue *b)
{
    yapValue *value = yapValueTypeSafeCall(a->type, Arithmetic)(vm, a, b, YVAO_DIV);
    if(!value)
        yapTrace(("Don't know how to divide types %s and %s\n", yapValueTypePtr(a->type)->name, yapValueTypePtr(b->type)->name));
    return value;
}

yapValue *yapValueToBool(struct yapVM *vm, yapValue *p)
{
    yBool boolVal = yapValueTypeSafeCall(p->type, ToBool)(p);
    return yapValueSetInt(vm, yapValueAcquire(vm), boolVal);
}

yapValue *yapValueToInt(struct yapVM *vm, yapValue *p)
{
    yS32 intVal = yapValueTypeSafeCall(p->type, ToInt)(p);
    return yapValueSetInt(vm, yapValueAcquire(vm), intVal);
}

yapValue *yapValueToFloat(struct yapVM *vm, yapValue *p)
{
    yF32 floatVal = yapValueTypeSafeCall(p->type, ToFloat)(p);
    return yapValueSetFloat(vm, yapValueAcquire(vm), floatVal);
}

yapValue *yapValueToString(struct yapVM *vm, yapValue *p)
{
    yapValue *value = yapValueTypeSafeCall(p->type, ToString)(vm, p);
    if(!value)
    {
        printf("yapValueToString: unable to convert type '%s' to string\n", yapValueTypePtr(p->type)->name);
    }
    return value;
}

yapValue *yapValueStringFormat(struct yapVM *vm, yapValue *format, yS32 argCount)
{
    char *out = yapStrdup("");
    int outSize = 1;
    int outPos = 0;
    int addLen;

    char *curr = format->stringVal;
    char *next;

    yapValue *arg;
    int argIndex = 0;

    while(curr && (next = strchr(curr, '%')))
    {
        // First, add in all of the stuff before the %
        {
            addLen = next - curr;
            outSize += addLen;
            out = yapRealloc(out, outSize);
            memcpy(&out[outPos], curr, addLen);
            outPos += addLen;
        }
        next++;

        switch(*next)
        {
        case '\0':
            curr = NULL;
            break;
        case '%':
            out = yapRealloc(out, ++outSize);
            out[outPos++] = '%';
            break;
        case 's':
            arg = yapVMGetArg(vm, argIndex++, argCount);
            if(arg)
            {
                arg = yapValueToString(vm, arg);
                addLen = strlen(arg->stringVal);
                outSize += addLen;
                out = yapRealloc(out, outSize);
                memcpy(&out[outPos], arg->stringVal, addLen);
                outPos += addLen;
            }
            break;
        case 'd':
            arg = yapVMGetArg(vm, argIndex++, argCount);
            if(arg)
            {
                char temp[32];
                arg = yapValueToInt(vm, arg);
                sprintf(temp, "%d", arg->intVal);
                addLen = strlen(temp);
                outSize += addLen;
                out = yapRealloc(out, outSize);
                memcpy(&out[outPos], temp, addLen);
                outPos += addLen;
            }
            break;
        case 'f':
            arg = yapVMGetArg(vm, argIndex++, argCount);
            if(arg)
            {
                char temp[32];
                arg = yapValueToFloat(vm, arg);
                sprintf(temp, "%f", arg->floatVal);
                addLen = strlen(temp);
                outSize += addLen;
                out = yapRealloc(out, outSize);
                memcpy(&out[outPos], temp, addLen);
                outPos += addLen;
            }
            break;
        };

        curr = next + 1;
    }

    // Add the remainder of the string, if any
    if(curr)
    {
        addLen = strlen(curr);
        outSize += addLen;
        out = yapRealloc(out, outSize);
        memcpy(&out[outPos], curr, addLen);
        outPos += addLen;
    }

    // Terminate the string
    out[outPos] = 0;

    format = yapValueAcquire(vm);
    format->type = YVT_STRING;
    format->stringVal = out;

    yapVMPopValues(vm, argCount);
    return format;
}

yapValue *yapValueIndex(struct yapVM *vm, yapValue *p, yapValue *index, yBool lvalue)
{
    return yapValueTypeSafeCall(p->type, Index)(vm, p, index, lvalue);
}

const char *yapValueTypeName(struct yapVM *vm, int type)
{
    if((type >= 0) && (type < vm->types.count))
    {
        yapValueType *valueType = ((yapValueType*)vm->types.data[type]);
        return valueType->name;
    }
    return "unknown";
}