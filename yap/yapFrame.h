#ifndef YAPFRAME_H
#define YAPFRAME_H

#include "yapTypes.h"

typedef struct yapFrame
{
    yapArray variables;
    struct yapBlock *block;
    struct yapOp *ip;                  // Instruction Pointer
    yS32   bp;                         // Base pointer (prev. stack depth minus arg count)
    yapArray args;                     // Contains arguments to the called function/module
    yapArray ret;                      // Contains returned values from the most recent RET
} yapFrame;

#define yapFrameCreate() ((yapFrame*)yapAlloc(sizeof(yapFrame)))
void yapFrameDestroy(yapFrame *frame);

#endif
