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

    printf("* Dumping kStrings:\n");
    for(i=0; i<module->kStrings.count; i++)
    {
        printf("%6d \"%s\"\n", i, (char*)module->kStrings.data[i]);
    }

    printf("* Dumping kInts:\n");
    for(i=0; i<module->kInts.count; i++)
    {
        printf("%6d \"%d\"\n", i, module->kInts.data[i]);
    }

    printf("* Dumping blocks:\n");
    for(i=0; i<module->blocks.count; i++)
    {
        yapBlock *block = module->blocks.data[i];
        if(block != module->block)
        {
            printf("  * Block %d:\n", i);
            yapOpsDump(block->ops, block->opcount);
        }
    }

    printf("  * Main Block\n");
    yapOpsDump(module->block->ops, module->block->opcount);
}

