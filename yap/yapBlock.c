#include "yapBlock.h"

#include "yapCode.h"
#include "yapOp.h"

void yapBlockDestroy(yapBlock *block)
{
    yapOpsFree(block->ops);
    yapFree(block);
}

yOperand yapBlockConvertCode(struct yapCode *code, struct yapModule *owner)
{
    yOperand ret;
    yapBlock *block = yapBlockCreate();
    block->ops = code->ops;
    block->opcount = code->count;
    ret = yapModuleAddBlock(owner, block);

    code->ops = NULL;
    code->size = 0;
    code->count = 0;
    return ret;
}

