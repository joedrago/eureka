#ifndef YAPOP_H
#define YAPOP_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------

typedef struct yapOp
{
    yOpcode  opcode;
    yOperand operand;
} yapOp;

#define yapOpsAlloc(COUNT) ((yapOp*)(yapAlloc(sizeof(yapOp) * COUNT)))
#define yapOpsFree(OPS) yapFree(OPS)

#endif
