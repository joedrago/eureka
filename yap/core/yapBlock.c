#include "yapBlock.h"

#include "yapCode.h"
#include "yapOp.h"

void yapBlockDestroy(yapBlock *block)
{
    yapOpsFree(block->ops);
    yapFree(block);
}

yOperand yapBlockConvertCode(struct yapCode *code, struct yapChunk *owner, int argCount)
{
    yOperand ret;
    yapBlock *block = yapBlockCreate();
    block->ops = code->ops;
    block->opCount = code->count;
    block->argCount = argCount;

    ret = yapChunkAddBlock(owner, block);

    code->ops = NULL;
    code->size = 0;
    code->count = 0;
    yapCodeDestroy(code);
    return ret;
}
