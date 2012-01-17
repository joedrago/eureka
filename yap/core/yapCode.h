// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPCODE_H
#define YAPCODE_H

#include "yapTypes.h"
#include "yapArray.h"

typedef struct yapCode
{
    struct yapOp *ops;
    yS32 size;
    yS32 count;
} yapCode;

#define yapCodeCreate() ((yapCode*)yapAlloc(sizeof(yapCode)))
void yapCodeDestroy(yapCode *code);

void yapCodeGrow(yapCode *code, int count);
yS32 yapCodeAppend(yapCode *code, yOpcode opcode, yOperand operand, int line);
void yapCodeConcat(yapCode *dst, yapCode *src);

#endif
