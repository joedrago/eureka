// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ek.h"

#ifdef EUREKA_ENABLE_EXT_DOT
#include "ekxDot.h"
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
#ifdef EUREKA_TRACE_MEMORY
    ekMemoryStatsReset();
#endif
    {
        char *error = NULL;
        ekContext *Y = ekContextCreate(NULL);
        ekContextEval(Y, code, YEO_DUMP);
        if(ekContextGetError(Y))
        {
            error = strdup(ekContextGetError(Y));
            ret |= FAIL_EVAL;
        }
        ekContextDestroy(Y);
        if(error)
        {
            printf("VM Bailed out: %s\n", error);
        }
        free(error);
    }
#ifdef EUREKA_TRACE_MEMORY
    ekMemoryStatsPrint("ekChunk End: ");
    if(ekMemoryStatsLeftovers() > 0)
    {
        ret |= FAIL_MEMORY;
    }
#endif
#ifdef EUREKA_TRACE_REFS
    if(ekValueDebugCount() != 0)
    {
        printf("Leftover ekValues: %d\n", ekValueDebugCount());
        ret |= FAIL_REFS;
    }
#endif
    return ret;
}

// ---------------------------------------------------------------------------

void outputDot(const char *code)
{
#ifdef EUREKA_ENABLE_EXT_DOT
    ekContext *Y = ekContextCreate(NULL);
    ekCompiler *compiler = ekCompilerCreate(Y);
    ekCompile(compiler, code, YCO_KEEP_SYNTAX_TREE);
    if(ekArraySize(Y, &compiler->errors))
    {
        int i;
        for(i = 0; i < ekArraySize(Y, &compiler->errors); i++)
        {
            char *error = (char *)compiler->errors[i];
            fprintf(stderr, "Error: %s\n", error);
        }
    }
    if(compiler->root)
    {
        ekSyntaxDot(Y, compiler->root);
    }
    else
    {
        printf("ERROR: Failed to build syntax tree\n");
    }
    ekCompilerDestroy(compiler);
    ekContextDestroy(Y);
#else
    printf("Dot support is disabled! (EUREKA_ENABLE_EXT_DOT)\n");
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

// Eureka Test Mode
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
            printf("ektest [filename.ek]\n");
        }
    }

#ifdef PLATFORM_WIN32
    _CrtDumpMemoryLeaks();
#endif
    return ret;
}