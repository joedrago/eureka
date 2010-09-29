#include "yapModule.h"

#include "yapVariable.h"

void yapModuleDestroy(yapModule *module)
{
    // module->block is owned by vm->blocks
    yapArrayClear(&module->variables, (yapDestroyCB)yapVariableDestroy);

    yapArrayClear(&module->kStrings, (yapDestroyCB)yapFree);
    yap32ArrayClear(&module->kInts);

    yapFree(module);
}
