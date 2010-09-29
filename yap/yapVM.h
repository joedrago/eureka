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
    yapArray usedVariables;            // All variables used by the system

    // error data
    char *error;
} yapVM;

yapVM * yapVMCreate(void);
void yapVMDestroy(yapVM *vm);

struct yapModule * yapVMLoadModule(yapVM *vm, const char *name, const char *text);

void yapVMSetError(yapVM *vm, const char *errorFormat, ...);
void yapVMClearError(yapVM *vm);

void yapVMGC(struct yapVM *vm);
struct yapFrame * yapVMPushFrame(yapVM *vm, struct yapVariable *ref, int numArgs);
void yapVMLoop(yapVM *vm);

// ---------------------------------------------------------------------------

#endif
