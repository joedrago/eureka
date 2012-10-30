// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EUREKACHUNK_H
#define EUREKACHUNK_H

#include "ekArray.h"

struct ekContext;

typedef struct ekChunk
{
    struct ekBlock *block;            // Chunks are callable

    // "assembly"
    struct ekBlock **blocks;

    // constants
    char **kStrings;                   // constant string table
    yS32 *kInts;                       // constant integer table
    yF32 *kFloats;                     // constant float table

    yBool hasFuncs;                    // at least one of the blocks represents a function
    yBool temporary;                   // signifies that the ktables aren't sticking around, and to dupe into values
} ekChunk;

#define ekChunkCreate() ((ekChunk*)ekAlloc(sizeof(ekChunk)))
void ekChunkDestroy(struct ekContext *Y, ekChunk *chunk);

yOperand ekChunkAddBlock(struct ekContext *Y, ekChunk *chunk, struct ekBlock *block);

#endif
