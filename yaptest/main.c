#include "yapContext.h"
#include "yapVM.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
#ifdef PLATFORM_WIN32
    // _CrtSetBreakAlloc(101);
#endif
    {
        yapContext *context = yapContextCreate();
        yapVMLoadModule(context->vm, "main", "code");
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
