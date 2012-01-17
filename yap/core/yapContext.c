// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapContext.h"
#include "yapVM.h"

yapContext *yapContextCreate(void)
{
    yapContext *context;
#ifdef YAP_ENABLE_MEMORY_STATS
    yapMemoryStatsPrint("yapContextCreate: ");
#endif
    context = yapAlloc(sizeof(yapContext));
    context->vm = yapVMCreate();
    return context;
}

void yapContextFree(yapContext *context)
{
    yapVMDestroy(context->vm);
    yapFree(context);

#ifdef YAP_ENABLE_MEMORY_STATS
    yapMemoryStatsPrint("yapContextFree: ");
    yapMemoryStatsDumpLeaks();
#endif
}

const char *yapContextGetError(yapContext *context)
{
    return context->vm->error;
}
