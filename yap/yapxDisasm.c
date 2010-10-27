#include "yapxDisasm.h"

#include "yapBlock.h"
#include "yapModule.h"
#include "yapOp.h"

#include <string.h>
#include <stdio.h>

void yapOpsDump(yapOp *ops, int count)
{
    int i = 0;
    for(i=0;i<count;i++)
    {
        yapOp *op = &ops[i];

        switch(op->opcode)
        {
#define HURR(OP, NAME) case OP: printf("%12s", NAME); break;
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
            HURR(YOP_AND, "and")
            HURR(YOP_OR, "or")

            HURR(YOP_IF, "if")
            HURR(YOP_ENTER, "enter")
            HURR(YOP_LEAVE, "leave")
            HURR(YOP_BREAK, "break")

            HURR(YOP_ADD, "add")
            HURR(YOP_SUB, "sub")
            HURR(YOP_MUL, "mul")
            HURR(YOP_DIV, "div")

            HURR(YOP_TOSTRING, "tostring")
            HURR(YOP_TOINT, "toint")

            HURR(YOP_NOT, "not")

            HURR(YOP_FORMAT, "format")

            HURR(YOP_VARREG_KS, "varreg_ks")
            HURR(YOP_VARREF_KS, "varref_ks")
            HURR(YOP_REFVAL, "refval")
            HURR(YOP_SETVAR, "setvar")
#undef HURR

            default: printf("%12s", "??");
        }
        printf("%3d\n", op->operand);
    }
}

void yapModuleDump(yapModule *module)
{
    int i;

    for(i=0; i<module->kStrings.count; i++)
    {
        printf(".kstr %3d    \"%s\"\n", i, (char*)module->kStrings.data[i]);
    }

    for(i=0; i<module->kInts.count; i++)
    {
        printf(".kint %3d    %d\n", i, module->kInts.data[i]);
    }

    for(i=0; i<module->blocks.count; i++)
    {
        yapBlock *block = module->blocks.data[i];
        if(block != module->block)
        {
            printf("\n.block %d\n", i);
            yapOpsDump(block->ops, block->opCount);
        }
    }

    printf("\n.main\n");
    yapOpsDump(module->block->ops, module->block->opCount);
    printf("\n");
}
