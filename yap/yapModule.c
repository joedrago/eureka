#include "yapModule.h"

#include "yapVariable.h"

void yapModuleFree(yapModule *module)
{
    // module->block is owned by vm->blocks
    yapArrayClear(&module->variables, (yapDestroyCB)yapVariableFree);
    yapFree(module);
}
