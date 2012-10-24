// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPVALUE_H
#define YAPVALUE_H

#include "yapString.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapMap;
struct yapObject;
struct yapValue;

// ---------------------------------------------------------------------------

typedef enum yapValueBasicType
{
    YVT_NULL,                          // Must stay zero, so that calloc/memset sets a yapValue to NULL

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
} yapValueBasicType;

typedef enum yapValueArithmeticOp
{
    YVAO_ADD = 0,
    YVAO_SUB,
    YVAO_MUL,
    YVAO_DIV,

    YVAO_COUNT
} yapValueArithmeticOp;

typedef struct yapDumpParams
{
    yapString output;  // The final output value. Dump functions should use concatenate functions on this string, not set
    yapString tempStr; // to be used as a temporary string by the dump function, and should be considered to be changed if recursively calling dump()
    int tempInt;       // to be used as a temporary int by the dump function, and should be considered to be changed if recursively calling dump()
} yapDumpParams;

yapDumpParams *yapDumpParamsCreate(struct yapContext *Y);
void yapDumpParamsDestroy(struct yapContext *Y, yapDumpParams *params);

#define YVT_MAXNAMELEN 15

struct yapValueType;
typedef void (*yapValueTypeDestroyUserData)(struct yapContext *Y, struct yapValueType *valueType);

typedef void (*yapValueTypeFuncClear)(struct yapContext *Y, struct yapValue *p);
typedef void (*yapValueTypeFuncClone)(struct yapContext *Y, struct yapValue *dst, struct yapValue *src);
typedef yBool(*yapValueTypeFuncToBool)(struct yapContext *Y, struct yapValue *p);
typedef yS32(*yapValueTypeFuncToInt)(struct yapContext *Y, struct yapValue *p);
typedef yF32(*yapValueTypeFuncToFloat)(struct yapContext *Y, struct yapValue *p);
typedef struct yapValue *(*yapValueTypeFuncToString)(struct yapContext *Y, struct yapValue *p);
typedef struct yapValue *(*yapValueTypeFuncArithmetic)(struct yapContext *Y, struct yapValue *a, struct yapValue *b, yapValueArithmeticOp op);
typedef yBool(*yapValueTypeFuncCmp)(struct yapContext *Y, struct yapValue *a, struct yapValue *b, int *cmpResult);
typedef struct yapValue *(*yapValueTypeFuncIndex)(struct yapContext *Y, struct yapValue *p, struct yapValue *index, yBool lvalue);
typedef void (*yapValueTypeFuncDump)(struct yapContext *Y, yapDumpParams *params, struct yapValue *p); // creates debug text representing value, caller responsible for yapFree()

// This is used to enforce the setting of every function ptr in a yapValueType*; an explicit alternative to NULL
#define yapValueTypeFuncNotUsed ((void*)-1)

typedef struct yapValueType
{
    int id;
    char name[YVT_MAXNAMELEN + 1];

    void *userData;                                  // per-type global structure used to hold any static data a type needs
    yapValueTypeDestroyUserData funcDestroyUserData; // optional destructor for userdata

    yapValueTypeFuncClear funcClear;
    yapValueTypeFuncClone funcClone;
    yapValueTypeFuncToBool funcToBool;
    yapValueTypeFuncToInt funcToInt;
    yapValueTypeFuncToFloat funcToFloat;
    yapValueTypeFuncToString funcToString;
    yapValueTypeFuncArithmetic funcArithmetic;
    yapValueTypeFuncCmp funcCmp;
    yapValueTypeFuncIndex funcIndex;
    yapValueTypeFuncDump funcDump;
} yapValueType;

yapValueType *yapValueTypeCreate(struct yapContext *Y, const char *name);
void yapValueTypeDestroy(struct yapContext *Y, yapValueType *type);
int yapValueTypeRegister(struct yapContext *Y, yapValueType *newType); // takes ownership of newType (use yapAlloc), returns new type id

void yapValueTypeRegisterAllBasicTypes(struct yapContext *Y);

#define yapValueTypePtr(id) ((yapValueType*)Y->types[id])

// If the function ptr doesn't exist, just return 0 (NULL) safely, otherwise call it with arguments after the macro
#define yapValueTypeSafeCall(id, funcName) \
    (((yapValueType*)Y->types[id])->func ## funcName == yapValueTypeFuncNotUsed) ? 0 \
    : ((yapValueType*)Y->types[id])->func ## funcName

// ---------------------------------------------------------------------------

// Should return how many values it is returning on the stack
typedef yU32(yapCFunction)(struct yapContext *Y, yU32 argCount);

// ---------------------------------------------------------------------------

typedef struct yapValue
{
    yU8 type;
    yS32 refs;                            // reference count!
    union
    {
        yS32 intVal;
        yF32 floatVal;
        struct
        {
            struct yapMap *closureVars;   // Populated at runtime when a reference to a new function() is created
            struct yapBlock *blockVal;    // Hurr, Shield Slam
        };
        yapString stringVal;
        struct yapValue **refVal;
        yapCFunction *cFuncVal;
        struct yapValue **arrayVal;
        struct yapObject *objectVal;
    };
} yapValue;

yapValue *yapValueCreate(struct yapContext *Y);
void yapValueDestroy(struct yapContext *Y, yapValue *p);

void yapValueAddRef(struct yapContext *Y, yapValue *p);
void yapValueRemoveRef(struct yapContext *Y, yapValue *p);

void yapValueClear(struct yapContext *Y, yapValue *p);

void yapValueCloneData(struct yapContext *Y, yapValue *dst, yapValue *src);
yapValue *yapValueClone(struct yapContext *Y, yapValue *p);

yapValue *yapValueCreateInt(struct yapContext *Y, int v);
yapValue *yapValueCreateFloat(struct yapContext *Y, yF32 v);
yapValue *yapValueCreateKString(struct yapContext *Y, const char *s);
yapValue *yapValueCreateString(struct yapContext *Y, const char *s);
yapValue *yapValueDonateString(struct yapContext *Y, char *s);  // grants ownership to the char*
yapValue *yapValueCreateFunction(struct yapContext *Y, struct yapBlock *block);
yapValue *yapValueCreateCFunction(struct yapContext *Y, yapCFunction func);
yapValue *yapValueCreateRef(struct yapContext *Y, struct yapValue **ref);
yapValue *yapValueCreateArray(struct yapContext *Y);
yapValue *yapValueCreateObject(struct yapContext *Y, struct yapValue *isa, int argCount, yBool firstArgIsa);

// Special calls for arrays
void yapValueArrayPush(struct yapContext *Y, yapValue *p, yapValue *v);

// Special calls for refs
yBool yapValueSetRefVal(struct yapContext *Y, yapValue *ref, yapValue *p);

// Special calls for objects
void yapValueObjectSetMember(struct yapContext *Y, struct yapValue *object, const char *name, struct yapValue *value);
yBool yapValueTestInherits(struct yapContext *Y, yapValue *child, yapValue *parent);

// Special calls for functions
void yapValueAddClosureVars(struct yapContext *Y, yapValue *p);

yapValue *yapValueAdd(struct yapContext *Y, yapValue *a, yapValue *b);
yapValue *yapValueSub(struct yapContext *Y, yapValue *a, yapValue *b);
yapValue *yapValueMul(struct yapContext *Y, yapValue *a, yapValue *b);
yapValue *yapValueDiv(struct yapContext *Y, yapValue *a, yapValue *b);

yS32 yapValueCmp(struct yapContext *Y, yapValue *a, yapValue *b);

// These assume that you are using the pattern: v = yapValueTo*(Y, v)
// and auto-unref the passed-in value for you.
yapValue *yapValueToBool(struct yapContext *Y, yapValue *p);
yapValue *yapValueToInt(struct yapContext *Y, yapValue *p);
yapValue *yapValueToFloat(struct yapContext *Y, yapValue *p);
yapValue *yapValueToString(struct yapContext *Y, yapValue *p);

yapValue *yapValueStringFormat(struct yapContext *Y, yapValue *format, yS32 argCount);

yapValue *yapValueIndex(struct yapContext *Y, yapValue *p, yapValue *index, yBool lvalue);

const char *yapValueTypeName(struct yapContext *Y, int type); // used in error reporting

#ifdef YAP_TRACE_REFS
void yapValueTraceRefs(struct yapContext *Y, struct yapValue *p, int delta, const char *note);
void yapValueRemoveRefHashed(struct yapContext *Y, struct yapValue *p); // used for tracing cleanup of hashes of values
void yapValueRemoveRefArray(struct yapContext *Y, struct yapValue *p);  // used for tracing cleanup of arrays of values
#else
#define yapValueTraceRefs(A, B, C, D)
#define yapValueRemoveRefHashed yapValueRemoveRef
#define yapValueRemoveRefArray yapValueRemoveRef
#endif
#define yapValueAddRefNote(Y, V, N) { yapValueTraceRefs(Y, V, 1, N); yapValueAddRef(Y, V); }
#define yapValueRemoveRefNote(Y, V, N) { yapValueTraceRefs(Y, V, -1, N); yapValueRemoveRef(Y, V); }

void yapValueDump(struct yapContext *Y, yapDumpParams *params, yapValue *p);

#define yapValueIsCallable(VAL)     \
    (  (VAL->type == YVT_OBJECT)    \
       || (VAL->type == YVT_BLOCK)     \
       || (VAL->type == YVT_CFUNCTION))

// ---------------------------------------------------------------------------
// Globals

extern yapValue yapValueNull;
extern yapValue *yapValueNullPtr;

// ---------------------------------------------------------------------------

#endif
