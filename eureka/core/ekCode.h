// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EUREKACODE_H
#define EUREKACODE_H

#include "ekTypes.h"

typedef struct ekCode
{
    struct ekOp *ops;
    ekS32 size;
    ekS32 count;
} ekCode;

#define ekCodeCreate() ((ekCode*)ekAlloc(sizeof(ekCode)))
void ekCodeDestroy(struct ekContext *Y, ekCode *code);

void ekCodeGrow(struct ekContext *Y, ekCode *code, int count);
ekS32 ekCodeAppend(struct ekContext *Y, ekCode *code, ekOpcode opcode, ekOperand operand, int line);
void ekCodeConcat(struct ekContext *Y, ekCode *dst, ekCode *src);

#endif
