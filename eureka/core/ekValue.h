// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKVALUE_H
#define EKVALUE_H

#include "ekString.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekMap;
struct ekObject;
struct ekValue;

// ---------------------------------------------------------------------------

// Should return how many values it is returning on the stack
typedef ekU32(ekCFunction)(struct ekContext *E, ekU32 argCount);

// ---------------------------------------------------------------------------

enum
{
    EVT_NULL,                          // Must stay zero, so that calloc/memset sets a ekValue to NULL

    EVT_BLOCK,
    EVT_CFUNCTION,

    EVT_INT,
    EVT_FLOAT,
    EVT_STRING,

    EVT_ARRAY,
    EVT_OBJECT,

    EVT_REF,                           // Value reference

    EVT_COUNT,
    EVT_CUSTOM = EVT_COUNT,            // First custom value type
    EVT_INVALID = -1
};

typedef enum ekValueArithmeticOp
{
    EVAO_ADD = 0,
    EVAO_SUB,
    EVAO_MUL,
    EVAO_DIV,

    EVAO_COUNT
} ekValueArithmeticOp;

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
            union
            {
                struct ekBlock *blockVal;    // Hurr, Shield Slam
                ekCFunction *cFuncVal;
            };
        };
        ekString stringVal;
        struct ekValue **refVal;
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

ekValue *ekValueCreateInt(struct ekContext *E, ekS32 v);
ekValue *ekValueCreateFloat(struct ekContext *E, ekF32 v);
ekValue *ekValueCreateKString(struct ekContext *E, const char *s);
ekValue *ekValueCreateString(struct ekContext *E, const char *s);
ekValue *ekValueCreateStringLen(struct ekContext *E, const char *s, ekS32 len);
ekValue *ekValueDonateString(struct ekContext *E, char *s);  // grants ownership to the char*
ekValue *ekValueCreateFunction(struct ekContext *E, struct ekBlock *block);
ekValue *ekValueCreateCFunction(struct ekContext *E, ekCFunction func);
ekValue *ekValueCreateRef(struct ekContext *E, struct ekValue **ref);
ekValue *ekValueCreateArray(struct ekContext *E);
ekValue *ekValueCreateObject(struct ekContext *E, struct ekValue *isa, ekS32 argCount, ekBool firstArgIsa);

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
ekS32 ekValueLength(struct ekContext *E, ekValue *p);

// These assume that you are using the pattern: v = ekValueTo*(E, v)
// and auto-unref the passed-in value for you.
ekValue *ekValueToBool(struct ekContext *E, ekValue *p);
ekValue *ekValueToInt(struct ekContext *E, ekValue *p);
ekValue *ekValueToFloat(struct ekContext *E, ekValue *p);
ekValue *ekValueToString(struct ekContext *E, ekValue *p);
ekValue *ekValueReverse(struct ekContext *E, ekValue *p);

ekCFunction *ekValueIter(struct ekContext *E, ekValue *p);
ekValue *ekValueStringFormat(struct ekContext *E, ekValue *format, ekS32 argCount);
ekValue *ekValueIndex(struct ekContext *E, ekValue *p, ekValue *index, ekBool lvalue);
const char *ekValueTypeName(struct ekContext *E, ekS32 type); // used in error reporting

typedef struct ekDumpParams
{
    ekString output;  // The final output value. Dump functions should use concatenate functions on this string, not set
    ekString tempStr; // to be used as a temporary string by the dump function, and should be considered to be changed if recursively calling dump()
    ekS32 tempInt;       // to be used as a temporary ekS32 by the dump function, and should be considered to be changed if recursively calling dump()
} ekDumpParams;

ekDumpParams *ekDumpParamsCreate(struct ekContext *E);
void ekDumpParamsDestroy(struct ekContext *E, ekDumpParams *params);

#ifdef EUREKA_TRACE_REFS
void ekValueTraceRefs(struct ekContext *E, struct ekValue *p, ekS32 delta, const char *note);
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

// TODO: make this a bool on ekValueType?
#define ekValueIsCallable(VAL)     \
    (  (VAL->type == EVT_OBJECT)    \
       || (VAL->type == EVT_BLOCK)     \
       || (VAL->type == EVT_CFUNCTION))

// ---------------------------------------------------------------------------
// Globals

extern ekValue ekValueNull;
extern ekValue *ekValueNullPtr;

// ---------------------------------------------------------------------------

#endif
