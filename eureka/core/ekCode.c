// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekCode.h"

#include "ekCompiler.h"
#include "ekContext.h"
#include "ekLexer.h"
#include "ekChunk.h"
#include "ekOp.h"

#include <string.h>
#include <stdio.h>

void ekCodeDestroy(struct ekContext *Y, ekCode *code)
{
    if(code->ops)
    {
        ekFree(code->ops);
    }
    ekFree(code);
}

void ekCodeGrow(struct ekContext *Y, ekCode *code, int count)
{
    if(code->count + count <= code->size)
    {
        return;
    }

    if(code->size)
    {
        code->ops = ekRealloc(code->ops, sizeof(ekOp) * (code->size + count));
    }
    else
    {
        code->ops = ekAlloc(sizeof(ekOp) * count);
    }
    code->size += count;
}

ekS32 ekCodeAppend(struct ekContext *Y, ekCode *code, ekOpcode opcode, ekOperand operand, int line)
{
    ekOp *op = &code->ops[code->count];
    op->opcode  = opcode;
    op->operand = operand;
#ifdef EUREKA_DEBUG_SYMBOLS
    op->line = line;
#endif
    code->count++;
    return code->count - 1;
}

void ekCodeConcat(struct ekContext *Y, ekCode *dst, ekCode *src)
{
    if(src->count)
    {
        ekCodeGrow(Y, dst, src->count);
        memcpy(&dst->ops[dst->count], src->ops, src->size * sizeof(ekOp));
        dst->count += src->count;
    }
}
