// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekBlock.h"

#include "ekContext.h"
#include "ekChunk.h"
#include "ekCode.h"
#include "ekOp.h"

void ekBlockDestroy(struct ekContext * E, ekBlock * block)
{
    ekOpsFree(block->ops);
    ekFree(block);
}

ekOperand ekBlockConvertCode(struct ekContext * E, struct ekCode * code, struct ekChunk * owner, ekS32 argCount)
{
    ekOperand ret;
    ekBlock * block = ekBlockCreate();
    block->ops = code->ops;
    block->opCount = code->count;
    block->argCount = argCount;

    ret = ekChunkAddBlock(E, owner, block);

    code->ops = NULL;
    code->size = 0;
    code->count = 0;
    ekCodeDestroy(E, code);
    return ret;
}
