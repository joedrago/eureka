// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKOP_H
#define EKOP_H

#include "ekTypes.h"

// Operand Flags (used with INDEX)
enum
{
    EOF_LVALUE   = (1 << 0), // INDEX lookup needs to be lvaluable
    EOF_PUSHOBJ  = (1 << 1), // After the INDEX lookup, push the object that was indexed on top
    EOF_PUSHTHIS = (1 << 2)  // After the INDEX lookup, push 'this' on top
};

// ---------------------------------------------------------------------------
// In the comments below, X is the operand, and the k tables are owned by translation blocks.

enum
{
    EOP_NOP = 0, // does nothing

    EOP_PUSHNULL, // push null value on top of stack
    EOP_PUSHTHIS, // push 'this' value on top of stack
    EOP_PUSHI,    // push ekS32 X on the top of the stack (used for loop inits and increments)

    EOP_PUSH_KB,   // push constant block kb[X] on top of stack
    EOP_PUSH_KI,   // push constant ekS32 ki[X] on top of stack
    EOP_PUSH_KF,   // push constant float kf[X] on top of stack
    EOP_PUSH_BOOL, // push bool on the stack. true if X is true
    EOP_PUSH_KS,   // push constant string ks[X] on top of stack

    EOP_INDEX, // pops [array,index], pushes array[index]. if X, push lvalue instead rvalue

    EOP_DUPE, // pushes the Nth element in the stack on top again (eg. X=0 would dupe the top of the stack)
    EOP_MOVE, // like DUPE, except it moves the Nth element to the top instead of duping it
    EOP_POP,  // pop value stack X times

    EOP_CALL,    // calls function with X args, using the current frame's stack pushes as args
    EOP_RET,     // leave current call, returning X items on the stack
    EOP_CLEANUP, // Increments the current frame's count (by X) of stack elements that must be cleaned up underneath the args during a KEEP
    EOP_KEEP,    // Pad/trim most recent RET to X args, perform "stack cleanup"
    EOP_CLOSE,   // add closure variables to the EVT_BLOCK on top of the stack (harvested from frame stack)

    EOP_START, // no-op, marks the beginning of a loop for ekFrameReset
    EOP_AND,   // skips over the next X ops if top of stack is false (skipping leaves value on stack)
    EOP_OR,    // skips over the next X ops if top of stack is true  (skipping leaves value on stack)

    EOP_IF,       // pop value, then X+1 blocks. run 0 if true, 1 if false and present
    EOP_ENTER,    // pops block, pushes frame and enters new scope (a non-function CALL). X == EFT_*
    EOP_LEAVE,    // pop frame (non-function). if X=1, pop value and test first. if X=2, pop value and check for null (specifically) first.
    EOP_CONTINUE, // resets LOOP frame.
    EOP_BREAK,    // Pops LOOP frame.

    EOP_ADD, // pops [a,b], pushes a+b
    EOP_SUB, // pops [a,b], pushes a-b. Leaves [a,b] on stack if X=1
    EOP_MUL, // pops [a,b], pushes a*b
    EOP_DIV, // pops [a,b], pushes a/b

    EOP_CMP,                // pops [a,b], pushes a<=>b (ekS32)
    EOP_EQUALS,             // pops [a,b], pushes a==b (bool)
    EOP_NOTEQUALS,          // pops [a,b], pushes a!=b (bool)
    EOP_GREATERTHAN,        // pops [a,b], pushes a>b  (bool)
    EOP_GREATERTHANOREQUAL, // pops [a,b], pushes a>=b (bool)
    EOP_LESSTHAN,           // pops [a,b], pushes a<b  (bool)
    EOP_LESSTHANOREQUAL,    // pops [a,b], pushes a<=b (bool)

    EOP_NOT, // pops [a], pushes ![a] (bool value)

    EOP_BITWISE_NOT, // pops [a], pushes ~[a] (ekS32 value)
    EOP_BITWISE_XOR, // pops [a,b], pushes a^b
    EOP_BITWISE_AND, // pops [a,b], pushes a&b
    EOP_BITWISE_OR,  // pops [a,b], pushes a|b
    EOP_SHIFTLEFT,   // pops [a,b], pushes a<<b
    EOP_SHIFTRIGHT,  // pops [a,b], pushes a>>b

    EOP_FORMAT, // string format (%)

    EOP_VREG,     // register local variable named ks[X], push ref on value stack
    EOP_VREF,     // find variable named ks[X], push ref on value stack
    EOP_VVAL,     // find variable named ks[X], push value on value stack
    EOP_REFVAL,   // replace ref at top of stack with its value
    EOP_VSET,     // pops [ref,v], sets *ref = v. if X, don't actually pop v
    EOP_INHERITS, // pops [l,r], pushes bool indicating if l inherits from r
    EOP_VARARGS,  // pops all but X arguments pushed for this frame, adds them to an array, and pushes the array on the stack

    EOP_ARRAY, // creates an array using the top X entries on the stack, pushes the new array
    EOP_MAP,   // creates a map using the top X entries on the stack, pushes the new map

    EOP_ITER // replaces top of the stack with an iterator for that object
};

// ---------------------------------------------------------------------------

typedef struct ekOp
{
    ekOpcode opcode;
    ekOperand operand;
#ifdef EUREKA_DEBUG_SYMBOLS
    ekS32 line;
#endif
} ekOp;

#define ekOpsAlloc(COUNT) ((ekOp *)(ekAlloc(sizeof(ekOp) * COUNT)))
#define ekOpsFree(OPS) ekFree(OPS)

#endif // ifndef EKOP_H
