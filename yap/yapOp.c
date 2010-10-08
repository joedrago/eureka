#include "yapOp.h"

#include <string.h>
#include <stdio.h>

yapOp *yapOpsMerge(yapOp *dst, int dstCount, yapOp *src, int srcCount)
{
    dst = yapRealloc(dst, sizeof(yapOp) * (dstCount+srcCount));
    memcpy(&dst[dstCount], src, srcCount*sizeof(yapOp));
    return dst;
}

void yapOpsDump(yapOp *ops, int count)
{
    int i = 0;
    for(i=0;i<count;i++)
    {
        yapOp *op = &ops[i];

        switch(op->opcode)
        {
#define HURR(OP, NAME) case OP: yapTrace(("%12s", NAME)); break;
            HURR(YOP_NOP, "nop")

            HURR(YOP_PUSHNULL, "pushnull")
            HURR(YOP_PUSHLBLOCK, "pushlblock")

            HURR(YOP_PUSH_KI, "push_ki")
            HURR(YOP_PUSH_KS, "push_ks")

            HURR(YOP_POP, "pop")

            HURR(YOP_CALL, "call")
            HURR(YOP_RET, "ret")
            HURR(YOP_KEEP, "keep")

            HURR(YOP_START, "start")
            HURR(YOP_SKIP, "skip")

            HURR(YOP_IF, "if")
            HURR(YOP_ENTER, "enter")
            HURR(YOP_LEAVE, "leave")
            HURR(YOP_BREAK, "break")

            HURR(YOP_ADD, "add")
            HURR(YOP_SUB, "sub")
            HURR(YOP_MUL, "mul")
            HURR(YOP_DIV, "div")

            HURR(YOP_VARREG_KS, "varreg_ks")
            HURR(YOP_VARREF_KS, "varref_ks")
            HURR(YOP_REFVAL, "refval")
            HURR(YOP_SETVAR, "setvar")
            HURR(YOP_SETARG, "setarg")
#undef HURR

            default: yapTrace(("%12s", "??"));
        }
        yapTrace(("%3d\n", op->operand));
    }
}

