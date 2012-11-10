// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKXDISASM_H
#define EKXDISASM_H

// ---------------------------------------------------------------------------
// Forwards

struct ekChunk;
struct ekContext;
struct ekOp;

// ---------------------------------------------------------------------------

void ekOpsDump(struct ekOp *ops, int count);

void ekChunkDump(struct ekContext *E, struct ekChunk *chunk);

#endif
