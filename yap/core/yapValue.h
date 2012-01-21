// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPVALUE_H
#define YAPVALUE_H

#include "yapArray.h"
#include "yapString.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapObject;
struct yapValue;
struct yapContext;

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

    YVT_REF,                           // Variable reference

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
    struct yapContext *Y;
    yapString output;  // The final output value. Dump functions should use concatenate functions on this string, not set
    yapString tempStr; // to be used as a temporary string by the dump function, and should be considered to be changed if recursively calling dump()
    int tempInt;       // to be used as a temporary int by the dump function, and should be considered to be changed if recursively calling dump()
} yapDumpParams;

yapDumpParams *yapDumpParamsCreate(struct yapContext *Y);
void yapDumpParamsDestroy(yapDumpParams *params);

#define YVT_MAXNAMELEN 15

struct yapValueType;
typedef void (*yapValueTypeDestroyUserData)(struct yapValueType *valueType);

typedef void (*yapValueTypeFuncClear)(struct yapValue *p);
typedef void (*yapValueTypeFuncClone)(struct yapContext *Y, struct yapValue *dst, struct yapValue *src);
typedef void (*yapValueTypeFuncMark)(struct yapContext *Y, struct yapValue *value);
typedef yBool (*yapValueTypeFuncToBool)(struct yapValue *p);
typedef yS32 (*yapValueTypeFuncToInt)(struct yapValue *p);
typedef yF32 (*yapValueTypeFuncToFloat)(struct yapValue *p);
typedef struct yapValue * (*yapValueTypeFuncToString)(struct yapContext *Y, struct yapValue *p);
typedef struct yapValue * (*yapValueTypeFuncArithmetic)(struct yapContext *Y, struct yapValue *a, struct yapValue *b, yapValueArithmeticOp op);
typedef yBool (*yapValueTypeFuncCmp)(struct yapContext *Y, struct yapValue *a, struct yapValue *b, int *cmpResult);
typedef struct yapValue * (*yapValueTypeFuncIndex)(struct yapContext *Y, struct yapValue *p, struct yapValue *index, yBool lvalue);
typedef void (*yapValueTypeFuncDump)(yapDumpParams *params, struct yapValue *p); // creates debug text representing value, caller responsible for yapFree()

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
    yapValueTypeFuncMark funcMark;
    yapValueTypeFuncToBool funcToBool;
    yapValueTypeFuncToInt funcToInt;
    yapValueTypeFuncToFloat funcToFloat;
    yapValueTypeFuncToString funcToString;
    yapValueTypeFuncArithmetic funcArithmetic;
    yapValueTypeFuncCmp funcCmp;
    yapValueTypeFuncIndex funcIndex;
    yapValueTypeFuncDump funcDump;
} yapValueType;

yapValueType *yapValueTypeCreate(const char *name);
void yapValueTypeDestroy(yapValueType *type);
int yapValueTypeRegister(struct yapContext *Y, yapValueType *newType); // takes ownership of newType (use yapAlloc), returns new type id

void yapValueTypeRegisterAllBasicTypes(struct yapContext *Y);

#define yapValueTypePtr(id) ((yapValueType*)Y->types.data[id])

// If the function ptr doesn't exist, just return 0 (NULL) safely, otherwise call it with arguments after the macro
#define yapValueTypeSafeCall(id, funcName) \
    (((yapValueType*)Y->types.data[id])->func ## funcName == yapValueTypeFuncNotUsed) ? 0 \
   : ((yapValueType*)Y->types.data[id])->func ## funcName


// ---------------------------------------------------------------------------

typedef struct yapClosureVariable
{
    char *name;
    struct yapVariable *variable;
} yapClosureVariable;

yapClosureVariable *yapClosureVariableCreate(const char *name, struct yapVariable *variable);
void yapClosureVariableDestroy(yapClosureVariable *cv);

// ---------------------------------------------------------------------------

// Should return how many values it is returning on the stack
typedef yU32(yapCFunction)(struct yapContext *Y, yU32 argCount);

// ---------------------------------------------------------------------------

typedef struct yapValue
{
    yU8 type;
    yFlag constant: 1;                 // Pointing at a constant table, do not free
    yFlag used: 1;                     // The "mark" during the GC's mark-and-sweep
    union
    {
        yS32 intVal;
        yF32 floatVal;
        struct {
            yapArray *closureVars;         // Populated at runtime when a reference to a new function() is created
            struct yapBlock *blockVal;     // Hurr, Shield Slam
        };
        yapString stringVal;
        struct yapValue **refVal;
        yapCFunction *cFuncVal;
        yapArray *arrayVal;
        struct yapObject *objectVal;
    };
} yapValue;

// yapValueCreate() is only ever called via yapValueAcquire
yapValue *yapValueAcquire(struct yapContext *Y);
void yapValueRelease(struct yapContext *Y, yapValue *p); // returns to free pool

void yapValueMark(struct yapContext *Y, yapValue *value); // used by yapContextGC()
void yapValueDestroy(struct yapContext *Y, yapValue *p);  // only yapContextDestroy() should -ever- call this

void yapValueClear(struct yapContext *Y, yapValue *p);

void yapValueCloneData(struct yapContext *Y, yapValue *dst, yapValue *src);
yapValue *yapValueClone(struct yapContext *Y, yapValue *p);
yapValue *yapValuePersonalize(struct yapContext *Y, yapValue *p);  // only clones if used

yapValue *yapValueSetInt(struct yapContext *Y, yapValue *p, int v);
yapValue *yapValueSetFloat(struct yapContext *Y, yapValue *p, yF32 v);
yapValue *yapValueSetKString(struct yapContext *Y, yapValue *p, const char *s);
yapValue *yapValueSetString(struct yapContext *Y, yapValue *p, const char *s);
yapValue *yapValueDonateString(struct yapContext *Y, yapValue *p, char *s);  // grants ownership to the char*
yapValue *yapValueSetFunction(struct yapContext *Y, yapValue *p, struct yapBlock *block);
yapValue *yapValueSetCFunction(struct yapContext *Y, yapValue *p, yapCFunction func);
yapValue *yapValueSetRef(struct yapContext *Y, yapValue *p, struct yapValue **ref);

yBool yapValueSetRefVal(struct yapContext *Y, yapValue *ref, yapValue *p);

void yapValueAddClosureVars(struct yapContext *Y, yapValue *p);

yapValue *yapValueArrayCreate(struct yapContext *Y);
void yapValueArrayPush(struct yapContext *Y, yapValue *p, yapValue *v);

yapValue *yapValueObjectCreate(struct yapContext *Y, struct yapValue *isa, int argCount);
void yapValueObjectSetMember(struct yapContext *Y, struct yapValue *object, const char *name, struct yapValue *value);

yapValue *yapValueAdd(struct yapContext *Y, yapValue *a, yapValue *b);
yapValue *yapValueSub(struct yapContext *Y, yapValue *a, yapValue *b);
yapValue *yapValueMul(struct yapContext *Y, yapValue *a, yapValue *b);
yapValue *yapValueDiv(struct yapContext *Y, yapValue *a, yapValue *b);

yS32 yapValueCmp(struct yapContext *Y, yapValue *a, yapValue *b);

yapValue *yapValueToBool(struct yapContext *Y, yapValue *p);
yapValue *yapValueToInt(struct yapContext *Y, yapValue *p);
yapValue *yapValueToFloat(struct yapContext *Y, yapValue *p);
yapValue *yapValueToString(struct yapContext *Y, yapValue *p);

yapValue *yapValueStringFormat(struct yapContext *Y, yapValue *format, yS32 argCount);

yapValue *yapValueIndex(struct yapContext *Y, yapValue *p, yapValue *index, yBool lvalue);

const char *yapValueTypeName(struct yapContext *Y, int type); // used in error reporting

void yapValueDump(yapDumpParams *params, yapValue *p);

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
