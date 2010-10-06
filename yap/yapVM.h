#ifndef YAPVM_H
#define YAPVM_H

#include "yapTypes.h"
#include "yapValue.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapVariable;

// ---------------------------------------------------------------------------

typedef struct yapVM
{
    // things that can own values
    yapArray globals;                  // Global variables
    yapArray frames;                   // Current stack frames
    yapArray stack;                    // Value stack
    yapArray modules;                  // the VM owns all modules, making cheap vars

    // GC data
    yapArray usedValues;               // All values used by the system
    yapArray usedVariables;            // All variables used by the system

    // state
    int lastRet;

    // error data
    char *error;
} yapVM;

yapVM * yapVMCreate(void);
void yapVMDestroy(yapVM *vm);

void yapVMRegisterIntrinsic(yapVM *vm, const char *name, yapCFunction *func);
struct yapModule * yapVMLoadModule(yapVM *vm, const char *name, const char *text);

void yapVMSetError(yapVM *vm, const char *errorFormat, ...);
void yapVMClearError(yapVM *vm);

void yapVMGC(struct yapVM *vm);
struct yapFrame * yapVMPushFrame(yapVM *vm, struct yapVariable *ref, int numArgs);
void yapVMLoop(yapVM *vm);
yapValue * yapVMPopValue(yapVM *vm);

// ---------------------------------------------------------------------------

#endif
