// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekChunk.h"

#include "ekContext.h"
#include "ekBlock.h"

void ekChunkDestroy(struct ekContext *Y, ekChunk *chunk)
{
    // chunk->block is either NULL or pointing inside chunk->blocks
    ekArrayDestroy(Y, &chunk->blocks, (ekDestroyCB)ekBlockDestroy);

    ekArrayDestroy(Y, &chunk->kStrings, (ekDestroyCB)ekDestroyCBFree);
    ek32ArrayDestroy(Y, &chunk->kInts);
    ek32ArrayDestroy(Y, &chunk->kFloats);

    ekFree(chunk);
}

ekOperand ekChunkAddBlock(struct ekContext *Y, ekChunk *chunk, struct ekBlock *block)
{
    block->chunk = chunk;
    return ekArrayPush(Y, &chunk->blocks, block);
}
