// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPXDOT_H
#define YAPXDOT_H

// ---------------------------------------------------------------------------
// Forwards

struct yapChunk;
struct yapContext;
struct yapOp;

// ---------------------------------------------------------------------------

void yapOpsDump(struct yapOp *ops, int count);

void yapChunkDump(struct yapContext *Y, struct yapChunk *chunk);

#endif
