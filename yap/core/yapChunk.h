// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPCHUNK_H
#define YAPCHUNK_H

#include "yapArray.h"

struct yapContext;

typedef struct yapChunk
{
    struct yapBlock *block;            // Chunks are callable

    // "assembly"
    struct yapBlock **blocks;

    // constants
    char **kStrings;                   // constant string table
    yap32Array kInts;                  // constant integer table
    yap32Array kFloats;                // constant float table

    yBool hasFuncs;                    // at least one of the blocks represents a function
    yBool temporary;                   // signifies that the ktables aren't sticking around, and to dupe into values
} yapChunk;

#define yapChunkCreate() ((yapChunk*)yapAlloc(sizeof(yapChunk)))
void yapChunkDestroy(struct yapContext *Y, yapChunk *chunk);

yOperand yapChunkAddBlock(struct yapContext *Y, yapChunk *chunk, struct yapBlock *block);

#endif
