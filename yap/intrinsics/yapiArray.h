#ifndef YAPINTRINSICS_H
#define YAPINTRINSICS_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapVM;

// ---------------------------------------------------------------------------

yU32 array_push(struct yapVM *vm, yU32 argCount);
yU32 array_length(struct yapVM *vm, yU32 argCount);

// ---------------------------------------------------------------------------

void yapIntrinsicsRegister(struct yapVM *vm);

#endif
