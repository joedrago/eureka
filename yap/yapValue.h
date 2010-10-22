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

// yapValueCreate() is only ever called via yapValueAcquire
yapValue * yapValueAcquire(struct yapVM *vm);
void yapValueRelease(struct yapVM *vm, yapValue *p); // returns to free pool

void yapValueMark(yapValue *value);    // used by yapVMGC()
void yapValueDestroy(yapValue *p);     // only yapVMDestroy() should -ever- call this

void yapValueClear(yapValue *p);

void yapValueCloneData(struct yapVM *vm, yapValue *dst, yapValue *src);
yapValue * yapValueClone(struct yapVM *vm, yapValue *p);
yapValue * yapValuePersonalize(struct yapVM *vm, yapValue *p); // only clones if used

yapValue * yapValueSetInt(struct yapVM *vm, yapValue *p, int v);
yapValue * yapValueSetKString(struct yapVM *vm, yapValue *p, char *s);
yapValue * yapValueSetString(struct yapVM *vm, yapValue *p, char *s);
yapValue * yapValueDonateString(struct yapVM *vm, yapValue *p, char *s); // grants ownership to the char*
yapValue * yapValueSetFunction(struct yapVM *vm, yapValue *p, struct yapBlock *block);
yapValue * yapValueSetCFunction(struct yapVM *vm, yapValue *p, yapCFunction func);
yapValue * yapValueSetModule(struct yapVM *vm, yapValue *p, struct yapModule *module);
yapValue * yapValueSetRef(struct yapVM *vm, yapValue *p, struct yapVariable *variable);

yBool yapValueSetRefVal(struct yapVM *vm, yapValue *ref, yapValue *p);

void yapValueArrayPush(yapValue *p, yapValue *v);

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
