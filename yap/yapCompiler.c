#include "yapCompiler.h"

#include "yapLexer.h"
#include "yapModule.h"
#include "yapParser.h"

#include <stdlib.h>
#include <stdio.h>

void *yapParseAlloc();
void yapParse(void *yyp, int id, yapToken token, yapCompiler *compiler);
void yapParseFree(void *p);

void yapCompilerDestroy(yapCompiler *compiler)
{
    yapFree(compiler);
}

yBool yapCompile(yapCompiler *compiler, const char *text)
{
    yapToken emptyToken = {0};
    void *parser = yapParseAlloc();

    compiler->module = yapModuleCreate();

    yapLex(parser, text, yapParse, compiler);
    yapParse(parser, 0, emptyToken, compiler);

    //if(compiler->list)
    //{
    //    int i;
    //    for(i=0;i<compiler->list->count;i++)
    //    {
    //        char *identifier = (char*)compiler->list->data[i];
    //        printf("Identifier: %s\n", identifier);
    //    }
    //}

    yapParseFree(parser);
    return yTrue;
}
