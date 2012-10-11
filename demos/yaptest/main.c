// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yap.h"

#ifdef YAP_ENABLE_EXT_DOT
#include "yapxDot.h"
#endif

#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------

void loadChunk(const char *code)
{
#ifdef YAP_TRACE_MEMORY
    yapMemoryStatsReset();
#endif
    {
        yapContext *Y = yapContextCreate(NULL);
        yapContextEval(Y, code, YEO_DUMP);
        if(yapContextGetError(Y))
        {
            printf("VM Bailed out: %s\n", yapContextGetError(Y));
        }
        yapContextDestroy(Y);
    }
#ifdef YAP_TRACE_MEMORY
    yapMemoryStatsPrint("yapChunk End: ");
#endif
}

// ---------------------------------------------------------------------------

void outputDot(const char *code)
{
#ifdef YAP_ENABLE_EXT_DOT
    yapContext *Y = yapContextCreate(NULL);
    yapCompiler *compiler = yapCompilerCreate(Y);
    yapCompile(compiler, code, YCO_KEEP_SYNTAX_TREE);
    if(compiler->errors.count)
    {
        int i;
        for(i = 0; i < compiler->errors.count; i++)
        {
            char *error = (char *)compiler->errors.data[i];
            fprintf(stderr, "Error: %s\n", error);
        }
    }
    if(compiler->root)
        yapSyntaxDot(compiler->root);
    else
        printf("ERROR: Failed to build syntax tree\n");
    yapCompilerDestroy(compiler);
    yapContextDestroy(Y);
#else
    printf("Dot support is disabled! (YAP_ENABLE_EXT_DOT)\n");
#endif
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
