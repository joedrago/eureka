#include "yapBlock.h"

void yapBlockFree(yapBlock *block)
{
    yapFree(block->ops);
    yapFree(block);
}
