#ifndef YAPBLOCK_H
#define YAPBLOCK_H

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

yapBlock * yapBlockConvertCode(struct yapCode *code, struct yapModule *owner);

#endif
