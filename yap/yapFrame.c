#include "yapFrame.h"

#include "yapBlock.h"
#include "yapOp.h"

yapFrame * yapFrameCreate(yU32 type, struct yapBlock *block, struct yapValue *thisVal, yU32 flags)
{
    yapFrame *frame = (yapFrame*)yapAlloc(sizeof(yapFrame));
    frame->type = type;
    frame->block = block;
    frame->thisVal = thisVal;
    frame->flags = flags;
    yapFrameReset(frame, yFalse);
    return frame;
}

void yapFrameReset(yapFrame *frame, yBool jumpToStart)
{
    yapArrayClear(&frame->variables, NULL);
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
    yapFree(frame);
}

