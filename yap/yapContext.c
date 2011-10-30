#include "yapContext.h"

#include "yapIntrinsics.h"
#include "yapVM.h"

yapContext *yapContextCreate(void)
{
    yapContext *context = yapAlloc(sizeof(yapContext));
    context->vm = yapVMCreate();
    yapIntrinsicsRegister(context->vm);
    return context;
}

void yapContextFree(yapContext *context)
{
    yapVMDestroy(context->vm);
    yapFree(context);
}

const char *yapContextGetError(yapContext *context)
{
    return context->vm->error;
}
