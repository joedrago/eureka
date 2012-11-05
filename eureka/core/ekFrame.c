// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekFrame.h"

#include "ekBlock.h"
#include "ekContext.h"
#include "ekMap.h"
#include "ekOp.h"

ekFrame *ekFrameCreate(struct ekContext *Y, ekU32 type, struct ekValue *thisVal, struct ekBlock *block, ekU32 prevStackCount, ekU32 argCount, ekValue *closure)
{
    ekFrame *frame = (ekFrame *)ekAlloc(sizeof(ekFrame));
    frame->locals = ekMapCreate(Y, YMKT_STRING);
    frame->type = type;
    frame->thisVal = thisVal;
    frame->block = block;
    frame->prevStackCount = prevStackCount;
    frame->argCount = argCount;
    frame->closure = closure;
    frame->cleanupCount = 0;
    ekValueAddRefNote(Y, frame->thisVal, "this ptr");
    if(frame->closure)
    {
        ekValueAddRefNote(Y, frame->closure, "closure ptr");
    }
    ekFrameReset(Y, frame, ekFalse);
    return frame;
}

void ekFrameReset(struct ekContext *Y, ekFrame *frame, ekBool jumpToStart)
{
    ekMapClear(Y, frame->locals, ekValueRemoveRefHashed);
    frame->ip = (frame->block) ? frame->block->ops : NULL;
    if(frame->ip && jumpToStart)
    {
        while(frame->ip->opcode != YOP_START)
        {
            frame->ip++;
        }
    }
}

void ekFrameDestroy(struct ekContext *Y, ekFrame *frame)
{
    ekFrameReset(Y, frame, ekFalse);
    ekValueRemoveRefNote(Y, frame->thisVal, "this ptr done");
    if(frame->closure)
    {
        ekValueRemoveRefNote(Y, frame->closure, "closure ptr done");
    }
    ekMapDestroy(Y, frame->locals, NULL);
    ekFree(frame);
}

