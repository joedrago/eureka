#include "yapBlock.h"

#include "yapCode.h"
#include "yapOp.h"

void yapBlockDestroy(yapBlock *block)
{
    yapOpsFree(block->ops);
    yapFree(block);
}

yapBlock * yapBlockConvertCode(struct yapCode *code, struct yapModule *owner)
{
    yapBlock *block = yapBlockCreate();
    block->ops = code->ops;
    block->opcount = code->count;
    block->module = owner;
    code->ops = NULL;
    code->size = 0;
    code->count = 0;
    return block;
}

