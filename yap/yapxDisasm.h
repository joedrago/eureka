#ifndef YAPXDOT_H
#define YAPXDOT_H

// ---------------------------------------------------------------------------
// Forwards

struct yapModule;
struct yapOp;

// ---------------------------------------------------------------------------

void yapOpsDump(struct yapOp *ops, int count);

void yapModuleDump(struct yapModule *module);

#endif
