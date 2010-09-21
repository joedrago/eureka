#include "yapCore.h"

yapContext * yapContextCreate(void)
{
    yapContext *context = yapAlloc(sizeof(yapContext));
    context->vm = yapVMCreate();
    return context;
}

void yapContextDestroy(yapContext *context)
{
    yapVMDestroy(context->vm);
    yapFree(context);
}
