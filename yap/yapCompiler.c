#include "yapCompiler.h"

#include "yapCode.h"
#include "yapLexer.h"
#include "yapModule.h"
#include "yapParser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void yapCompileSyntaxError(yapCompiler *compiler, const char *token)
{
    if(token)
    {
        char temp[32];
        int len = strlen(token);
        if(len > 31) len = 31;
        memcpy(temp, token, len);
        temp[len] = 0;
        yapTrace(( "syntax error near '%s'\n", temp ));
    }
    compiler->error = yTrue;
}

yapArray * yapCompileIdentifierListCreate(yapCompiler *compiler, struct yapToken *firstIdentifier)
{
    yapArray *list = yapArrayCreate();
    if(firstIdentifier)
        yapArrayPush(list, yapExpressionCreateIdentifier(firstIdentifier));
    return list;
}

yapArray * yapCompileIdentifierListAppend(yapCompiler *compiler, yapArray *list, struct yapToken *identifier)
{
    yapArrayPush(list, yapExpressionCreateIdentifier(identifier));
    return list;
}

yapArray * yapCompileExpressionListCreate(yapCompiler *compiler, struct yapExpression *firstExpression)
{
    yapArray *list = yapArrayCreate();
    if(firstExpression)
        yapArrayPush(list, firstExpression);
    return list;
}

yapArray * yapCompileExpressionListAppend(yapCompiler *compiler, yapArray *list, struct yapExpression *expression)
{
    yapArrayPush(list, expression);
    return list;
}

struct yapCode * yapCompileStatementFunctionDecl(yapCompiler *compiler, struct yapToken *name, yapArray *args, struct yapCode *body)
{
    yapCode *regFunc; // code that will register the variable for this function
    yOperand index;

    // If there are named arguments, prepend the ops that will create
    // them as temporary variables when the function begins
    if(args->count)
    {
        int i;
        yapCode *code = yapCodeCreate();
        for(i=args->count-1; i>=0; i--)
        {
            yapExpression *arg = (yapExpression*)args->data[i];
            yapCodeAppendNamedArg(compiler, code, arg);
        }
        yapCodeAppendCode(code, body);
        yapCodeDestroy(body);
        body = code;
    }

    yapCodeAppendRet(body, 0);
    index = yapBlockConvertCode(body, compiler->module, args->count);

    yapArrayDestroy(args, (yapDestroyCB)yapExpressionDestroy);

    // Return the chunk of code that registers this function as a variable
    // back to the main module block
    regFunc = yapCodeCreate();
    yapCodeAppendVar(compiler, regFunc, name, yFalse);
    yapCodeGrow(regFunc, 1);
    yapCodeAppend(regFunc, YOP_PUSHLBLOCK, index);
    yapCodeAppendSetVar(regFunc);

    yapTrace(("function created. block %d\n", index));
    return regFunc;
}

struct yapCode * yapCompileStatementExpressionList(yapCompiler *compiler, yapArray *list)
{
    int i;
    yapCode *code = yapCodeCreate();

    for(i=0; i<list->count; i++)
    {
        yapCodeAppendExpression(compiler, code, (yapExpression*)list->data[i], 0);
    }

    yapArrayDestroy(list, (yapDestroyCB)yapExpressionDestroy);
    return code;
}

struct yapCode * yapCompileStatementReturn(yapCompiler *compiler, yapArray *list)
{
    int i;
    yapCode *code = yapCodeCreate();

    for(i=0; i<list->count; i++)
    {
        yapCodeAppendExpression(compiler, code, (yapExpression*)list->data[i], 1);
    }

    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_RET, list->count);

    yapArrayDestroy(list, (yapDestroyCB)yapExpressionDestroy);
    return code;
}

struct yapCode * yapCompileStatementVar(yapCompiler *compiler, struct yapToken *name)
{
    yapCode *code = yapCodeCreate();
    yapCodeAppendVar(compiler, code, name, yTrue);
    return code;
}

struct yapCode * yapCompileStatementVarInit(yapCompiler *compiler, struct yapToken *name, struct yapExpression *initialValue)
{
    yapCode *code = yapCodeCreate();
    yapCodeAppendVar(compiler, code, name, yFalse);
    yapCodeAppendExpression(compiler, code, initialValue, 1);
    yapCodeAppendSetVar(code);

    yapExpressionDestroy(initialValue);
    return code;
}

struct yapCode * yapCompileStatementAssignment(yapCompiler *compiler, struct yapToken *name, struct yapExpression *value)
{
    yapCode *code = yapCodeCreate();
    yapCodeAppendVarRef(compiler, code, name);
    yapCodeAppendExpression(compiler, code, value, 1);
    yapCodeAppendSetVar(code);

    yapExpressionDestroy(value);
    return code;
}

struct yapCode * yapCompileStatementListAppend(yapCompiler *compiler, struct yapCode *list, struct yapCode *statement)
{
    yapCodeAppendCode(list, statement);

    yapCodeDestroy(statement);
    return list;
}

void yapCompileModuleStatementList(yapCompiler *compiler, struct yapCode *list)
{
    yOperand index;
    yapCodeAppendRet(list, 0);
    index = yapBlockConvertCode(list, compiler->module, 0);
    compiler->module->block = compiler->module->blocks.data[index];
}

struct yapCode * yapCompileStatementIfElse(yapCompiler *compiler, struct yapArray *cond, struct yapCode *ifBody, struct yapCode *elseBody)
{
    int i;
    int index;
    yapCode *code = yapCodeCreate();

    if(elseBody)
    {
        yapCodeGrow(elseBody, 1);
        yapCodeAppend(elseBody, YOP_LEAVE, 0);
        index = yapBlockConvertCode(elseBody, compiler->module, 0);
        yapCodeGrow(code, 1);
        yapCodeAppend(code, YOP_PUSHLBLOCK, index);
    }

    yapCodeGrow(ifBody, 1);
    yapCodeAppend(ifBody, YOP_LEAVE, 0);
    index = yapBlockConvertCode(ifBody, compiler->module, 0);
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_PUSHLBLOCK, index);

    // Only keeps the value of the first expression on the stack for bool testing
    for(i=0; i<cond->count; i++)
    {
        yapCodeAppendExpression(compiler, code, (yapExpression*)cond->data[i], (i) ? 0 : 1);
    }

    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_IF, (elseBody) ? 1 : 0);

    yapArrayDestroy(cond, (yapDestroyCB)yapExpressionDestroy);
    return code;
}

struct yapCode * yapCompileStatementLoop(yapCompiler *compiler, struct yapArray *init, struct yapArray *cond, struct yapArray *incr, struct yapCode *body)
{
    int i;
    int skipInstruction = -1;
    yU32 index;
    yapCode *loop = yapCodeCreate();
    yapCode *code = yapCodeCreate();

    if(init && init->count)
    {
        for(i=0; i<init->count; i++)
        {
            yapCodeAppendExpression(compiler, loop, (yapExpression*)init->data[i], 0);
        }
    }

    if(incr && incr->count)
    {
        yapCodeGrow(loop, 1);
        yapCodeAppend(loop, YOP_SKIP, 0);
        skipInstruction = yapCodeLastIndex(loop);
    }

    yapCodeGrow(loop, 1);
    yapCodeAppend(loop, YOP_START, 0);

    if(incr && incr->count)
    {
        for(i=0; i<incr->count; i++)
        {
            yapCodeAppendExpression(compiler, loop, (yapExpression*)incr->data[i], 0);
        }
        loop->ops[skipInstruction].operand = yapCodeLastIndex(loop) - skipInstruction;
    }

    if(cond->count)
    {
        // Only keeps the value of the first expression on the stack for bool testing
        for(i=0; i<cond->count; i++)
        {
            yapCodeAppendExpression(compiler, loop, (yapExpression*)cond->data[i], (i) ? 0 : 1);
        }
        yapCodeGrow(loop, 1);
        yapCodeAppend(loop, YOP_LEAVE, 1);
    }
    yapCodeAppendCode(loop, body);
    yapCodeGrow(loop, 1);
    yapCodeAppend(loop, YOP_BREAK, 1);
    index = yapBlockConvertCode(loop, compiler->module, 0);

    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_PUSHLBLOCK, index);
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_ENTER, 0);

    yapCodeDestroy(body);
    yapArrayDestroy(cond, (yapDestroyCB)yapExpressionDestroy);
    return code;
}

