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

yS32 yapCodeAppend(yapCode *code, yOpcode opcode, yOperand operand)
{
    yapOp *op = &code->ops[code->count];
    op->opcode  = opcode;
    op->operand = operand;
    code->count++;
    return code->count - 1;
}
