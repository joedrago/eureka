// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapFrame.h"

#include "yapBlock.h"
#include "yapContext.h"
#include "yapHash.h"
#include "yapOp.h"

yapFrame *yapFrameCreate(struct yapContext *Y, yU32 type, struct yapValue *thisVal, struct yapBlock *block, yU32 prevStackCount, yU32 argCount, yapValue *closure)
{
    yapFrame *frame = (yapFrame *)yapAlloc(sizeof(yapFrame));
    frame->locals = yap2HashCreate(Y, KEYTYPE_STRING, 0);
    frame->type = type;
    frame->thisVal = thisVal;
    frame->block = block;
    frame->prevStackCount = prevStackCount;
    frame->argCount = argCount;
    frame->closure = closure;
    frame->cleanupCount = 0;
    yapValueAddRefNote(Y, frame->thisVal, "this ptr");
    if(frame->closure)
    {
        yapValueAddRefNote(Y, frame->closure, "closure ptr");
    }
    yapFrameReset(Y, frame, yFalse);
    return frame;
}

void yapFrameReset(struct yapContext *Y, yapFrame *frame, yBool jumpToStart)
{
    yap2HashClear(Y, frame->locals, yapValueRemoveRefHashed);
    frame->ip = (frame->block) ? frame->block->ops : NULL;
    if(frame->ip && jumpToStart)
    {
        while(frame->ip->opcode != YOP_START)
        {
            frame->ip++;
        }
    }
}

void yapFrameDestroy(struct yapContext *Y, yapFrame *frame)
{
    yapFrameReset(Y, frame, yFalse);
    yapValueRemoveRefNote(Y, frame->thisVal, "this ptr done");
    if(frame->closure)
    {
        yapValueRemoveRefNote(Y, frame->closure, "closure ptr done");
    }
    yap2HashDestroy(Y, frame->locals, NULL);
    yapFree(frame);
}

