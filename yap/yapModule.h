#ifndef YAPMODULE_H
#define YAPMODULE_H

#include "yapTypes.h"
#include "yapOp.h"

typedef struct yapModule
{
    struct yapBlock *block;            // Modules are callable
    yapArray variables;

    // "code"
    yapArray blocks;

    // constants
    yapArray kStrings;                 // constant string table
    yap32Array kInts;                  // constant integer table
} yapModule;

#define yapModuleCreate() ((yapModule*)yapAlloc(sizeof(yapModule)))
void yapModuleDestroy(yapModule *module);

yOperand yapModuleAddBlock(yapModule *module, struct yapBlock *block);

void yapModuleDump(yapModule *module);

#endif
