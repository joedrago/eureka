#ifndef YAPOP_H
#define YAPOP_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// In the comments below, X is the operand, and the k tables are owned by translation blocks.

enum
{
    YOP_NOOP = 0,                      // does nothing

    YOP_PUSHNULL,                      // push null value on top of stack
    YOP_PUSHLBLOCK,                    // push reference to local (to module) block

    YOP_PUSH_KI,                       // push constant int ki[X] on top of stack
    YOP_ADD_KI,                        // add constant int ki[X] to top of stack
    YOP_SUB_KI,                        // subtract constant int ki[X] to top of stack

    YOP_PUSH_KS,                       // push constant string ks[X] on top of stack

    YOP_POP,                           // pop value stack X times

    YOP_CALL,                          // calls function named ks[X], using the current frame's stack pushes as args
    YOP_RET,                           // leave current call, returning X items on the stack
    YOP_KEEP,                          // Pad/trim most recent RET to X args

    YOP_START,                         // no-op, marks the beginning of a loop for yapFrameReset
    YOP_SKIP,                          // skips over the next X ops; used in for loops

    YOP_IF,                            // pop value, then X+1 LBLOCKs. run 0 if true, 1 if false and present
    YOP_ENTER,                         // pops LBLOCK, pushes frame and enters new scope (a non-function CALL)
    YOP_LEAVE,                         // pop frame (non-function). if X=1, pop value and test first
    YOP_BREAK,                         // pop/finds LOOP frame. if 1, reset frame and ip. if 0, pop loop.
                                       // (BREAK 0 = break, BREAK 1 = continue)

    YOP_VARREG_KS,                     // register local variable named ks[X], push ref on value stack
    YOP_VARREF_KS,                     // find variable named ks[X], push ref on value stack
    YOP_REFVAL,                        // replace ref at top of stack with its value
    YOP_SETVAR,                        // pops [v,ref], sets *ref = v
    YOP_SETARG,                        // pops [ref,v], sets *ref = v

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
