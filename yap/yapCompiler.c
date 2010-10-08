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
    {
        yapToken *token = (yapToken*)yapAlloc(sizeof(yapToken));
        *token = *firstIdentifier;
        yapArrayPush(list, token);
    }
    return list;
}

yapArray * yapCompileIdentifierListAppend(yapCompiler *compiler, yapArray *list, struct yapToken *identifier)
{
    yapToken *token = (yapToken*)yapAlloc(sizeof(yapToken));
    *token = *identifier;
    yapArrayPush(list, token);
    return list;
}

struct yapCode * yapCompileExpressionListCreate(yapCompiler *compiler, struct yapCode *firstExpression)
{
    yapCode *list = yapCodeCreate();
    if(firstExpression)
        yapCodeAppendCode(list, firstExpression);
    return list;
}

struct yapCode * yapCompileExpressionListAppend(yapCompiler *compiler, struct yapCode *list, struct yapCode *expression)
{
    yapCodeAppendCode(list, expression);
    return list;
}

struct yapCode * yapCompileStatementFunctionDecl(yapCompiler *compiler, struct yapToken *name, struct yapArray *args, struct yapCode *body)
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
            yapToken *arg = (yapToken*)args->data[i];
            yapCodeAppendNamedArg(compiler, code, arg);
        }
        yapCodeAppendCode(code, body);
        yapCodeDestroy(body);
        body = code;
    }

    yapCodeAppendRet(body, 0);
    index = yapBlockConvertCode(body, compiler->module, args->count);

    yapArrayDestroy(args, (yapDestroyCB)yapTokenDestroy);

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

struct yapCode * yapCompileStatementExpressionList(yapCompiler *compiler, yapCode *list)
{
    yapCodeAppendKeep(compiler, list, 0);
    return list;
}

struct yapCode * yapCompileStatementReturn(yapCompiler *compiler, yapCode *list)
{
    yapCodeAppend(list, YOP_RET, list->offer);
    return list;
}

struct yapCode * yapCompileStatementVar(yapCompiler *compiler, struct yapToken *name)
{
    yapCode *code = yapCodeCreate();
    yapCodeAppendVar(compiler, code, name, yTrue);
    return code;
}

struct yapCode * yapCompileStatementVarInit(yapCompiler *compiler, struct yapToken *name, struct yapCode *initialValue)
{
    yapCode *code = yapCodeCreate();
    yapCodeAppendVar(compiler, code, name, yFalse);
    yapCodeAppendExpression(compiler, code, initialValue, 1);
    yapCodeAppendSetVar(code);

    yapCodeDestroy(initialValue);
    return code;
}

struct yapCode * yapCompileStatementAssignment(yapCompiler *compiler, struct yapToken *name, struct yapCode *value)
{
    yapCode *code = yapCodeCreate();
    yapCodeAppendVarRef(compiler, code, name);
    yapCodeAppendExpression(compiler, code, value, 1);
    yapCodeAppendSetVar(code);

    yapCodeDestroy(value);
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

struct yapCode * yapCompileStatementIfElse(yapCompiler *compiler, struct yapCode *cond, struct yapCode *ifBody, struct yapCode *elseBody)
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

    yapCodeAppendKeep(compiler, cond, 1);
    yapCodeAppendCode(code, cond);

    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_IF, (elseBody) ? 1 : 0);

    yapCodeDestroy(cond);
    return code;
}

struct yapCode * yapCompileStatementLoop(yapCompiler *compiler, struct yapCode *init, struct yapCode *cond, struct yapCode *incr, struct yapCode *body)
{
    int i;
    int skipInstruction = -1;
    yU32 index;
    yapCode *loop = yapCodeCreate();
    yapCode *code = yapCodeCreate();

    if(init)
    {
        yapCodeAppendKeep(compiler, init, 0);
        yapCodeAppendCode(init, code);
    }

    if(incr)
    {
        yapCodeGrow(loop, 1);
        yapCodeAppend(loop, YOP_SKIP, 0);
        skipInstruction = yapCodeLastIndex(loop);
    }

    yapCodeGrow(loop, 1);
    yapCodeAppend(loop, YOP_START, 0);

    if(incr)
    {
        for(i=0; i<incr->count; i++)
        {
            yapCodeAppendKeep(compiler, incr, 0);
            yapCodeAppendCode(incr, code);
        }
        loop->ops[skipInstruction].operand = yapCodeLastIndex(loop) - skipInstruction;
    }

    if(cond)
    {
        yapCodeAppendKeep(compiler, cond, 0);
        yapCodeAppendCode(cond, code);
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

    if(init)
        yapCodeDestroy(init);
    if(incr)
        yapCodeDestroy(incr);
    yapCodeDestroy(body);
    yapCodeDestroy(cond);
    return code;
}

