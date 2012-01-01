#include "yapContext.h"

#include "yapIntrinsics.h"
#include "yapVM.h"

yapContext *yapContextCreate(void)
{
    yapContext *context;
#ifdef YAP_ENABLE_MEMORY_STATS
    yapMemoryStatsPrint("yapContextCreate: ");
#endif
    context = yapAlloc(sizeof(yapContext));
    context->vm = yapVMCreate();
    yapValueTypeRegisterAllBasicTypes(context->vm);
    yapIntrinsicsRegister(context->vm);
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
