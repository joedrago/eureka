#include "yapChunk.h"

#include "yapBlock.h"
#include "yapVariable.h"

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
