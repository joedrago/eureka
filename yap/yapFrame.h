#ifndef YAPFRAME_H
#define YAPFRAME_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapValue;

// ---------------------------------------------------------------------------

// TODO: remove YFT_* and roll it into YFF_* (init funcs would just be (YFF_FUNC|YFF_INIT)

enum
{
    YFT_ANY = 0,                       // "any type", used in function calls

    YFT_FUNC,                          // Function scope
    YFT_COND,                          // Conditional (if/else) scope
    YFT_LOOP,                          // Loop (while/for) scope
    YFT_CLASS,                         // Class declaration scope

    YFT_COUNT
};

// yapFrame flags
enum
{
    YFF_NONE = 0,
    YFF_INIT = (1 << 0)                // Was an init() member function call
};

typedef struct yapFrame
{
    yapArray variables;
    struct yapBlock *block;
    struct yapOp *ip;                  // Instruction Pointer
    struct yapValue *thisVal;          // 'this', as in C++ (non-null in a member function call). Only valid on YFT_FUNC frames.
    yU32 type;                         // YFT_*
    yU32 flags;                        // YFF_*
} yapFrame;

yapFrame * yapFrameCreate(yU32 type, struct yapBlock *block, struct yapValue *thisVal, yU32 flags);
void yapFrameReset(yapFrame *frame, yBool jumpToStart);
void yapFrameDestroy(yapFrame *frame);

#endif
