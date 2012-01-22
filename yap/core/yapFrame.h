// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPFRAME_H
#define YAPFRAME_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapHash;
struct yapValue;

// ---------------------------------------------------------------------------

// Argument Values
enum
{
    YAV_ALL_ARGS = (-1)
};

enum
{
    YFT_ANY = 0,                       // "any type", used in function calls

    YFT_CHUNK = (1 << 0),              // Chunk boundary (used for recovery)
    YFT_FUNC  = (1 << 1),              // Function scope
    YFT_COND  = (1 << 2),              // Conditional (if/else) scope
    YFT_LOOP  = (1 << 3),              // Loop (while/for) scope
    YFT_SCOPE = (1 << 4),              // arbitrary scope not tied to anything specific

    YFT_COUNT
};

typedef struct yapFrame
{
    struct yapHash *locals;
    struct yapBlock *block;
    struct yapOp *ip;                  // Instruction Pointer
    struct yapValue *thisVal;          // In function scope, it is "this"
    yU32 type;                         // YFT_*
    yU32 prevStackCount;               // Remembers the stack count when the frame was pushed; used for recovery
    yU32 argCount;                     // Number of args available on the stack when entering the frame; used by varargs
    yU32 cleanupCount;                 // Number of stack items to cleanup when this frame is popped
} yapFrame;

yapFrame *yapFrameCreate(struct yapContext *Y, yU32 type, struct yapValue *thisVal, struct yapBlock *block, yU32 prevStackCount, yU32 argCount);
void yapFrameReset(struct yapContext *Y, yapFrame *frame, yBool jumpToStart);
void yapFrameDestroy(struct yapContext *Y, yapFrame *frame);

#endif
