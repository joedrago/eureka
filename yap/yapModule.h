#ifndef YAPMODULE_H
#define YAPMODULE_H

#include "yapTypes.h"
#include "yapOp.h"

typedef struct yapModule
{
    struct yapBlock *block;            // Modules are callable
    yapArray variables;
} yapModule;

void yapModuleFree(yapModule *module);

#endif
