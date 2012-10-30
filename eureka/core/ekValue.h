// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EUREKAVALUE_H
#define EUREKAVALUE_H

#include "ekString.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekMap;
struct ekObject;
struct ekValue;

// ---------------------------------------------------------------------------

typedef enum ekValueBasicType
{
    YVT_NULL,                          // Must stay zero, so that calloc/memset sets a ekValue to NULL

    YVT_BLOCK,
    YVT_CFUNCTION,

    YVT_INT,
    YVT_FLOAT,
    YVT_STRING,

    YVT_ARRAY,
    YVT_OBJECT,

    YVT_REF,                           // Value reference

    YVT_COUNT,
    YVT_CUSTOM = YVT_COUNT,            // First custom value type
    YVT_INVALID = -1
} ekValueBasicType;

typedef enum ekValueArithmeticOp
{
    YVAO_ADD = 0,
    YVAO_SUB,
    YVAO_MUL,
    YVAO_DIV,

    YVAO_COUNT
} ekValueArithmeticOp;

typedef struct ekDumpParams
{
    ekString output;  // The final output value. Dump functions should use concatenate functions on this string, not set
    ekString tempStr; // to be used as a temporary string by the dump function, and should be considered to be changed if recursively calling dump()
    int tempInt;       // to be used as a temporary int by the dump function, and should be considered to be changed if recursively calling dump()
} ekDumpParams;

ekDumpParams *ekDumpParamsCreate(struct ekContext *Y);
void ekDumpParamsDestroy(struct ekContext *Y, ekDumpParams *params);

#define YVT_MAXNAMELEN 15

struct ekValueType;
typedef void (*ekValueTypeDestroyUserData)(struct ekContext *Y, struct ekValueType *valueType);

typedef void (*ekValueTypeFuncClear)(struct ekContext *Y, struct ekValue *p);
typedef void (*ekValueTypeFuncClone)(struct ekContext *Y, struct ekValue *dst, struct ekValue *src);
typedef yBool(*ekValueTypeFuncToBool)(struct ekContext *Y, struct ekValue *p);
typedef yS32(*ekValueTypeFuncToInt)(struct ekContext *Y, struct ekValue *p);
typedef yF32(*ekValueTypeFuncToFloat)(struct ekContext *Y, struct ekValue *p);
typedef struct ekValue *(*ekValueTypeFuncToString)(struct ekContext *Y, struct ekValue *p);
typedef struct ekValue *(*ekValueTypeFuncArithmetic)(struct ekContext *Y, struct ekValue *a, struct ekValue *b, ekValueArithmeticOp op);
typedef yBool(*ekValueTypeFuncCmp)(struct ekContext *Y, struct ekValue *a, struct ekValue *b, int *cmpResult);
typedef struct ekValue *(*ekValueTypeFuncIndex)(struct ekContext *Y, struct ekValue *p, struct ekValue *index, yBool lvalue);
typedef void (*ekValueTypeFuncDump)(struct ekContext *Y, ekDumpParams *params, struct ekValue *p); // creates debug text representing value, caller responsible for ekFree()

// This is used to enforce the setting of every function ptr in a ekValueType*; an explicit alternative to NULL
#define ekValueTypeFuncNotUsed ((void*)-1)

typedef struct ekValueType
{
    int id;
    char name[YVT_MAXNAMELEN + 1];

    void *userData;                                  // per-type global structure used to hold any static data a type needs
    ekValueTypeDestroyUserData funcDestroyUserData; // optional destructor for userdata

    ekValueTypeFuncClear funcClear;
    ekValueTypeFuncClone funcClone;
    ekValueTypeFuncToBool funcToBool;
    ekValueTypeFuncToInt funcToInt;
    ekValueTypeFuncToFloat funcToFloat;
    ekValueTypeFuncToString funcToString;
    ekValueTypeFuncArithmetic funcArithmetic;
    ekValueTypeFuncCmp funcCmp;
    ekValueTypeFuncIndex funcIndex;
    ekValueTypeFuncDump funcDump;
} ekValueType;

ekValueType *ekValueTypeCreate(struct ekContext *Y, const char *name);
void ekValueTypeDestroy(struct ekContext *Y, ekValueType *type);
int ekValueTypeRegister(struct ekContext *Y, ekValueType *newType); // takes ownership of newType (use ekAlloc), returns new type id

void ekValueTypeRegisterAllBasicTypes(struct ekContext *Y);

#define ekValueTypePtr(id) ((ekValueType*)Y->types[id])

// If the function ptr doesn't exist, just return 0 (NULL) safely, otherwise call it with arguments after the macro
#define ekValueTypeSafeCall(id, funcName) \
    (((ekValueType*)Y->types[id])->func ## funcName == ekValueTypeFuncNotUsed) ? 0 \
    : ((ekValueType*)Y->types[id])->func ## funcName

// ---------------------------------------------------------------------------

// Should return how many values it is returning on the stack
typedef yU32(ekCFunction)(struct ekContext *Y, yU32 argCount);

// ---------------------------------------------------------------------------

typedef struct ekValue
{
    yU8 type;
    yS32 refs;                            // reference count!
    union
    {
        yS32 intVal;
        yF32 floatVal;
        struct
        {
            struct ekMap *closureVars;   // Populated at runtime when a reference to a new function() is created
            struct ekBlock *blockVal;    // Hurr, Shield Slam
        };
        ekString stringVal;
        struct ekValue **refVal;
        ekCFunction *cFuncVal;
        struct ekValue **arrayVal;
        struct ekObject *objectVal;
    };
} ekValue;

ekValue *ekValueCreate(struct ekContext *Y);
void ekValueDestroy(struct ekContext *Y, ekValue *p);

void ekValueAddRef(struct ekContext *Y, ekValue *p);
void ekValueRemoveRef(struct ekContext *Y, ekValue *p);

void ekValueClear(struct ekContext *Y, ekValue *p);

void ekValueCloneData(struct ekContext *Y, ekValue *dst, ekValue *src);
ekValue *ekValueClone(struct ekContext *Y, ekValue *p);

ekValue *ekValueCreateInt(struct ekContext *Y, int v);
ekValue *ekValueCreateFloat(struct ekContext *Y, yF32 v);
ekValue *ekValueCreateKString(struct ekContext *Y, const char *s);
ekValue *ekValueCreateString(struct ekContext *Y, const char *s);
ekValue *ekValueDonateString(struct ekContext *Y, char *s);  // grants ownership to the char*
ekValue *ekValueCreateFunction(struct ekContext *Y, struct ekBlock *block);
ekValue *ekValueCreateCFunction(struct ekContext *Y, ekCFunction func);
ekValue *ekValueCreateRef(struct ekContext *Y, struct ekValue **ref);
ekValue *ekValueCreateArray(struct ekContext *Y);
ekValue *ekValueCreateObject(struct ekContext *Y, struct ekValue *isa, int argCount, yBool firstArgIsa);

// Special calls for arrays
void ekValueArrayPush(struct ekContext *Y, ekValue *p, ekValue *v);

// Special calls for refs
yBool ekValueSetRefVal(struct ekContext *Y, ekValue *ref, ekValue *p);

// Special calls for objects
void ekValueObjectSetMember(struct ekContext *Y, struct ekValue *object, const char *name, struct ekValue *value);
yBool ekValueTestInherits(struct ekContext *Y, ekValue *child, ekValue *parent);

// Special calls for functions
void ekValueAddClosureVars(struct ekContext *Y, ekValue *p);

ekValue *ekValueAdd(struct ekContext *Y, ekValue *a, ekValue *b);
ekValue *ekValueSub(struct ekContext *Y, ekValue *a, ekValue *b);
ekValue *ekValueMul(struct ekContext *Y, ekValue *a, ekValue *b);
ekValue *ekValueDiv(struct ekContext *Y, ekValue *a, ekValue *b);

yS32 ekValueCmp(struct ekContext *Y, ekValue *a, ekValue *b);

// These assume that you are using the pattern: v = ekValueTo*(Y, v)
// and auto-unref the passed-in value for you.
ekValue *ekValueToBool(struct ekContext *Y, ekValue *p);
ekValue *ekValueToInt(struct ekContext *Y, ekValue *p);
ekValue *ekValueToFloat(struct ekContext *Y, ekValue *p);
ekValue *ekValueToString(struct ekContext *Y, ekValue *p);

ekValue *ekValueStringFormat(struct ekContext *Y, ekValue *format, yS32 argCount);

ekValue *ekValueIndex(struct ekContext *Y, ekValue *p, ekValue *index, yBool lvalue);

const char *ekValueTypeName(struct ekContext *Y, int type); // used in error reporting

#ifdef EUREKA_TRACE_REFS
void ekValueTraceRefs(struct ekContext *Y, struct ekValue *p, int delta, const char *note);
void ekValueRemoveRefHashed(struct ekContext *Y, struct ekValue *p); // used for tracing cleanup of hashes of values
void ekValueRemoveRefArray(struct ekContext *Y, struct ekValue *p);  // used for tracing cleanup of arrays of values
#else
#define ekValueTraceRefs(A, B, C, D)
#define ekValueRemoveRefHashed ekValueRemoveRef
#define ekValueRemoveRefArray ekValueRemoveRef
#endif
#define ekValueAddRefNote(Y, V, N) { ekValueTraceRefs(Y, V, 1, N); ekValueAddRef(Y, V); }
#define ekValueRemoveRefNote(Y, V, N) { ekValueTraceRefs(Y, V, -1, N); ekValueRemoveRef(Y, V); }

void ekValueDump(struct ekContext *Y, ekDumpParams *params, ekValue *p);

#define ekValueIsCallable(VAL)     \
    (  (VAL->type == YVT_OBJECT)    \
       || (VAL->type == YVT_BLOCK)     \
       || (VAL->type == YVT_CFUNCTION))

// ---------------------------------------------------------------------------
// Globals

extern ekValue ekValueNull;
extern ekValue *ekValueNullPtr;

// ---------------------------------------------------------------------------

#endif
