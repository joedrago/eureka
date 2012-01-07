#ifndef YAPFRAME_H
#define YAPFRAME_H

#include "yapArray.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapValue;

// ---------------------------------------------------------------------------

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
    yapArray variables;
    struct yapBlock *block;
    struct yapOp *ip;                  // Instruction Pointer
    struct yapValue *thisVal;          // In function scope, it is "this"
    yU32 type;                         // YFT_*
    yU32 prevStackCount;               // Remembers the stack count when the frame was pushed; used for recovery
} yapFrame;

yapFrame *yapFrameCreate(yU32 type, struct yapBlock *block, yU32 prevStackCount, struct yapValue *thisVal);
void yapFrameReset(yapFrame *frame, yBool jumpToStart);
void yapFrameDestroy(yapFrame *frame);

#endif
