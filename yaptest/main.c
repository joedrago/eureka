#include "yapContext.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
#ifdef PLATFORM_WIN32
    //_CrtSetBreakAlloc(84);
#endif
    {
        yapContext *context = yapContextCreate();
        yapVMCall(context->vm, "main", 0);
        if(yapContextGetError(context))
        {
            printf("VM Bailed out: %s\n", yapContextGetError(context));
        }
        yapContextFree(context);
    }
#ifdef PLATFORM_WIN32
    _CrtDumpMemoryLeaks();
#endif
	return 0;
}
