// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yap.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------

#ifdef HAVE_READLINE
#include <readline.h>
#else
#define LINE_BUFFER_SIZE 4096
static char sLineBuffer[LINE_BUFFER_SIZE];
static char *readline(const char *prompt)
{
    printf("%s", prompt);
    if(fgets(sLineBuffer, LINE_BUFFER_SIZE, stdin))
    {
        int len = strlen(sLineBuffer);
        if(len > 0)
        {
            char *line = (char *)malloc(len+1);
            strcpy(line, sLineBuffer);
            line[len-1] = 0; // remove newline
            return line;
        }
    }
    return NULL;
}
#endif

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

static char *getprompt(yapContext *Y)
{
    static char prompt[1024];
    sprintf(prompt, "\n[chunks: %d] [globals: %d] $ ", (int)yapArraySize(Y, &Y->chunks), Y->globals->count);
    return prompt;
}

// ---------------------------------------------------------------------------

static void printVariable(struct yapContext *Y, void *ignored, yap2HashEntry *entry)
{
    printf("* '%s'\n", entry->keyStr);
}

// ---------------------------------------------------------------------------

int main(int argc, char *argv[])
{
#ifdef PLATFORM_WIN32
    //_CrtSetBreakAlloc(212);
#endif
    {
        yapContext *Y = yapContextCreate(NULL);
        int dump = 0;
        int running = 1;
        char *line;
        int i;

        for(i = 1; i < argc; i++)
        {
            if(argv[i][0] == '-')
            {
                switch(argv[i][1])
                {
                    case 'd':
                        dump = 1;
                        break;
                };
            }
        }

        while(running && (line = readline(getprompt(Y))))
        {
            char *code = NULL;
            if(!strcmp(line, "$globals"))
            {
                yap2HashIterateP1(Y, Y->globals, printVariable, NULL);
            }
            else if(!strncmp(line, "$load ", 6))
            {
                code = loadFile(line + 6);
            }
            else if(!strcmp(line, "$quit"))
            {
                running = 0;
            }
            else
            {
                code = strdup(line);
            }

            if(code)
            {
                int opts = YEO_DEFAULT;
                if(dump)
                {
                    opts = YEO_DUMP;
                }
                yapContextEval(Y, code, opts);
                if(yapContextGetError(Y))
                {
                    printf("Errors:\n%s\n", yapContextGetError(Y));
                    yapContextRecover(Y);
                }
                free(code);
            }

            free(line);
        }
        yapContextDestroy(Y);
    }

#ifdef PLATFORM_WIN32
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}
