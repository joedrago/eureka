// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKVALUETYPE_H
#define EKVALUETYPE_H

#include "ekValue.h"

#define EVT_MAXNAMELEN 15

struct ekValueType;
typedef void (*ekValueTypeDestroyUserData)(struct ekContext *E, struct ekValueType *valueType);

typedef void (*ekValueTypeFuncClear)(struct ekContext *E, struct ekValue *p);
typedef void (*ekValueTypeFuncClone)(struct ekContext *E, struct ekValue *dst, struct ekValue *src);
typedef ekBool(*ekValueTypeFuncToBool)(struct ekContext *E, struct ekValue *p);
typedef ekS32(*ekValueTypeFuncToInt)(struct ekContext *E, struct ekValue *p);
typedef ekF32(*ekValueTypeFuncToFloat)(struct ekContext *E, struct ekValue *p);
typedef struct ekValue *(*ekValueTypeFuncToString)(struct ekContext *E, struct ekValue *p);
typedef struct ekValue *(*ekValueTypeFuncReverse)(struct ekContext *E, struct ekValue *p);
typedef ekCFunction *(*ekValueTypeFuncIter)(struct ekContext *E, struct ekValue *p);
typedef struct ekValue *(*ekValueTypeFuncArithmetic)(struct ekContext *E, struct ekValue *a, struct ekValue *b, ekValueArithmeticOp op);
typedef ekBool(*ekValueTypeFuncCmp)(struct ekContext *E, struct ekValue *a, struct ekValue *b, ekS32 *cmpResult);
typedef ekS32(*ekValueTypeFuncLength)(struct ekContext *E, struct ekValue *p);
typedef struct ekValue *(*ekValueTypeFuncIndex)(struct ekContext *E, struct ekValue *p, struct ekValue *index, ekBool lvalue);
typedef void (*ekValueTypeFuncDump)(struct ekContext *E, ekDumpParams *params, struct ekValue *p); // creates debug text representing value, caller responsible for ekFree()

typedef struct ekValueType
{
    ekS32 id;
    char name[EVT_MAXNAMELEN + 1];
    int format;                                      // Format string type representation when using ekContextGetArgs(). By convention, custom types use a capital letter ('F')

    struct ekMap *intrinsics;                        // map of funcName -> ekCFunction, used as an Index fallback for things like .length()

    void *userData;                                  // per-type global structure used to hold any static data a type needs
    ekValueTypeDestroyUserData funcDestroyUserData;  // optional destructor for userdata

    ekValueTypeFuncClear funcClear;
    ekValueTypeFuncClone funcClone;
    ekValueTypeFuncToBool funcToBool;
    ekValueTypeFuncToInt funcToInt;
    ekValueTypeFuncToFloat funcToFloat;
    ekValueTypeFuncToString funcToString;
    ekValueTypeFuncReverse funcReverse;
    ekValueTypeFuncIter funcIter;
    ekValueTypeFuncArithmetic funcArithmetic;
    ekValueTypeFuncCmp funcCmp;
    ekValueTypeFuncLength funcLength;
    ekValueTypeFuncIndex funcIndex;
    ekValueTypeFuncDump funcDump;
} ekValueType;

ekValueType *ekValueTypeCreate(struct ekContext *E, const char *name, int format);
void ekValueTypeDestroy(struct ekContext *E, ekValueType *type);
void ekValueTypeAddIntrinsic(struct ekContext *E, ekValueType *type, const char *name, ekCFunction func);
struct ekValue *ekValueTypeGetIntrinsic(struct ekContext *E, ekU32 type, struct ekValue *index, ekBool lvalue);
ekS32 ekValueTypeRegister(struct ekContext *E, ekValueType *newType); // takes ownership of newType (use ekAlloc), returns new type id
ekS32 ekValueTypeId(struct ekContext *E, int format); // returns TypeId associated with type (by format)

void ekValueTypeRegisterAllBasicTypes(struct ekContext *E);

void ekValueTypeRegisterNull(struct ekContext *E);
void ekValueTypeRegisterBlock(struct ekContext *E);
void ekValueTypeRegisterBool(struct ekContext *E);
void ekValueTypeRegisterCFunction(struct ekContext *E);
void ekValueTypeRegisterInt(struct ekContext *E);
void ekValueTypeRegisterFloat(struct ekContext *E);
void ekValueTypeRegisterString(struct ekContext *E);
void ekValueTypeRegisterArray(struct ekContext *E);
void ekValueTypeRegisterObject(struct ekContext *E);
void ekValueTypeRegisterRef(struct ekContext *E);

#define ekValueTypePtr(id) ((ekValueType*)E->types[id])

// If the function ptr doesn't exist, just return 0 (NULL) safely, otherwise call it with arguments after the macro
#define ekValueTypeSafeCall(id, funcName) \
    (((ekValueType*)E->types[id])->func ## funcName == NULL) ? 0 \
    : ((ekValueType*)E->types[id])->func ## funcName


#endif
