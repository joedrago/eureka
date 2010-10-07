#include "yapFrame.h"

#include "yapBlock.h"
#include "yapOp.h"

yapFrame * yapFrameCreate(yU32 type, struct yapBlock *block, yS32 bp)
{
    yapFrame *frame = (yapFrame*)yapAlloc(sizeof(yapFrame));
    frame->type = type;
    frame->block = block;
    frame->bp = bp;
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
    // Clearing the stack to bp should not be necessary here, as non-func blocks have no args
}

void yapFrameDestroy(yapFrame *frame)
{
    yapFrameReset(frame, yFalse);
    yapFree(frame);
}

