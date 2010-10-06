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
#define HURR(OP) case OP: yapTrace(("%20s", #OP)); break;
            HURR(YOP_NOOP)

            HURR(YOP_PUSHNULL)
            HURR(YOP_PUSHLBLOCK)

            HURR(YOP_PUSH_KI)
            HURR(YOP_ADD_KI)
            HURR(YOP_SUB_KI)

            HURR(YOP_PUSH_KS)

            HURR(YOP_POP)

            HURR(YOP_CALL)
            HURR(YOP_RET)
            HURR(YOP_KEEP)

            HURR(YOP_VARREG_KS)
            HURR(YOP_VARREF_KS)
            HURR(YOP_REFVAL)
            HURR(YOP_SETVAR)
            HURR(YOP_SETARG)
#undef HURR

            default: yapTrace(("%10s", "??"));
        }
        yapTrace(("%6d\n", op->operand));
    }
}
