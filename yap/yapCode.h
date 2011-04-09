#ifndef YAPCODE_H
#define YAPCODE_H

#include "yapTypes.h"

typedef struct yapCode
{
    struct yapOp *ops;
    yS32 size;
    yS32 count;
} yapCode;

#define yapCodeCreate() ((yapCode*)yapAlloc(sizeof(yapCode)))
void yapCodeDestroy(yapCode *code);

void yapCodeGrow(yapCode *code, int count);
yS32 yapCodeAppend(yapCode *code, yOpcode opcode, yOperand operand);

#endif
