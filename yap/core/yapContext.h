// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPCONTEXT_H
#define YAPCONTEXT_H

#include "yapTypes.h"

typedef struct yapContext
{
    struct yapVM *vm;
} yapContext;

yapContext *yapContextCreate(void);
void yapContextFree(yapContext *context);

const char *yapContextGetError(yapContext *context);

#endif
