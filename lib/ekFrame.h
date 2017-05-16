// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKFRAME_H
#define EKFRAME_H

#include "ekTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekMap;
struct ekValue;

// ---------------------------------------------------------------------------

// Argument Values
enum
{
    EAV_ALL_ARGS = (-1)
};

enum
{
    EFT_ANY = 0, // "any type", used in function calls

    EFT_CHUNK = (1 << 0), // Chunk boundary (used for recovery)
    EFT_FUNC  = (1 << 1), // Function scope
    EFT_COND  = (1 << 2), // Conditional (if/else) scope
    EFT_LOOP  = (1 << 3), // Loop (while/for) scope
    EFT_SCOPE = (1 << 4), // arbitrary scope not tied to anything specific

    EFT_COUNT
};

typedef struct ekFrame
{
    struct ekMap * locals;
    struct ekBlock * block;
    struct ekOp * ip;         // Instruction Pointer
    struct ekValue * thisVal; // In function scope, it is "this"
    struct ekValue * closure; // to resolve variables after locals but before globals
    ekU32 type;               // EFT_*
    ekU32 prevStackCount;     // Remembers the stack count when the frame was pushed; used for recovery
    ekU32 argCount;           // Number of args available on the stack when entering the frame; used by varargs
    ekU32 cleanupCount;       // Number of stack items to cleanup when this frame is popped
} ekFrame;

ekFrame * ekFrameCreate(struct ekContext * E, ekU32 type, struct ekValue * thisVal, struct ekBlock * block, ekU32 prevStackCount, ekU32 argCount, struct ekValue * closure);
void ekFrameReset(struct ekContext * E, ekFrame * frame, ekBool jumpToStart);
void ekFrameDestroy(struct ekContext * E, ekFrame * frame);

#endif // ifndef EKFRAME_H
