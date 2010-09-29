#include "yapFrame.h"

void yapFrameDestroy(yapFrame *frame)
{
    yapArrayClear(&frame->ret,  NULL);
    yapArrayClear(&frame->args, NULL);
    yapArrayClear(&frame->variables, NULL);
    yapFree(frame);
}
