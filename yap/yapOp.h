#ifndef YAPOP_H
#define YAPOP_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// In the comments below, X is the operand, and the k tables are owned by translation blocks.

enum
{
    YOP_NOOP = 0,                      // does nothing

    YOP_PUSH_KI,                       // push constant int ki[X] on top of stack
    YOP_ADD_KI,                        // add constant int ki[X] to top of stack
    YOP_SUB_KI,                        // subtract constant int ki[X] to top of stack

    YOP_PUSH_KS,                       // push constant string ks[X] on top of stack

    YOP_PUSHARGN,                      // push argument #X on top of stack

    YOP_POP,                           // pop value stack X times

    YOP_CALL,                          // calls function named ks[X], using the current frame's stack pushes as args
    YOP_RET,                           // leave current call, returning X items on the stack

    YOP_VARREG_KS,                     // register local variable named ks[X], push ref on value stack
    YOP_VARREF_KS,                     // find variable named ks[X], push ref on value stack
    YOP_REFVAL,                        // replace ref at top of stack with its value
    YOP_SETVAR,                        // pops [v,ref], sets *ref = v

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

void yapOpsDump(yapOp *ops, int count);

#endif
