#ifndef YAPVM_H
#define YAPVM_H

#include "yapTypes.h"

// In the comments below, X is the operand, and the k tables are owned by translation blocks.

typedef enum yapOpBlock
{
    YOP_NOOP = 0,            // Does nothing

    YOP_PUSHKI,              // Push constant int ki[X] on top of stack
    YOP_ADDKI,               // Add constant int ki[X] to top of stack
    YOP_SUBKI,               // Subtract constant int ki[X] to top of stack

    YOP_CALL,                // Calls function named ks[X], using the current frame's stack pushes as args
    YOP_RET,                 // Leave current call, returning X items on the stack

    YOP_COUNT
} yapOpBlock;

typedef struct yapOp
{
    yU16 opcode;
    yU16 operand;
} yapOp;

// ---------------------------------------------------------------------------

typedef struct yapBlock
{
    char *name;
    yapOp *ops;
} yapBlock;

int yapCompile(const char *text, yapArray *outBlocks);
void yapBlockDestroy(yapBlock *func);

// ---------------------------------------------------------------------------

typedef struct yapFrame
{
    yapBlock *block;
    yapOp *ip;               // Instruction Pointer
    yS32   bp;               // Base pointer (prev. stack depth minus arg count)
    yapArray ret;            // Contains returned values from the most recent RET
} yapFrame;

void yapFrameFree(yapFrame *frame);

// ---------------------------------------------------------------------------

typedef enum yapValueType
{
    YVT_UNKNOWN = 0,
    YVT_INT,

    YVT_COUNT
} yapValueType;

typedef struct yapValue
{
    yU8 type;
    yS32 intValue;
} yapValue;

void yapValueClear(yapValue *p);
void yapValueFree(yapValue *p);

// ---------------------------------------------------------------------------

typedef struct yapVM
{
    yapArray blocks;          // All linked translation blocks
    yapArray frames;         // Current stack frames
    yapArray stack;          // Value stack
    char *error;
} yapVM;

yapVM * yapVMCreate(void);
void yapVMDestroy(yapVM *vm);
void yapVMCall(yapVM *vm, const char *funcName, int numArgs);

void yapVMSetError(yapVM *vm, const char *errorFormat, ...);
void yapVMClearError(yapVM *vm);

// ---------------------------------------------------------------------------

#endif
