// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapChunk.h"

#include "yapBlock.h"

void yapChunkDestroy(yapChunk *chunk)
{
    // chunk->block is either NULL or pointing inside chunk->blocks
    yapArrayClear(&chunk->blocks, (yapDestroyCB)yapBlockDestroy);

    yapArrayClear(&chunk->kStrings, (yapDestroyCB)yapFree);
    yap32ArrayClear(&chunk->kInts);

    yapFree(chunk);
}

yOperand yapChunkAddBlock(yapChunk *chunk, struct yapBlock *block)
{
    block->chunk = chunk;
    return yapArrayPush(&chunk->blocks, block);
}
