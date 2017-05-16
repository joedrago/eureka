// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKBLOCK_H
#define EKBLOCK_H

#include "ekTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekCode;

// ---------------------------------------------------------------------------

typedef struct ekBlock
{
    struct ekChunk * chunk;
    struct ekOp * ops;
    ekS32 opCount;
    ekS32 argCount;
} ekBlock;

#define ekBlockCreate() ((ekBlock *)ekAlloc(sizeof(ekBlock)))
void ekBlockDestroy(struct ekContext * E, ekBlock * block);

ekOperand ekBlockConvertCode(struct ekContext * E, struct ekCode * code, struct ekChunk * owner, ekS32 argCount);

#endif
