// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapBlock.h"

#include "yapContext.h"
#include "yapChunk.h"
#include "yapCode.h"
#include "yapOp.h"

void yapBlockDestroy(struct yapContext *Y, yapBlock *block)
{
    yapOpsFree(block->ops);
    yapFree(block);
}

yOperand yapBlockConvertCode(struct yapContext *Y, struct yapCode *code, struct yapChunk *owner, int argCount)
{
    yOperand ret;
    yapBlock *block = yapBlockCreate();
    block->ops = code->ops;
    block->opCount = code->count;
    block->argCount = argCount;

    ret = yapChunkAddBlock(Y, owner, block);

    code->ops = NULL;
    code->size = 0;
    code->count = 0;
    yapCodeDestroy(Y, code);
    return ret;
}

