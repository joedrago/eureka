#ifndef YAPVM_H
#define YAPVM_H

#include "yapTypes.h"

// In the comments below, X is the operand, and the k tables are owned by translation blocks.

enum
{
    YOP_NOOP = 0,                      // Does nothing

    YOP_PUSHKI,                        // Push constant int ki[X] on top of stack
    YOP_ADDKI,                         // Add constant int ki[X] to top of stack
    YOP_SUBKI,                         // Subtract constant int ki[X] to top of stack

    YOP_PUSHKS,                        // Push constant string ks[X] on top of stack

    YOP_PUSHARGN,                      // Push argument #X on top of stack

    YOP_CALL,                          // Calls function named ks[X], using the current frame's stack pushes as args
    YOP_RET,                           // Leave current call, returning X items on the stack

    YOP_COUNT
};

typedef struct yapOp
{
    yOpcode  opcode;
    yOperand operand;
} yapOp;

// ---------------------------------------------------------------------------

typedef struct yapModule yapModule;

typedef struct yapBlock
{
    yapModule *module;
    yapOp *ops;
} yapBlock;

void yapBlockFree(yapBlock *block);

// ---------------------------------------------------------------------------

typedef struct yapFrame
{
    yapArray variables;
    yapBlock *block;
    yapOp *ip;                         // Instruction Pointer
    yS32   bp;                         // Base pointer (prev. stack depth minus arg count)
    yapArray args;                     // Contains arguments to the called function/module
    yapArray ret;                      // Contains returned values from the most recent RET
} yapFrame;

void yapFrameFree(yapFrame *frame);

// ---------------------------------------------------------------------------

typedef enum yapValueType
{
    YVT_UNKNOWN = 0,

    YVT_MODULE,
    YVT_FUNCTION,

    YVT_INT,
    YVT_STRING,

    YVT_COUNT
} yapValueType;

typedef struct yapValue
{
    yU8 type;
    union
    {
        yS32 intVal;
        yapModule *moduleVal;
        yapBlock *blockVal;            // Hurr, Shield Slam
        char *stringVal;
    };
    yFlag constant:1;                  // Pointing at a constant table, do not free
} yapValue;

void yapValueClear(yapValue *p);
void yapValueFree(yapValue *p);
yapValue *yapValueDupe(yapValue *p);

#define yapValueIsCallable(VAL) ((VAL.type == YVT_MODULE) || (VAL.type == YVT_FUNCTION))

// ---------------------------------------------------------------------------

typedef struct yapVariable
{
    char *name;
    yapValue value;
} yapVariable;

yapVariable *yapVariableAlloc(const char *name);
void yapVariableFree(yapVariable *v);

// ---------------------------------------------------------------------------

typedef struct yapModule
{
    yapBlock *block;                   // Modules are callable
    yapArray variables;
} yapModule;

void yapModuleFree(yapModule *module);

// ---------------------------------------------------------------------------

typedef struct yapVM
{
    yapArray globals;                  // Global variables
    yapArray blocks;                   // the VM owns all blocks, making cheap vars
    yapArray modules;                  // the VM owns all modules, making cheap vars
    yapArray frames;                   // Current stack frames
    yapArray stack;                    // Value stack
    yapArray kStrings;                 // constant string table
    yap32Array kInts;                  // constant integer table
    char *error;
} yapVM;

yapVM * yapVMCreate(void);
void yapVMFree(yapVM *vm);
int yapVMCompile(yapVM *vm, const char *text);
void yapVMCall(yapVM *vm, const char *name, int numArgs);

void yapVMSetError(yapVM *vm, const char *errorFormat, ...);
void yapVMClearError(yapVM *vm);

// ---------------------------------------------------------------------------

#endif
