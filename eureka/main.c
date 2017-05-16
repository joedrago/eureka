// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ek.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------

#ifdef HAVE_READLINE
#include <readline/readline.h>
#else
#define LINE_BUFFER_SIZE 4096
static char sLineBuffer[LINE_BUFFER_SIZE];
static char * readline(const char * prompt)
{
    printf("%s", prompt);
    if (fgets(sLineBuffer, LINE_BUFFER_SIZE, stdin)) {
        int len = strlen(sLineBuffer);
        if (len > 0) {
            char * line = (char *)malloc(len + 1);
            strcpy(line, sLineBuffer);
            line[len - 1] = 0; // remove newline
            return line;
        }
    }
    return NULL;
}
static void add_history(const char * line) {}
#endif

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

    fprintf(stderr, "cant open '%s' for read\n", filename);
    return NULL;
}

// ---------------------------------------------------------------------------

static char * getprompt(ekContext * E)
{
    static char prompt[1024];
    sprintf(prompt, "\n[chunks: %d] [globals: %d] $ ", (int)ekArraySize(E, &E->chunks), E->globals->count);
    return prompt;
}

// ---------------------------------------------------------------------------

static void printVariable(struct ekContext * E, void * ignored, ekMapEntry * entry)
{
    printf("* '%s'\n", entry->keyStr);
}

// ---------------------------------------------------------------------------

int main(int argc, char * argv[])
{
#ifdef PLATFORM_WIN32
    //_CrtSetBreakAlloc(212);
#endif
    {
        ekContext * E = ekContextCreate(NULL);
        ekValue * ARGV = ekValueCreateArray(E);
        const char * script = NULL;
        char * code = NULL;
        int evalOpts = EEO_DEFAULT;
        int repl = 0; // interested in a read-eval-print loop (REPL)
        char * line;
        int i;
        char * c;

        for (i = 1; i < argc; ++i) {
            if (argv[i][0] == '-') {
                for (c = argv[i] + 1; *c; ++c) {
                    switch (*c) {
                        case 'c':
                            evalOpts |= EEO_COMPILE;
                            break;
                        case 'o':
                            evalOpts |= EEO_OPTIMIZE;
                            break;
                        case 'd':
                            evalOpts |= EEO_DUMP;
                            break;
                        case 'r':
                            repl = 1;
                            break;
                    }
                }
            } else {
                script = argv[i];
                ++i;
                break;
            }
        }
        for (; i < argc; ++i) {
            ekValueArrayPush(E, ARGV, ekValueCreateString(E, argv[i]));
        }
        ekContextAddGlobal(E, "ARGV", ARGV);

        if (!script && !repl) {
            // No script to run, no request for a read-eval-print loop.
            // Clue them in on their mistake and bail out.

            fprintf(stderr, "Syntax: %s [-d] [-o] [-r] [file.ek]\n", argv[0]);
        } else {
            if (script) {
                code = loadFile(script);
                if (code) {
                    ekContextEval(E, script, code, evalOpts, NULL);
                    if (ekContextGetError(E)) {
                        printf("Errors:\n%s\n", ekContextGetError(E));
                        ekContextRecover(E);
                    }
                    free(code);
                    code = NULL;
                }
            }

            while (repl && (line = readline(getprompt(E)))) {
                if (line[0]) {
                    add_history(line);
                }
                if (!strcmp(line, "$globals")) {
                    ekMapIterateP1(E, E->globals, printVariable, NULL);
                } else if (!strncmp(line, "$load ", 6))    {
                    code = loadFile(line + 6);
                } else if (!strcmp(line, "$quit"))    {
                    repl = 0;
                } else {
                    code = strdup(line);
                }

                if (code) {
                    ekValue * result = ekValueCreateArray(E);
                    ekContextEval(E, NULL, code, evalOpts, result);
                    if (ekContextGetError(E)) {
                        printf("Errors:\n%s\n", ekContextGetError(E));
                        ekContextRecover(E);
                    } else {
                        int i;
                        for (i = 0; i < ekArraySize(E, &result->arrayVal); ++i) {
                            ekDumpParams * dump = ekDumpParamsCreate(E);
                            ekValueDump(E, dump, result->arrayVal[i]);
                            printf("=> %s\n", ekStringSafePtr(&dump->output));
                            ekDumpParamsDestroy(E, dump);
                        }
                    }
                    ekValueRemoveRefNote(E, result, "eval result done");
                    free(code);
                    code = NULL;
                }

                free(line);
            }
        }
        ekContextDestroy(E);
    }

#ifdef PLATFORM_WIN32
    //_CrtDumpMemoryLeaks();
#endif
    return 0;
}
