#ifndef YAPVALUE_H
#define YAPVALUE_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapVM;

// ---------------------------------------------------------------------------

typedef enum yapValueType
{
    YVT_NULL,

    YVT_MODULE,
    YVT_BLOCK,
    YVT_CFUNCTION,

    YVT_INT,
    YVT_STRING,

    YVT_ARRAY,

    YVT_REF,                           // Variable reference

    YVT_COUNT
} yapValueType;

// ---------------------------------------------------------------------------

// Should return how many values it is returning on the stack
typedef yU32 (yapCFunction)(struct yapVM *vm, yU32 argCount);

typedef struct yapValue
{
    yU8 type;
    yFlag constant:1;                  // Pointing at a constant table, do not free
    yFlag used:1;                      // The "mark" during the GC's mark-and-sweep
    yFlag shared:1;                    // GC's second mark; flags copy-on-write for string
    union
    {
        yS32 intVal;
        struct yapModule *moduleVal;
        struct yapBlock *blockVal;     // Hurr, Shield Slam
        char *stringVal;
        struct yapVariable *refVal;
        yapCFunction *cFuncVal;
        yapArray *arrayVal;
    };
} yapValue;

yapValue * yapValueCreate(struct yapVM *vm);

void yapValueCloneData(struct yapVM *vm, yapValue *dst, yapValue *src);
yapValue * yapValueClone(struct yapVM *vm, yapValue *p);

void yapValueSetInt(yapValue *p, int v);
void yapValueSetKString(yapValue *p, char *s);
void yapValueSetString(yapValue *p, char *s);
void yapValueDonateString(yapValue *p, char *s); // grants ownership
void yapValueSetFunction(yapValue *p, struct yapBlock *block);
void yapValueSetCFunction(yapValue *p, yapCFunction func);

void yapValueArrayPush(yapValue *p, yapValue *v);

void yapValueClear(yapValue *p);

void yapValueMark(yapValue *value);    // used by yapVMGC()
void yapValueDestroy(yapValue *p);     // only yapVMDestroy() should -ever- call this

yapValue * yapValueAdd(struct yapVM *vm, yapValue *a, yapValue *b);
yapValue * yapValueSub(struct yapVM *vm, yapValue *a, yapValue *b);
yapValue * yapValueMul(struct yapVM *vm, yapValue *a, yapValue *b);
yapValue * yapValueDiv(struct yapVM *vm, yapValue *a, yapValue *b);

yapValue * yapValueToBool(struct yapVM *vm, yapValue *p);
yapValue * yapValueToInt(struct yapVM *vm, yapValue *p);
yapValue * yapValueToString(struct yapVM *vm, yapValue *p);

yapValue * yapValueStringFormat(struct yapVM *vm, yapValue *format, yS32 argCount);

#define yapValueIsCallable(VAL)     \
    (  (VAL->type == YVT_MODULE)    \
    || (VAL->type == YVT_BLOCK)     \
    || (VAL->type == YVT_CFUNCTION))

// ---------------------------------------------------------------------------
// Globals

extern yapValue yapValueNull;

// ---------------------------------------------------------------------------

#endif
