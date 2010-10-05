#include "yapContext.h"
#include "yapVM.h"

#include "yapCompiler.h"

#include <stdio.h>
#include <stdlib.h>

void vmTest(const char *code)
{
    yapContext *context = yapContextCreate();
    yapVMLoadModule(context->vm, "main", code);
    if(yapContextGetError(context))
    {
        printf("VM Bailed out: %s\n", yapContextGetError(context));
    }
    yapContextFree(context);
}

void lexTest()
{
    FILE *f = fopen("test.yap", "rb");
    if(f)
    {
        int size;
        char *buffer;

        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);

        buffer = (char*)malloc(size+1);
        fread(buffer, 1, size, f);
        buffer[size] = 0;

        fclose(f);

        {
            vmTest(buffer);
        }

        free(buffer);
    }
    else
    {
        printf("cant open test.yap\n");
    }
}

int main(int argc, char* argv[])
{
#ifdef PLATFORM_WIN32
    // _CrtSetBreakAlloc(101);
#endif
    {
        //vmTest();
        lexTest();
    }
#ifdef PLATFORM_WIN32
    _CrtDumpMemoryLeaks();
#endif
	return 0;
}
