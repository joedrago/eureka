#include "yapModule.h"

#include "yapBlock.h"
#include "yapVariable.h"

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
