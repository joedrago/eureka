// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekChunk.h"

#include "ekContext.h"
#include "ekBlock.h"

#include <stdio.h>

ekChunk * ekChunkCreate(struct ekContext * E, const char * sourcePath, ekBool useSourcePathForImports)
{
    ekChunk * chunk = (ekChunk *)ekAlloc(sizeof(ekChunk));
    if (sourcePath) {
        ekContextAbsolutePath(E, sourcePath, &chunk->sourcePath, ekFalse);
        if (useSourcePathForImports) {
            ekContextAbsolutePath(E, sourcePath, &chunk->searchPath, ekTrue);
        }
    }
    return chunk;
}

void ekChunkDestroy(struct ekContext * E, ekChunk * chunk)
{
    // chunk->block is either NULL or pointing inside chunk->blocks
    ekArrayDestroy(E, &chunk->blocks, (ekDestroyCB)ekBlockDestroy);

    ekArrayDestroy(E, &chunk->kStrings, (ekDestroyCB)ekDestroyCBFree);
    ek32ArrayDestroy(E, &chunk->kInts);
    ek32ArrayDestroy(E, &chunk->kFloats);

    ekStringClear(E, &chunk->sourcePath);
    ekStringClear(E, &chunk->searchPath);

    ekFree(chunk);
}

ekOperand ekChunkAddBlock(struct ekContext * E, ekChunk * chunk, struct ekBlock * block)
{
    block->chunk = chunk;
    return ekArrayPush(E, &chunk->blocks, block);
}
