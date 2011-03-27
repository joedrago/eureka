#ifndef YAPVM_H
#define YAPVM_H

#include "yapTypes.h"
#include "yapValue.h"                  // for yapCFunction

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
    yapArray freeValues;               // Free value pool

    // state
    int lastRet;
    struct yapValue *nextThis;         // Set by INDEX (and object CALLs), used/reset by PushFrame

    // error data
    char *error;
} yapVM;

yapVM * yapVMCreate(void);
void yapVMDestroy(yapVM *vm);

void yapVMRegisterIntrinsic(yapVM *vm, const char *name, yapCFunction func);
struct yapModule * yapVMLoadModule(yapVM *vm, const char *name, const char *text);

void yapVMSetError(yapVM *vm, const char *errorFormat, ...);
void yapVMClearError(yapVM *vm);

void yapVMGC(struct yapVM *vm);

struct yapFrame * yapVMPushFrame(yapVM *vm, struct yapBlock *block, int numArgs, yU32 frameType, yU32 flags);
struct yapFrame * yapVMPopFrames(yapVM *vm, yU32 frameTypeToFind, yBool keepIt);

void yapVMLoop(yapVM *vm);

void yapVMPopValues(yapVM *vm, yU32 count);
yapValue * yapVMGetValue(yapVM *vm, yU32 howDeep); // 0 is "top of stack"

#define yapVMGetTop(VM) yapVMGetValue(VM, 0)
#define yapVMGetArg(VM, INDEX, ARGCOUNT) yapVMGetValue(VM, (ARGCOUNT-1) - INDEX)

// ---------------------------------------------------------------------------

#endif
