#ifndef YAPBLOCK_H
#define YAPBLOCK_H

typedef struct yapBlock
{
    struct yapModule *module;
    struct yapOp *ops;
} yapBlock;

void yapBlockFree(yapBlock *block);

#endif
