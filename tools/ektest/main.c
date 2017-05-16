// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ek.h"

#include "ekxAst.h"
#include "ekxDot.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------

enum
{
    FAIL_EVAL   = (1 << 0),
    FAIL_MEMORY = (1 << 1)
};

int loadChunk(const char * sourcePath, const char * code, ekU32 evalFlags)
{
    int ret = 0; // 0 for success
#ifdef EUREKA_TRACE_MEMORY
    ekMemoryStatsReset();
#endif
    {
        char * error = NULL;
        ekContext * E = ekContextCreate(NULL);
        ekContextEval(E, sourcePath, code, evalFlags, NULL);
        if (ekContextGetError(E)) {
            error = strdup(ekContextGetError(E));
            ret |= FAIL_EVAL;
        }
        ekContextDestroy(E);
        if (error) {
            printf("%s\n", error);
        }
        free(error);
    }
#ifdef EUREKA_TRACE_MEMORY
    ekMemoryStatsPrint("ekChunk End: ");
    if (ekMemoryStatsLeftovers() > 0) {
        ekMemoryStatsDumpLeaks();
        ret |= FAIL_MEMORY;
    }
#endif
    return ret;
}

// ---------------------------------------------------------------------------

void outputAst(const char * sourcePath, const char * code, ekU32 evalFlags)
{
#ifdef EUREKA_ENABLE_EXT_AST
    ekU32 compileFlags = ECO_KEEP_SYNTAX_TREE;
    ekContext * E = ekContextCreate(NULL);
    ekCompiler * compiler = ekCompilerCreate(E);
    ekString err = { 0 };
    if (evalFlags & EEO_OPTIMIZE) {
        compileFlags |= ECO_OPTIMIZE;
    }
    ekCompile(compiler, sourcePath, code, compileFlags);
    if (ekCompilerFormatErrors(compiler, &err)) {
        fprintf(stderr, "%s", ekStringSafePtr(&err));
    }
    ekStringClear(E, &err);
    if (compiler->root) {
        ekSyntaxAst(E, compiler->root);
    }
    ekCompilerDestroy(compiler);
    ekContextDestroy(E);
#else
    printf("AST dump support is disabled! (EUREKA_ENABLE_EXT_AST)\n");
#endif
}

// ---------------------------------------------------------------------------

void outputGraph(const char * sourcePath, const char * code, ekU32 evalFlags)
{
#ifdef EUREKA_ENABLE_EXT_DOT
    ekU32 compileFlags = ECO_KEEP_SYNTAX_TREE;
    ekContext * E = ekContextCreate(NULL);
    ekCompiler * compiler = ekCompilerCreate(E);
    ekString err = { 0 };
    if (evalFlags & EEO_OPTIMIZE) {
        compileFlags |= ECO_OPTIMIZE;
    }
    ekCompile(compiler, sourcePath, code, compileFlags);
    if (ekCompilerFormatErrors(compiler, &err)) {
        fprintf(stderr, "%s", ekStringSafePtr(&err));
    }
    ekStringClear(E, &err);
    if (compiler->root) {
        ekSyntaxDot(E, compiler->root);
    }
    ekCompilerDestroy(compiler);
    ekContextDestroy(E);
#else
    printf("Dot support is disabled! (EUREKA_ENABLE_EXT_DOT)\n");
#endif
}

// ---------------------------------------------------------------------------

char * loadFile(const char * filename)
{
    FILE * f = fopen(filename, "rb");
    if (f) {
        int size;
        char * buffer;

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
    ETM_LOADCHUNK,
    ETM_AST,
    ETM_GRAPH
};

int main(int argc, char * argv[])
{
    int ret = 0;
#ifdef PLATFORM_WIN32
    //_CrtSetBreakAlloc(212);
#endif
    {
        int mode = ETM_LOADCHUNK;
        int i;
        ekU32 evalFlags = 0;
        char * filename = NULL;
        for (i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                switch (argv[i][1]) {
                    case 'd':
                        evalFlags |= EEO_DUMP;
                        break;
                    case 'o':
                        evalFlags |= EEO_OPTIMIZE;
                        break;
                    case 'a':
                        mode = ETM_AST;
                        break;
                    case 'g':
                        mode = ETM_GRAPH;
                        break;
                }
            }
            filename = argv[i];
        }

        if (filename) {
            char * code = loadFile(filename);
            if (code) {
                switch (mode) {
                    case ETM_LOADCHUNK:
                        ret = loadChunk(filename, code, evalFlags);
                        break;
                    case ETM_AST:
                        outputAst(filename, code, evalFlags);
                        break;
                    case ETM_GRAPH:
                        outputGraph(filename, code, evalFlags);
                        break;
                }
                free(code);
            }
        } else {
            printf("ektest [-a] [-d] [-g] [-o] [filename.ek]\n");
        }
    }

#ifdef PLATFORM_WIN32
    // _CrtDumpMemoryLeaks();
#endif
    return ret;
}
