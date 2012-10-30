// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EUREKAFRAME_H
#define EUREKAFRAME_H

#include "ekTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekMap;
struct ekValue;

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

typedef struct ekFrame
{
    struct ekMap *locals;
    struct ekBlock *block;
    struct ekOp *ip;                  // Instruction Pointer
    struct ekValue *thisVal;          // In function scope, it is "this"
    struct ekValue *closure;          // to resolve variables after locals but before globals
    yU32 type;                         // YFT_*
    yU32 prevStackCount;               // Remembers the stack count when the frame was pushed; used for recovery
    yU32 argCount;                     // Number of args available on the stack when entering the frame; used by varargs
    yU32 cleanupCount;                 // Number of stack items to cleanup when this frame is popped
} ekFrame;

ekFrame *ekFrameCreate(struct ekContext *Y, yU32 type, struct ekValue *thisVal, struct ekBlock *block, yU32 prevStackCount, yU32 argCount, struct ekValue *closure);
void ekFrameReset(struct ekContext *Y, ekFrame *frame, yBool jumpToStart);
void ekFrameDestroy(struct ekContext *Y, ekFrame *frame);

#endif
