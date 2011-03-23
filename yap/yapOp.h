#ifndef YAPOP_H
#define YAPOP_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// In the comments below, X is the operand, and the k tables are owned by translation blocks.

enum
{
    YOP_NOP = 0,                      // does nothing

    YOP_PUSHNULL,                      // push null value on top of stack
    YOP_PUSHTHIS,                      // push 'this' value on top of stack
    YOP_PUSHLBLOCK,                    // push reference to local (to module) block

    YOP_PUSH_KI,                       // push constant int ki[X] on top of stack
    YOP_PUSH_KS,                       // push constant string ks[X] on top of stack

    YOP_INDEX,                         // pops [array,index], pushes array[index]. if X, push lvalue instead rvalue

    YOP_POP,                           // pop value stack X times

    YOP_CALL,                          // calls function named ks[X], using the current frame's stack pushes as args
    YOP_RET,                           // leave current call, returning X items on the stack
    YOP_KEEP,                          // Pad/trim most recent RET to X args

    YOP_START,                         // no-op, marks the beginning of a loop for yapFrameReset
    YOP_AND,                           // skips over the next X ops if top of stack is false (skipping leaves value on stack)
    YOP_OR,                            // skips over the next X ops if top of stack is true  (skipping leaves value on stack)

    YOP_IF,                            // pop value, then X+1 LBLOCKs. run 0 if true, 1 if false and present
    YOP_ENTER,                         // pops LBLOCK, pushes frame and enters new scope (a non-function CALL)
    YOP_LEAVE,                         // pop frame (non-function). if X=1, pop value and test first
    YOP_BREAK,                         // pop/finds LOOP frame. if 1, reset frame and ip. if 0, pop loop.
                                       // (BREAK 0 = break, BREAK 1 = continue)

    YOP_ADD,                           // pops [a,b], pushes a+b
    YOP_SUB,                           // pops [a,b], pushes a-b
    YOP_MUL,                           // pops [a,b], pushes a*b
    YOP_DIV,                           // pops [a,b], pushes a/b

    YOP_TOSTRING,                      // type conversion: string
    YOP_TOINT,                         // type conversion: int

    YOP_NOT,                           // pops [a], pushes ![a] (bool value)

    YOP_FORMAT,                        // string format (%)

    YOP_VARREG_KS,                     // register local variable named ks[X], push ref on value stack
    YOP_VARREF_KS,                     // find variable named ks[X], push ref on value stack
    YOP_REFVAL,                        // replace ref at top of stack with its value
    YOP_SETVAR,                        // pops [ref,v], sets *ref = v. if X, don't actually pop v

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
