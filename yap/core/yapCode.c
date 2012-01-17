// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapCode.h"

#include "yapCompiler.h"
#include "yapLexer.h"
#include "yapChunk.h"
#include "yapOp.h"

#include <string.h>
#include <stdio.h>

void yapCodeDestroy(yapCode *code)
{
    if(code->ops)
        yapFree(code->ops);
    yapFree(code);
}

void yapCodeGrow(yapCode *code, int count)
{
    if(code->count + count <= code->size)
        return;

    if(code->size)
        code->ops = yapRealloc(code->ops, sizeof(yapOp) * (code->size + count));
    else
        code->ops = yapAlloc(sizeof(yapOp) * count);
    code->size += count;
}

yS32 yapCodeAppend(yapCode *code, yOpcode opcode, yOperand operand, int line)
{
    yapOp *op = &code->ops[code->count];
    op->opcode  = opcode;
    op->operand = operand;
#ifdef YAP_DEBUGGING
    op->line = line;
#endif
    code->count++;
    return code->count - 1;
}

void yapCodeConcat(yapCode *dst, yapCode *src)
{
    if(src->count)
    {
        yapCodeGrow(dst, src->count);
        memcpy(&dst->ops[dst->count], src->ops, src->size * sizeof(yapOp));
        dst->count += src->count;
    }
}
