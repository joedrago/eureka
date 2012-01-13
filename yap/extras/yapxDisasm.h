#ifndef YAPXDOT_H
#define YAPXDOT_H

// ---------------------------------------------------------------------------
// Forwards

struct yapChunk;
struct yapOp;

// ---------------------------------------------------------------------------

void yapOpsDump(struct yapOp *ops, int count);

void yapChunkDump(struct yapChunk *chunk);

#endif
