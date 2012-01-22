// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPCODE_H
#define YAPCODE_H

#include "yapTypes.h"

typedef struct yapCode
{
    struct yapOp *ops;
    yS32 size;
    yS32 count;
} yapCode;

#define yapCodeCreate() ((yapCode*)yapAlloc(sizeof(yapCode)))
void yapCodeDestroy(struct yapContext *Y, yapCode *code);

void yapCodeGrow(struct yapContext *Y, yapCode *code, int count);
yS32 yapCodeAppend(struct yapContext *Y, yapCode *code, yOpcode opcode, yOperand operand, int line);
void yapCodeConcat(struct yapContext *Y, yapCode *dst, yapCode *src);

#endif
