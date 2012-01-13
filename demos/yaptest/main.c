#include "yapCompiler.h"
#include "yapContext.h"
#include "yapChunk.h"
#include "yapxDisasm.h"
#include "yapxDot.h"
#include "yapVM.h"
#include "yapHash.h"

#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------

void loadChunk(const char *code)
{
    yapContext *context = yapContextCreate();
    yapVMEval(context->vm, code, YEO_DUMP);
    if(yapContextGetError(context))
    {
        printf("VM Bailed out: %s\n", yapContextGetError(context));
    }
    yapContextFree(context);
}

// ---------------------------------------------------------------------------

void outputDot(const char *code)
{
    yapCompiler *compiler = yapCompilerCreate();
    yapCompile(compiler, code, YCO_KEEP_SYNTAX_TREE);
    if(compiler->root)
        yapSyntaxDot(compiler->root);
    else
        printf("ERROR: Failed to build syntax tree\n");
    yapCompilerDestroy(compiler);
}

// ---------------------------------------------------------------------------

char *loadFile(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if(f)
    {
        int size;
        char *buffer;

        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);

        buffer = (char *)malloc(size + 1);
        fread(buffer, 1, size, f);
        buffer[size] = 0;

        fclose(f);

        return buffer;
    }

    printf("cant open '%s' for read\n", filename);
    return NULL;
}

// ---------------------------------------------------------------------------

// Yap Test Mode
enum
{
    YTM_LOADCHUNK,
    YTM_DOT
};

int main(int argc, char *argv[])
{
#ifdef PLATFORM_WIN32
    //_CrtSetBreakAlloc(212);
#endif
    {
        int mode = YTM_LOADCHUNK;
        int i;
        char *filename = NULL;
        for(i = 1; i < argc; i++)
        {
            if(argv[i][0] == '-')
            {
                switch(argv[i][1])
                {
                case 'd':
                    mode = YTM_DOT;
                    break;
                };
            }
            filename = argv[i];
        }

        if(filename)
        {
            char *code = loadFile(filename);
            if(code)
            {
                switch(mode)
                {
                case YTM_LOADCHUNK:
                    loadChunk(code);
                    break;
                case YTM_DOT:
                    outputDot(code);
                    break;
                };
                free(code);
            }
        }
        else
        {
            printf("yaptest [filename.yap]\n");
        }
    }

#ifdef PLATFORM_WIN32
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}