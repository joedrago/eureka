#ifndef YAPVM_H
#define YAPVM_H

#include "yapTypes.h"

typedef struct yapVM
{
    // things that can own values
    yapArray globals;                  // Global variables
    yapArray frames;                   // Current stack frames
    yapArray stack;                    // Value stack
    yapArray modules;                  // the VM owns all modules, making cheap vars

    // "code"
    yapArray blocks;                   // the VM owns all blocks, making cheap vars

    // GC data
    yapArray usedValues;               // All values used by the system
    yapArray freeValues;               // free list of value objects

    // error data
    char *error;
} yapVM;

yapVM * yapVMCreate(void);
void yapVMDestroy(yapVM *vm);

int yapVMCompile(yapVM *vm, const char *text);
void yapVMCall(yapVM *vm, const char *name, int numArgs);

void yapVMSetError(yapVM *vm, const char *errorFormat, ...);
void yapVMClearError(yapVM *vm);

void yapVMGC(struct yapVM *vm);

// ---------------------------------------------------------------------------

#endif
