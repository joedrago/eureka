#include "yapxDisasm.h"

#include "yapBlock.h"
#include "yapChunk.h"
#include "yapOp.h"

#include <string.h>
#include <stdio.h>

void yapOpsDump(yapOp *ops, int count)
{
    int i = 0;
    for(i = 0; i < count; i++)
    {
        yapOp *op = &ops[i];

        switch(op->opcode)
        {
#define HURR(OP, NAME) case OP: printf("%12s", NAME); break;
            HURR(YOP_NOP, "nop")

            HURR(YOP_PUSHNULL, "pushnull")
            HURR(YOP_PUSHTHIS, "pushthis")
            HURR(YOP_PUSHI, "pushi")
            HURR(YOP_PUSH_KB, "push_kb")

            HURR(YOP_PUSH_KI, "push_ki")
            HURR(YOP_PUSH_KF, "push_kf")
            HURR(YOP_PUSH_KS, "push_ks")

            HURR(YOP_INDEX, "index")

            HURR(YOP_DUPE, "dupe")
            HURR(YOP_MOVE, "move")
            HURR(YOP_POP, "pop")

            HURR(YOP_CALL, "call")
            HURR(YOP_RET, "ret")
            HURR(YOP_CLEANUP, "cleanup")
            HURR(YOP_KEEP, "keep")
            HURR(YOP_CLOSE, "close")

            HURR(YOP_START, "start")
            HURR(YOP_AND, "and")
            HURR(YOP_OR, "or")

            HURR(YOP_IF, "if")
            HURR(YOP_ENTER, "enter")
            HURR(YOP_LEAVE, "leave")
            HURR(YOP_CONTINUE, "continue")
            HURR(YOP_BREAK, "break")

            HURR(YOP_ADD, "add")
            HURR(YOP_SUB, "sub")
            HURR(YOP_MUL, "mul")
            HURR(YOP_DIV, "div")

            HURR(YOP_CMP, "cmp")
            HURR(YOP_EQUALS, "eq")
            HURR(YOP_NOTEQUALS, "ne")
            HURR(YOP_GREATERTHAN, "gt")
            HURR(YOP_GREATERTHANOREQUAL, "gte")
            HURR(YOP_LESSTHAN, "lt")
            HURR(YOP_LESSTHANOREQUAL, "lte")

            HURR(YOP_TOSTRING, "tostring")
            HURR(YOP_TOINT, "toint")
            HURR(YOP_TOFLOAT, "tofloat")

            HURR(YOP_NOT, "not")

            HURR(YOP_BITWISE_NOT, "bnot")
            HURR(YOP_BITWISE_XOR, "xor")
            HURR(YOP_BITWISE_AND, "band")
            HURR(YOP_BITWISE_OR, "bor")
            HURR(YOP_SHIFTLEFT, "shl")
            HURR(YOP_SHIFTRIGHT, "shr")

            HURR(YOP_FORMAT, "format")

            HURR(YOP_VARREG_KS, "varreg_ks")
            HURR(YOP_VARREF_KS, "varref_ks")
            HURR(YOP_REFVAL, "refval")
            HURR(YOP_SETVAR, "setvar")
            HURR(YOP_INHERITS, "inherits")
            HURR(YOP_VARARGS, "varargs")

            HURR(YOP_NTH, "nth")
            HURR(YOP_COUNT, "count")
#undef HURR

        default:
            printf("%12s", "??");
        }
        printf("%3d", op->operand);
#ifdef YAP_DEBUGGING
        printf("line %8d\n", op->line);
#endif
        printf("\n");
    }
}

void yapChunkDump(yapChunk *chunk)
{
    int i;

    for(i = 0; i < chunk->kStrings.count; i++)
    {
        printf(".kstr %3d    \"%s\"\n", i, (char *)chunk->kStrings.data[i]);
    }

    for(i = 0; i < chunk->kInts.count; i++)
    {
        printf(".kint %3d    %d\n", i, chunk->kInts.data[i]);
    }

    for(i = 0; i < chunk->kFloats.count; i++)
    {
        printf(".kfloat %3d    %f\n", i, *((yF32*)&chunk->kFloats.data[i]));
    }

    for(i = 0; i < chunk->blocks.count; i++)
    {
        yapBlock *block = chunk->blocks.data[i];
        if(block != chunk->block)
        {
            printf("\n.block %d\n", i);
            yapOpsDump(block->ops, block->opCount);
        }
    }

    printf("\n.main\n");
    yapOpsDump(chunk->block->ops, chunk->block->opCount);
    printf("\n");
}
