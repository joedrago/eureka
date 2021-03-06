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
typedef ekU32 (ekCFunction)(struct ekContext * E, ekU32 argCount);

// ---------------------------------------------------------------------------

enum
{
    EVT_NULL, // Must stay zero, so that calloc/memset sets a ekValue to NULL

    EVT_BLOCK,
    EVT_CFUNCTION,

    EVT_INT,
    EVT_FLOAT,
    EVT_BOOL,
    EVT_STRING,

    EVT_ARRAY,
    EVT_OBJECT,

    EVT_REF, // Value reference

    EVT_COUNT,
    EVT_CUSTOM = EVT_COUNT, // First custom value type
    EVT_INVALID = -1
};

typedef enum ekValueArithmeticOp
{
    EVAO_ADD = 0,
    EVAO_SUB,
    EVAO_MUL,
    EVAO_DIV
} ekValueArithmeticOp;

// ---------------------------------------------------------------------------

typedef struct ekValue
{
    ekU8 type;
    ekBool used;
    union {
        ekS32 boolVal;
        ekS32 intVal;
        ekF32 floatVal;
        struct
        {
            struct ekMap * closureVars; // Populated at runtime when a reference to a new function() is created
            union {
                struct ekBlock * blockVal; // Hurr, Shield Slam
                ekCFunction * cFuncVal;
            };
        };
        ekString stringVal;
        struct ekValue ** refVal;
        struct ekValue ** arrayVal;
        struct ekObject * objectVal;
        void * ptrVal; // Raw ptr used when creating custom ekValueTypes
    };
} ekValue;

ekValue * ekValueCreate(struct ekContext * E, int type);
void ekValueDestroy(struct ekContext * E, ekValue * p);

void ekValueMark(struct ekContext * E, ekValue * p);

void ekValueClear(struct ekContext * E, ekValue * p);

void ekValueCloneData(struct ekContext * E, ekValue * dst, ekValue * src);
ekValue * ekValueClone(struct ekContext * E, ekValue * p);

ekValue * ekValueCreateInt(struct ekContext * E, ekS32 v);
ekValue * ekValueCreateFloat(struct ekContext * E, ekF32 v);
ekValue * ekValueCreateBool(struct ekContext * E, ekBool v);
ekValue * ekValueCreateKString(struct ekContext * E, const char * s);
ekValue * ekValueCreateString(struct ekContext * E, const char * s);
ekValue * ekValueCreateStringLen(struct ekContext * E, const char * s, ekS32 len);
ekValue * ekValueDonateString(struct ekContext * E, char * s); // grants ownership to the char*
ekValue * ekValueCreateFunction(struct ekContext * E, struct ekBlock * block);
ekValue * ekValueCreateCFunction(struct ekContext * E, ekCFunction func);
ekValue * ekValueCreateRef(struct ekContext * E, struct ekValue ** ref);
ekValue * ekValueCreateArray(struct ekContext * E);
ekValue * ekValueCreateObject(struct ekContext * E, struct ekValue * prototype, ekS32 argCount, ekBool firstArgPrototype);

// Special calls for arrays
void ekValueArrayPush(struct ekContext * E, ekValue * p, ekValue * v);
void ekValueArrayClear(struct ekContext * E, ekValue * p);

// Special calls for refs
ekBool ekValueSetRefVal(struct ekContext * E, ekValue * ref, ekValue * p);

// Special calls for objects
void ekValueObjectSetMember(struct ekContext * E, struct ekValue * object, const char * name, struct ekValue * value);
ekBool ekValueTestInherits(struct ekContext * E, ekValue * child, ekValue * parent);

// Special calls for functions
void ekValueAddClosureVars(struct ekContext * E, ekValue * p);

ekValue * ekValueAdd(struct ekContext * E, ekValue * a, ekValue * b);
ekValue * ekValueSub(struct ekContext * E, ekValue * a, ekValue * b);
ekValue * ekValueMul(struct ekContext * E, ekValue * a, ekValue * b);
ekValue * ekValueDiv(struct ekContext * E, ekValue * a, ekValue * b);

ekS32 ekValueCmp(struct ekContext * E, ekValue * a, ekValue * b);
ekS32 ekValueLength(struct ekContext * E, ekValue * p);
const char * ekValueSafeStr(ekValue * stringValue);

// These assume that you are using the pattern: v = ekValueTo*(E, v)
// and auto-unref the passed-in value for you.
ekValue * ekValueToBool(struct ekContext * E, ekValue * p);
ekValue * ekValueToInt(struct ekContext * E, ekValue * p);
ekValue * ekValueToFloat(struct ekContext * E, ekValue * p);
ekValue * ekValueToString(struct ekContext * E, ekValue * p);
ekValue * ekValueReverse(struct ekContext * E, ekValue * p);

ekCFunction * ekValueIter(struct ekContext * E, ekValue * p);
ekValue * ekValueStringFormat(struct ekContext * E, ekValue * format, ekS32 argCount);
ekValue * ekValueIndex(struct ekContext * E, ekValue * p, ekValue * index, ekBool lvalue);
const char * ekValueTypeName(struct ekContext * E, ekS32 type); // used in error reporting

typedef struct ekDumpParams
{
    ekString output;  // The final output value. Dump functions should use concatenate functions on this string, not set
    ekString tempStr; // to be used as a temporary string by the dump function, and should be considered to be changed if recursively calling dump()
    ekS32 tempInt;    // to be used as a temporary ekS32 by the dump function, and should be considered to be changed if recursively calling dump()
} ekDumpParams;

ekDumpParams * ekDumpParamsCreate(struct ekContext * E);
void ekDumpParamsDestroy(struct ekContext * E, ekDumpParams * params);

void ekValueDump(struct ekContext * E, ekDumpParams * params, ekValue * p);

// TODO: make this a bool on ekValueType?
#define ekValueIsCallable(VAL)     \
    (  (VAL->type == EVT_OBJECT)   \
       || (VAL->type == EVT_BLOCK) \
       || (VAL->type == EVT_CFUNCTION))

// ---------------------------------------------------------------------------
// Globals

extern ekValue ekValueNull;
extern ekValue * ekValueNullPtr;

// ---------------------------------------------------------------------------

#endif // ifndef EKVALUE_H
