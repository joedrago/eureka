#ifndef YAPFRAME_H
#define YAPFRAME_H

#include "yapTypes.h"

enum
{
    YFT_ANY = 0,                       // "any type", used in function calls

    YFT_FUNC,                          // Function scope
    YFT_COND,                          // Conditional (if/else) scope
    YFT_LOOP,                          // Loop (while/for) scope

    YFT_COUNT
};

typedef struct yapFrame
{
    yapArray variables;
    struct yapBlock *block;
    struct yapOp *ip;                  // Instruction Pointer
    yS32   bp;                         // Base pointer (prev. stack depth minus arg count)
    yU32 type;                         // YFT_*
} yapFrame;

yapFrame * yapFrameCreate(yU32 type, struct yapBlock *block, yS32 bp);
void yapFrameReset(yapFrame *frame, yBool jumpToStart);
void yapFrameDestroy(yapFrame *frame);

#endif
