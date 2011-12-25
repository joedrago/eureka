#ifndef YAPCHUNK_H
#define YAPCHUNK_H

#include "yapTypes.h"

typedef struct yapChunk
{
    struct yapBlock *block;            // Chunks are callable

    // "assembly"
    yapArray blocks;

    // constants
    yapArray kStrings;                 // constant string table
    yap32Array kInts;                  // constant integer table

    yBool hasFuncs;                    // at least one of the blocks represents a function
    yBool temporary;                   // signifies that the ktables aren't sticking around, and to dupe into values
} yapChunk;

#define yapChunkCreate() ((yapChunk*)yapAlloc(sizeof(yapChunk)))
void yapChunkDestroy(yapChunk *chunk);

yOperand yapChunkAddBlock(yapChunk *chunk, struct yapBlock *block);

#endif
