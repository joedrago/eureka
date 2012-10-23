// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapChunk.h"

#include "yapContext.h"
#include "yapBlock.h"

void yapChunkDestroy(struct yapContext *Y, yapChunk *chunk)
{
    // chunk->block is either NULL or pointing inside chunk->blocks
    yap2ArrayDestroy(Y, &chunk->blocks, (yapDestroyCB)yapBlockDestroy);

    yap2ArrayDestroy(Y, &chunk->kStrings, (yapDestroyCB)yapDestroyCBFree);
    yap32ArrayClear(Y, &chunk->kInts);
    yap32ArrayClear(Y, &chunk->kFloats);

    yapFree(chunk);
}

yOperand yapChunkAddBlock(struct yapContext *Y, yapChunk *chunk, struct yapBlock *block)
{
    block->chunk = chunk;
    return yap2ArrayPush(Y, &chunk->blocks, block);
}
