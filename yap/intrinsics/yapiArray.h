// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

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
