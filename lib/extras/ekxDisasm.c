// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekxDisasm.h"

#include "ekBlock.h"
#include "ekChunk.h"
#include "ekOp.h"

#include <string.h>
#include <stdio.h>

void ekOpsDump(ekOp *ops, ekS32 count)
{
    ekS32 i = 0;
    for(i = 0; i < count; i++)
    {
        ekOp *op = &ops[i];

        switch(op->opcode)
        {
#define HURR(OP, NAME) case OP: printf("%12s", NAME); break;
                HURR(EOP_NOP, "nop")

                HURR(EOP_PUSHNULL, "pn")
                HURR(EOP_PUSHTHIS, "pt")
                HURR(EOP_PUSHI, "pi")
                HURR(EOP_PUSH_KB, "pkb")

                HURR(EOP_PUSH_KI, "pki")
                HURR(EOP_PUSH_KF, "pkf")
                HURR(EOP_PUSH_KS, "pks")

                HURR(EOP_INDEX, "index")

                HURR(EOP_DUPE, "dupe")
                HURR(EOP_MOVE, "move")
                HURR(EOP_POP, "pop")

                HURR(EOP_CALL, "call")
                HURR(EOP_RET, "ret")
                HURR(EOP_CLEANUP, "cleanup")
                HURR(EOP_KEEP, "keep")
                HURR(EOP_CLOSE, "close")

                HURR(EOP_START, "start")
                HURR(EOP_AND, "and")
                HURR(EOP_OR, "or")

                HURR(EOP_IF, "if")
                HURR(EOP_ENTER, "enter")
                HURR(EOP_LEAVE, "leave")
                HURR(EOP_CONTINUE, "continue")
                HURR(EOP_BREAK, "break")

                HURR(EOP_ADD, "add")
                HURR(EOP_SUB, "sub")
                HURR(EOP_MUL, "mul")
                HURR(EOP_DIV, "div")

                HURR(EOP_CMP, "cmp")
                HURR(EOP_EQUALS, "eq")
                HURR(EOP_NOTEQUALS, "ne")
                HURR(EOP_GREATERTHAN, "gt")
                HURR(EOP_GREATERTHANOREQUAL, "gte")
                HURR(EOP_LESSTHAN, "lt")
                HURR(EOP_LESSTHANOREQUAL, "lte")

                HURR(EOP_NOT, "not")

                HURR(EOP_BITWISE_NOT, "bnot")
                HURR(EOP_BITWISE_XOR, "xor")
                HURR(EOP_BITWISE_AND, "band")
                HURR(EOP_BITWISE_OR, "bor")
                HURR(EOP_SHIFTLEFT, "shl")
                HURR(EOP_SHIFTRIGHT, "shr")

                HURR(EOP_FORMAT, "format")

                HURR(EOP_VREG, "vreg")
                HURR(EOP_VREF, "vref")
                HURR(EOP_VVAL, "vval")
                HURR(EOP_REFVAL, "refval")
                HURR(EOP_VSET, "vset")
                HURR(EOP_INHERITS, "inherits")
                HURR(EOP_VARARGS, "varargs")

                HURR(EOP_ARRAY, "array")
                HURR(EOP_MAP, "map")

                HURR(EOP_ITER, "iter")

#undef HURR

            default:
                printf("%12s", "??");
        }
        printf("%3d", op->operand);
#ifdef EUREKA_DEBUG_SYMBOLS
        printf("         line %4d", op->line);
#endif
        printf("\n");
    }
}

void ekChunkDump(struct ekContext *E, ekChunk *chunk)
{
    ekS32 i;

    for(i = 0; i < ekArraySize(E, &chunk->kStrings); i++)
    {
        printf(".kstr %3d    \"%s\"\n", i, chunk->kStrings[i]);
    }

    for(i = 0; i < ek32ArraySize(E, &chunk->kInts); i++)
    {
        printf(".kint %3d    %d\n", i, chunk->kInts[i]);
    }

    for(i = 0; i < ek32ArraySize(E, &chunk->kFloats); i++)

    {
        printf(".kfloat %3d    %f\n", i, chunk->kFloats[i]);
    }

    for(i = 0; i < ekArraySize(E, &chunk->blocks); i++)
    {
        ekBlock *block = chunk->blocks[i];
        if(block != chunk->block)
        {
            printf("\n.block %d\n", i);
            ekOpsDump(block->ops, block->opCount);
        }
    }

    printf("\n.main\n");
    ekOpsDump(chunk->block->ops, chunk->block->opCount);
    printf("\n");
}
