#ifndef YAPVALUE_H
#define YAPVALUE_H

#include "yapTypes.h"

typedef enum yapValueType
{
    YVT_UNKNOWN = 0,

    YVT_MODULE,
    YVT_FUNCTION,

    YVT_INT,
    YVT_STRING,

    YVT_COUNT
} yapValueType;

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
    };
} yapValue;

struct yapVM;

yapValue * yapValueCreate(struct yapVM *vm);
yapValue * yapValueClone(struct yapVM *vm, yapValue *p);

void yapValueSetInt(yapValue *p, int v);
void yapValueSetKString(yapValue *p, char *s);
void yapValueSetString(yapValue *p, char *s);

void yapValueClear(yapValue *p);
void yapValueFree(yapValue *p);

#define yapValueIsCallable(VAL) ((VAL.type == YVT_MODULE) || (VAL.type == YVT_FUNCTION))

// ---------------------------------------------------------------------------

#endif

