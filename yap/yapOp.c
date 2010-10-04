#include "yapOp.h"

yapOp *yapOpsMerge(yapOp *dst, int dstCount, yapOp *src, int srcCount)
{
    dst = yapRealloc(dst, sizeof(yapOp) * (dstCount+srcCount));
    memcpy(&dst[dstCount], src, srcCount*sizeof(yapOp));
    return dst;
}
