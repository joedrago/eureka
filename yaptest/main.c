#include "yapCore.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
    yapContext *context = yapContextCreate();
    yapVMCall(context->vm, "main", 0);
    if(context->vm->error)
    {
        printf("VM Bailed out: %s\n", context->vm->error);
    }
    yapContextDestroy(context);
	return 0;
}
