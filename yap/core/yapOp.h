// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPOP_H
#define YAPOP_H

#include "yapTypes.h"

// Operand Flags (used with INDEX)
enum
{
    YOF_LVALUE   = (1 << 0), // INDEX lookup needs to be lvaluable
    YOF_PUSHOBJ  = (1 << 1), // After the INDEX lookup, push the object that was indexed on top
    YOF_PUSHTHIS = (1 << 2)  // After the INDEX lookup, push 'this' on top
};

// ---------------------------------------------------------------------------
// In the comments below, X is the operand, and the k tables are owned by translation blocks.

enum
{
    YOP_NOP = 0,                      // does nothing

    YOP_PUSHNULL,                      // push null value on top of stack
    YOP_PUSHTHIS,                      // push 'this' value on top of stack
    YOP_PUSHI,                         // push int X on the top of the stack (used for loop inits and increments)

    YOP_PUSH_KB,                       // push constant block kb[X] on top of stack
    YOP_PUSH_KI,                       // push constant int ki[X] on top of stack
    YOP_PUSH_KF,                       // push constant float kf[X] on top of stack
    YOP_PUSH_KS,                       // push constant string ks[X] on top of stack

    YOP_INDEX,                         // pops [array,index], pushes array[index]. if X, push lvalue instead rvalue

    YOP_DUPE,                          // pushes the Nth element in the stack on top again (eg. X=0 would dupe the top of the stack)
    YOP_MOVE,                          // like DUPE, except it moves the Nth element to the top instead of duping it
    YOP_POP,                           // pop value stack X times

    YOP_CALL,                          // calls function with X args, using the current frame's stack pushes as args
    YOP_RET,                           // leave current call, returning X items on the stack
    YOP_CLEANUP,                       // Increments the current frame's count (by X) of stack elements that must be cleaned up underneath the args during a KEEP
    YOP_KEEP,                          // Pad/trim most recent RET to X args, perform "stack cleanup"
    YOP_CLOSE,                         // add closure variables to the YVT_BLOCK on top of the stack (harvested from frame stack)

    YOP_START,                         // no-op, marks the beginning of a loop for yapFrameReset
    YOP_AND,                           // skips over the next X ops if top of stack is false (skipping leaves value on stack)
    YOP_OR,                            // skips over the next X ops if top of stack is true  (skipping leaves value on stack)

    YOP_IF,                            // pop value, then X+1 blocks. run 0 if true, 1 if false and present
    YOP_ENTER,                         // pops block, pushes frame and enters new scope (a non-function CALL). X == YFT_*
    YOP_LEAVE,                         // pop frame (non-function). if X=1, pop value and test first
    YOP_CONTINUE,                      // resets LOOP frame.
    YOP_BREAK,                         // Pops LOOP frame.

    YOP_ADD,                           // pops [a,b], pushes a+b
    YOP_SUB,                           // pops [a,b], pushes a-b. Leaves [a,b] on stack if X=1
    YOP_MUL,                           // pops [a,b], pushes a*b
    YOP_DIV,                           // pops [a,b], pushes a/b

    YOP_CMP,                           // pops [a,b], pushes a<=>b (int)
    YOP_EQUALS,                        // pops [a,b], pushes a==b (bool)
    YOP_NOTEQUALS,                     // pops [a,b], pushes a!=b (bool)
    YOP_GREATERTHAN,                   // pops [a,b], pushes a>b  (bool)
    YOP_GREATERTHANOREQUAL,            // pops [a,b], pushes a>=b (bool)
    YOP_LESSTHAN,                      // pops [a,b], pushes a<b  (bool)
    YOP_LESSTHANOREQUAL,               // pops [a,b], pushes a<=b (bool)

    YOP_NOT,                           // pops [a], pushes ![a] (bool value)

    YOP_BITWISE_NOT,                   // pops [a], pushes ~[a] (int value)
    YOP_BITWISE_XOR,                   // pops [a,b], pushes a^b
    YOP_BITWISE_AND,                   // pops [a,b], pushes a&b
    YOP_BITWISE_OR,                    // pops [a,b], pushes a|b
    YOP_SHIFTLEFT,                     // pops [a,b], pushes a<<b
    YOP_SHIFTRIGHT,                    // pops [a,b], pushes a>>b

    YOP_FORMAT,                        // string format (%)

    YOP_VARREG_KS,                     // register local variable named ks[X], push ref on value stack
    YOP_VARREF_KS,                     // find variable named ks[X], push ref on value stack
    YOP_REFVAL,                        // replace ref at top of stack with its value
    YOP_SETVAR,                        // pops [ref,v], sets *ref = v. if X, don't actually pop v
    YOP_INHERITS,                      // pops [ref,v], creates a new ref to a stub object if ref=null, sets *ref to inherit from v. if X, don't actually pop v
    YOP_VARARGS,                       // pops all but X arguments pushed for this frame, adds them to an array, and pushes the array on the stack

    YOP_NTH,                           // pops [object, index] and attempts to push the Nth value either via a call to object.get(n) or array[n]
    YOP_COUNT                          // pops value, returns either array length and sets lastRet=1, or performs call to 'count' on obj
};

// ---------------------------------------------------------------------------

typedef struct yapOp
{
    yOpcode  opcode;
    yOperand operand;
#ifdef YAP_DEBUGGING
    int line;
#endif
} yapOp;

#define yapOpsAlloc(COUNT) ((yapOp*)(yapAlloc(sizeof(yapOp) * COUNT)))
#define yapOpsFree(OPS) yapFree(OPS)

#endif
