#include "yapCore.h"

yapContext * yapContextCreate(void)
{
    yapContext *context = yapAlloc(sizeof(yapContext));
    context->vm = yapVMCreate();
    return context;
}

void yapContextFree(yapContext *context)
{
    yapVMFree(context->vm);
    yapFree(context);
}
