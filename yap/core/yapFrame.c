// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapFrame.h"

#include "yapBlock.h"
#include "yapOp.h"

yapFrame *yapFrameCreate(yU32 type, struct yapValue *thisVal, struct yapBlock *block, yU32 prevStackCount, yU32 argCount)
{
    yapFrame *frame = (yapFrame *)yapAlloc(sizeof(yapFrame));
    frame->locals = yapHashCreate(0);
    frame->type = type;
    frame->thisVal = thisVal;
    frame->block = block;
    frame->prevStackCount = prevStackCount;
    frame->argCount = argCount;
    frame->cleanupCount = 0;
    yapFrameReset(frame, yFalse);
    return frame;
}

void yapFrameReset(yapFrame *frame, yBool jumpToStart)
{
    yapHashClear(frame->locals, NULL);
    frame->ip = (frame->block) ? frame->block->ops : NULL;
    if(frame->ip && jumpToStart)
    {
        while(frame->ip->opcode != YOP_START)
            frame->ip++;
    }
}

void yapFrameDestroy(yapFrame *frame)
{
    yapFrameReset(frame, yFalse);
    yapHashDestroy(frame->locals, NULL);
    yapFree(frame);
}

