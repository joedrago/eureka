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

ekDumpParams *ekDumpParamsCreate(struct ekContext *E);
void ekDumpParamsDestroy(struct ekContext *E, ekDumpParams *params);

#define YVT_MAXNAMELEN 15

struct ekValueType;
typedef void (*ekValueTypeDestroyUserData)(struct ekContext *E, struct ekValueType *valueType);

typedef void (*ekValueTypeFuncClear)(struct ekContext *E, struct ekValue *p);
typedef void (*ekValueTypeFuncClone)(struct ekContext *E, struct ekValue *dst, struct ekValue *src);
typedef ekBool(*ekValueTypeFuncToBool)(struct ekContext *E, struct ekValue *p);
typedef ekS32(*ekValueTypeFuncToInt)(struct ekContext *E, struct ekValue *p);
typedef ekF32(*ekValueTypeFuncToFloat)(struct ekContext *E, struct ekValue *p);
typedef struct ekValue *(*ekValueTypeFuncToString)(struct ekContext *E, struct ekValue *p);
typedef struct ekValue *(*ekValueTypeFuncArithmetic)(struct ekContext *E, struct ekValue *a, struct ekValue *b, ekValueArithmeticOp op);
typedef ekBool(*ekValueTypeFuncCmp)(struct ekContext *E, struct ekValue *a, struct ekValue *b, int *cmpResult);
typedef struct ekValue *(*ekValueTypeFuncIndex)(struct ekContext *E, struct ekValue *p, struct ekValue *index, ekBool lvalue);
typedef void (*ekValueTypeFuncDump)(struct ekContext *E, ekDumpParams *params, struct ekValue *p); // creates debug text representing value, caller responsible for ekFree()

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

ekValueType *ekValueTypeCreate(struct ekContext *E, const char *name);
void ekValueTypeDestroy(struct ekContext *E, ekValueType *type);
int ekValueTypeRegister(struct ekContext *E, ekValueType *newType); // takes ownership of newType (use ekAlloc), returns new type id

void ekValueTypeRegisterAllBasicTypes(struct ekContext *E);

#define ekValueTypePtr(id) ((ekValueType*)E->types[id])

// If the function ptr doesn't exist, just return 0 (NULL) safely, otherwise call it with arguments after the macro
#define ekValueTypeSafeCall(id, funcName) \
    (((ekValueType*)E->types[id])->func ## funcName == ekValueTypeFuncNotUsed) ? 0 \
    : ((ekValueType*)E->types[id])->func ## funcName

// ---------------------------------------------------------------------------

// Should return how many values it is returning on the stack
typedef ekU32(ekCFunction)(struct ekContext *E, ekU32 argCount);

// ---------------------------------------------------------------------------

typedef struct ekValue
{
    ekU8 type;
    ekS32 refs;                            // reference count!
    union
    {
        ekS32 intVal;
        ekF32 floatVal;
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

ekValue *ekValueCreate(struct ekContext *E);
void ekValueDestroy(struct ekContext *E, ekValue *p);

void ekValueAddRef(struct ekContext *E, ekValue *p);
void ekValueRemoveRef(struct ekContext *E, ekValue *p);

void ekValueClear(struct ekContext *E, ekValue *p);

void ekValueCloneData(struct ekContext *E, ekValue *dst, ekValue *src);
ekValue *ekValueClone(struct ekContext *E, ekValue *p);

ekValue *ekValueCreateInt(struct ekContext *E, int v);
ekValue *ekValueCreateFloat(struct ekContext *E, ekF32 v);
ekValue *ekValueCreateKString(struct ekContext *E, const char *s);
ekValue *ekValueCreateString(struct ekContext *E, const char *s);
ekValue *ekValueDonateString(struct ekContext *E, char *s);  // grants ownership to the char*
ekValue *ekValueCreateFunction(struct ekContext *E, struct ekBlock *block);
ekValue *ekValueCreateCFunction(struct ekContext *E, ekCFunction func);
ekValue *ekValueCreateRef(struct ekContext *E, struct ekValue **ref);
ekValue *ekValueCreateArray(struct ekContext *E);
ekValue *ekValueCreateObject(struct ekContext *E, struct ekValue *isa, int argCount, ekBool firstArgIsa);

// Special calls for arrays
void ekValueArrayPush(struct ekContext *E, ekValue *p, ekValue *v);

// Special calls for refs
ekBool ekValueSetRefVal(struct ekContext *E, ekValue *ref, ekValue *p);

// Special calls for objects
void ekValueObjectSetMember(struct ekContext *E, struct ekValue *object, const char *name, struct ekValue *value);
ekBool ekValueTestInherits(struct ekContext *E, ekValue *child, ekValue *parent);

// Special calls for functions
void ekValueAddClosureVars(struct ekContext *E, ekValue *p);

ekValue *ekValueAdd(struct ekContext *E, ekValue *a, ekValue *b);
ekValue *ekValueSub(struct ekContext *E, ekValue *a, ekValue *b);
ekValue *ekValueMul(struct ekContext *E, ekValue *a, ekValue *b);
ekValue *ekValueDiv(struct ekContext *E, ekValue *a, ekValue *b);

ekS32 ekValueCmp(struct ekContext *E, ekValue *a, ekValue *b);

// These assume that you are using the pattern: v = ekValueTo*(E, v)
// and auto-unref the passed-in value for you.
ekValue *ekValueToBool(struct ekContext *E, ekValue *p);
ekValue *ekValueToInt(struct ekContext *E, ekValue *p);
ekValue *ekValueToFloat(struct ekContext *E, ekValue *p);
ekValue *ekValueToString(struct ekContext *E, ekValue *p);

ekValue *ekValueStringFormat(struct ekContext *E, ekValue *format, ekS32 argCount);

ekValue *ekValueIndex(struct ekContext *E, ekValue *p, ekValue *index, ekBool lvalue);

const char *ekValueTypeName(struct ekContext *E, int type); // used in error reporting

#ifdef EUREKA_TRACE_REFS
void ekValueTraceRefs(struct ekContext *E, struct ekValue *p, int delta, const char *note);
void ekValueRemoveRefHashed(struct ekContext *E, struct ekValue *p); // used for tracing cleanup of hashes of values
void ekValueRemoveRefArray(struct ekContext *E, struct ekValue *p);  // used for tracing cleanup of arrays of values
#else
#define ekValueTraceRefs(A, B, C, D)
#define ekValueRemoveRefHashed ekValueRemoveRef
#define ekValueRemoveRefArray ekValueRemoveRef
#endif
#define ekValueAddRefNote(E, V, N) { ekValueTraceRefs(E, V, 1, N); ekValueAddRef(E, V); }
#define ekValueRemoveRefNote(E, V, N) { ekValueTraceRefs(E, V, -1, N); ekValueRemoveRef(E, V); }

void ekValueDump(struct ekContext *E, ekDumpParams *params, ekValue *p);

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
