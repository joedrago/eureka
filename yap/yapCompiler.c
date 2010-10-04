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

void yapNuggetGrowOps(yapNugget *nugget, int count)
{
    if(nugget->count)
        nugget->ops = yapRealloc(nugget->ops, sizeof(yapOp) * (nugget->count+count));
    else
        nugget->ops = yapAlloc(sizeof(yapOp) * count);
    nugget->count += count;
}

void yapNuggetAppendOp(yapNugget *nugget, yOpcode opcode, yOperand operand)
{
    yapOp *op = &nugget->ops[nugget->count];
    op->opcode  = opcode;
    op->operand = operand;
    nugget->count++;
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
