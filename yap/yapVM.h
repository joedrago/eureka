#ifndef YAPVM_H
#define YAPVM_H

#include "yapTypes.h"

// In the comments below, X is the operand, and the k tables are owned by translation units.

typedef enum yapOpCode
{
    YOP_NOOP = 0,            // Does nothing

    YOP_PUSHKI,              // Push constant int ki[X] on top of stack
    YOP_ADDKI,               // Add constant int ki[X] to top of stack
    YOP_SUBKI,               // Subtract constant int ki[X] to top of stack

    YOP_CALL,                // Calls function named ks[X], using the current frame's stack pushes as args
    YOP_RET,                 // Leave current call, returning X items on the stack

    YOP_COUNT
} yapOpCode;

typedef struct yapOp
{
    yU16 opcode;
    yU16 operand;
} yapOp;

// ---------------------------------------------------------------------------

typedef struct yapUnit
{
    yapArray funcs;
    yapOp *code;
} yapUnit;

yapUnit * yapUnitCompile(const char *text);
void yapUnitDestroy(yapUnit *p);

// ---------------------------------------------------------------------------

typedef struct yapFunction
{
    char *name;
    yapUnit *unit;
    ySize pc;
} yapFunction;

void yapFunctionDestroy(yapFunction *func);

// ---------------------------------------------------------------------------

typedef struct yapFrame
{
    yapFunction *func;
    yapOp *ip;               // Instruction Pointer
    ySize stackPushes;       // Count of value stack pushes during this frame
} yapFrame;

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
    yapArray units;          // All linked translation units
    yapArray funcs;          // Combination of funcs from all linked units
    yapArray frames;         // Current stack frames
    yapArray stack;          // Value stack
    char *error;
} yapVM;

yapVM * yapVMCreate(void);
void yapVMLink(yapVM *vm, yapUnit *unit);
void yapVMDestroy(yapVM *vm);
void yapVMCall(yapVM *vm, const char *funcName);

void yapVMSetError(yapVM *vm, const char *errorFormat, ...);
void yapVMClearError(yapVM *vm);

// ---------------------------------------------------------------------------

#endif
