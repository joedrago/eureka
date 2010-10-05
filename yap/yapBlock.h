#ifndef YAPBLOCK_H
#define YAPBLOCK_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCode;

// ---------------------------------------------------------------------------

typedef struct yapBlock
{
    struct yapModule *module;
    struct yapOp *ops;
    int opcount;
} yapBlock;

#define yapBlockCreate() ((yapBlock*)yapAlloc(sizeof(yapBlock)))
void yapBlockDestroy(yapBlock *block);

yOperand yapBlockConvertCode(struct yapCode *code, struct yapModule *owner);

#endif
