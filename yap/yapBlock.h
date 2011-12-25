#ifndef YAPBLOCK_H
#define YAPBLOCK_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCode;

// ---------------------------------------------------------------------------

typedef struct yapBlock
{
    struct yapChunk *chunk;
    struct yapOp *ops;
    int opCount;
    int argCount;
} yapBlock;

#define yapBlockCreate() ((yapBlock*)yapAlloc(sizeof(yapBlock)))
void yapBlockDestroy(yapBlock *block);

yOperand yapBlockConvertCode(struct yapCode *code, struct yapChunk *owner, int argCount);

#endif
