#include "yapModule.h"

#include "yapBlock.h"
#include "yapVariable.h"

#include <stdio.h>

void yapModuleDestroy(yapModule *module)
{
    // module->block is owned by vm->blocks
    yapArrayClear(&module->variables, NULL);

    yapArrayClear(&module->blocks, (yapDestroyCB)yapBlockDestroy);

    yapArrayClear(&module->kStrings, (yapDestroyCB)yapFree);
    yap32ArrayClear(&module->kInts);

    yapFree(module);
}

yOperand yapModuleAddBlock(yapModule *module, struct yapBlock *block)
{
    block->module = module;
    return yapArrayPush(&module->blocks, block);
}

void yapModuleDump(yapModule *module)
{
    int i;

    for(i=0; i<module->kStrings.count; i++)
    {
        yapTrace((".kstr %3d    \"%s\"\n", i, (char*)module->kStrings.data[i]));
    }

    for(i=0; i<module->kInts.count; i++)
    {
        yapTrace((".kint %3d    %d\n", i, module->kInts.data[i]));
    }

    for(i=0; i<module->blocks.count; i++)
    {
        yapBlock *block = module->blocks.data[i];
        if(block != module->block)
        {
            yapTrace(("\n.block %d\n", i));
            yapOpsDump(block->ops, block->opCount);
        }
    }

    yapTrace(("\n.main\n"));
    yapOpsDump(module->block->ops, module->block->opCount);
    yapTrace(("\n"));
}

