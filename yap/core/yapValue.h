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
struct yapVM;

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
    struct yapVM *vm;
    yapString output;  // The final output value. Dump functions should use concatenate functions on this string, not set
    yapString tempStr; // to be used as a temporary string by the dump function, and should be considered to be changed if recursively calling dump()
    int tempInt;       // to be used as a temporary int by the dump function, and should be considered to be changed if recursively calling dump()
} yapDumpParams;

yapDumpParams *yapDumpParamsCreate(struct yapVM *vm);
void yapDumpParamsDestroy(yapDumpParams *params);

#define YVT_MAXNAMELEN 15

struct yapValueType;
typedef void (*yapValueTypeDestroyUserData)(struct yapValueType *valueType);

typedef void (*yapValueTypeFuncClear)(struct yapValue *p);
typedef void (*yapValueTypeFuncClone)(struct yapVM *vm, struct yapValue *dst, struct yapValue *src);
typedef void (*yapValueTypeFuncMark)(struct yapVM *vm, struct yapValue *value);
typedef yBool (*yapValueTypeFuncToBool)(struct yapValue *p);
typedef yS32 (*yapValueTypeFuncToInt)(struct yapValue *p);
typedef yF32 (*yapValueTypeFuncToFloat)(struct yapValue *p);
typedef struct yapValue * (*yapValueTypeFuncToString)(struct yapVM *vm, struct yapValue *p);
typedef struct yapValue * (*yapValueTypeFuncArithmetic)(struct yapVM *vm, struct yapValue *a, struct yapValue *b, yapValueArithmeticOp op);
typedef yBool (*yapValueTypeFuncCmp)(struct yapVM *vm, struct yapValue *a, struct yapValue *b, int *cmpResult);
typedef struct yapValue * (*yapValueTypeFuncIndex)(struct yapVM *vm, struct yapValue *p, struct yapValue *index, yBool lvalue);
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
int yapValueTypeRegister(struct yapVM *vm, yapValueType *newType); // takes ownership of newType (use yapAlloc), returns new type id

void yapValueTypeRegisterAllBasicTypes(struct yapVM *vm);

#define yapValueTypePtr(id) ((yapValueType*)vm->types.data[id])

// If the function ptr doesn't exist, just return 0 (NULL) safely, otherwise call it with arguments after the macro
#define yapValueTypeSafeCall(id, funcName) \
    (((yapValueType*)vm->types.data[id])->func ## funcName == yapValueTypeFuncNotUsed) ? 0 \
   : ((yapValueType*)vm->types.data[id])->func ## funcName


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
typedef yU32(yapCFunction)(struct yapVM *vm, yU32 argCount);

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
yapValue *yapValueAcquire(struct yapVM *vm);
void yapValueRelease(struct yapVM *vm, yapValue *p); // returns to free pool

void yapValueMark(struct yapVM *vm, yapValue *value); // used by yapVMGC()
void yapValueDestroy(struct yapVM *vm, yapValue *p);  // only yapVMDestroy() should -ever- call this

void yapValueClear(struct yapVM *vm, yapValue *p);

void yapValueCloneData(struct yapVM *vm, yapValue *dst, yapValue *src);
yapValue *yapValueClone(struct yapVM *vm, yapValue *p);
yapValue *yapValuePersonalize(struct yapVM *vm, yapValue *p);  // only clones if used

yapValue *yapValueSetInt(struct yapVM *vm, yapValue *p, int v);
yapValue *yapValueSetFloat(struct yapVM *vm, yapValue *p, yF32 v);
yapValue *yapValueSetKString(struct yapVM *vm, yapValue *p, const char *s);
yapValue *yapValueSetString(struct yapVM *vm, yapValue *p, const char *s);
yapValue *yapValueDonateString(struct yapVM *vm, yapValue *p, char *s);  // grants ownership to the char*
yapValue *yapValueSetFunction(struct yapVM *vm, yapValue *p, struct yapBlock *block);
yapValue *yapValueSetCFunction(struct yapVM *vm, yapValue *p, yapCFunction func);
yapValue *yapValueSetRef(struct yapVM *vm, yapValue *p, struct yapValue **ref);

yBool yapValueSetRefVal(struct yapVM *vm, yapValue *ref, yapValue *p);

void yapValueAddClosureVars(struct yapVM *vm, yapValue *p);

yapValue *yapValueArrayCreate(struct yapVM *vm);
void yapValueArrayPush(struct yapVM *vm, yapValue *p, yapValue *v);

yapValue *yapValueObjectCreate(struct yapVM *vm, struct yapValue *isa, int argCount);
void yapValueObjectSetMember(struct yapVM *vm, struct yapValue *object, const char *name, struct yapValue *value);

yapValue *yapValueAdd(struct yapVM *vm, yapValue *a, yapValue *b);
yapValue *yapValueSub(struct yapVM *vm, yapValue *a, yapValue *b);
yapValue *yapValueMul(struct yapVM *vm, yapValue *a, yapValue *b);
yapValue *yapValueDiv(struct yapVM *vm, yapValue *a, yapValue *b);

yS32 yapValueCmp(struct yapVM *vm, yapValue *a, yapValue *b);

yapValue *yapValueToBool(struct yapVM *vm, yapValue *p);
yapValue *yapValueToInt(struct yapVM *vm, yapValue *p);
yapValue *yapValueToFloat(struct yapVM *vm, yapValue *p);
yapValue *yapValueToString(struct yapVM *vm, yapValue *p);

yapValue *yapValueStringFormat(struct yapVM *vm, yapValue *format, yS32 argCount);

yapValue *yapValueIndex(struct yapVM *vm, yapValue *p, yapValue *index, yBool lvalue);

const char *yapValueTypeName(struct yapVM *vm, int type); // used in error reporting

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
