#ifndef YAPBLOCK_H
#define YAPBLOCK_H

typedef struct yapBlock
{
    struct yapModule *module;
    struct yapOp *ops;
} yapBlock;

#define yapBlockCreate() ((yapBlock*)yapAlloc(sizeof(yapBlock)))
void yapBlockDestroy(yapBlock *block);

#endif
