// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPBLOCK_H
#define YAPBLOCK_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCode;

// ---------------------------------------------------------------------------

typedef struct yapBlock
{
    struct yapChunk *chunk;
    struct yapOp *ops;
    int opCount;
    int argCount;
} yapBlock;

#define yapBlockCreate() ((yapBlock*)yapAlloc(sizeof(yapBlock)))
void yapBlockDestroy(yapBlock *block);

yOperand yapBlockConvertCode(struct yapCode *code, struct yapChunk *owner, int argCount);

#endif
