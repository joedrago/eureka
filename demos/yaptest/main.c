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
#include <string.h>

// ---------------------------------------------------------------------------

enum
{
    FAIL_EVAL   = (1 << 0),
    FAIL_MEMORY = (1 << 1),
    FAIL_REFS   = (1 << 2)
};

int loadChunk(const char *code)
{
    int ret = 0; // 0 for success
#ifdef YAP_TRACE_MEMORY
    yapMemoryStatsReset();
#endif
    {
        char *error = NULL;
        yapContext *Y = yapContextCreate(NULL);
        yapContextEval(Y, code, YEO_DUMP);
        if(yapContextGetError(Y))
        {
            error = strdup(yapContextGetError(Y));
            ret |= FAIL_EVAL;
        }
        yapContextDestroy(Y);
        if(error)
        {
            printf("VM Bailed out: %s\n", error);
        }
        free(error);
    }
#ifdef YAP_TRACE_MEMORY
    yapMemoryStatsPrint("yapChunk End: ");
    if(yapMemoryStatsLeftovers() > 0)
    {
        ret |= FAIL_MEMORY;
    }
#endif
#ifdef YAP_TRACE_REFS
    if(yapValueDebugCount() != 0)
    {
        printf("Leftover yapValues: %d\n", yapValueDebugCount());
        ret |= FAIL_REFS;
    }
#endif
    return ret;
}

// ---------------------------------------------------------------------------

void outputDot(const char *code)
{
#ifdef YAP_ENABLE_EXT_DOT
    yapContext *Y = yapContextCreate(NULL);
    yapCompiler *compiler = yapCompilerCreate(Y);
    yapCompile(compiler, code, YCO_KEEP_SYNTAX_TREE);
    if(yapArraySize(Y, &compiler->errors))
    {
        int i;
        for(i = 0; i < yapArraySize(Y, &compiler->errors); i++)
        {
            char *error = (char *)compiler->errors[i];
            fprintf(stderr, "Error: %s\n", error);
        }
    }
    if(compiler->root)
    {
        yapSyntaxDot(Y, compiler->root);
    }
    else
    {
        printf("ERROR: Failed to build syntax tree\n");
    }
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
    int ret = 0;
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
                        ret = loadChunk(code);
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
    return ret;
}
