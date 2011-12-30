#ifndef YAPVM_H
#define YAPVM_H

#include "yapTypes.h"
#include "yapValue.h"                  // for yapCFunction

// ---------------------------------------------------------------------------
// Forwards

struct yapVariable;

// ---------------------------------------------------------------------------

// VM Error Types
enum
{
    YVE_NONE = 0,
    YVE_RUNTIME,
    YVE_COMPILE,

    YVE_COUNT
};

typedef struct yapVM
{
    // type information
    yapArray types;

    // things that can own values
    yapArray globals;                  // Global variables
    yapArray frames;                   // Current stack frames
    yapArray stack;                    // Value stack
    yapArray chunks;                  // the VM owns all chunks, making cheap vars

    // GC data
    yapArray usedValues;               // All values used by the system
    yapArray usedVariables;            // All variables used by the system
    yapArray freeValues;               // Free value pool

    // state
    int lastRet;

    // error data
    yU32 errorType;
    char *error;
} yapVM;

yapVM *yapVMCreate(void);
void yapVMDestroy(yapVM *vm);

void yapVMRegisterGlobalFunction(yapVM *vm, const char *name, yapCFunction func);

// Yap Eval Options
enum
{
    YEO_DEFAULT = 0,

    YEO_DUMP = (1 << 0)
};
void yapVMEval(yapVM *vm, const char *text, yU32 evalOpts);
void yapVMRecover(yapVM *vm); // cleans out frames, clears error

void yapVMSetError(yapVM *vm, yU32 errorType, const char *errorFormat, ...);
void yapVMClearError(yapVM *vm);

void yapVMGC(struct yapVM *vm);

struct yapFrame *yapVMPushFrame(yapVM *vm, struct yapBlock *block, int argCount, yU32 frameType);
struct yapFrame *yapVMPopFrames(yapVM *vm, yU32 frameTypeToFind, yBool keepIt);

void yapVMLoop(yapVM *vm, yBool stopAtPop); // stopAtPop means to stop processing if we ever have less frames than we started with

void yapVMPopValues(yapVM *vm, yU32 count);
yapValue *yapVMGetValue(yapVM *vm, yU32 howDeep);  // 0 is "top of stack"

yBool yapVMCallFuncByName(yapVM *vm, const char *name, int argCount); // returns whether or not it found it

#define yapVMGetTop(VM) yapVMGetValue(VM, 0)
#define yapVMGetArg(VM, INDEX, ARGCOUNT) yapVMGetValue(VM, (ARGCOUNT-1) - INDEX)

// ---------------------------------------------------------------------------

#endif
