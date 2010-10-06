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
    yBool success = yFalse;
    yapToken emptyToken = {0};
    void *parser = yapParseAlloc();

    compiler->module = yapModuleCreate();

    yapLex(parser, text, yapParse, compiler);
    yapParse(parser, 0, emptyToken, compiler);

    if(compiler->module->block)
    {
        success = yTrue;
        yapModuleDump(compiler->module);
    };

    yapParseFree(parser);
    return success;
}
