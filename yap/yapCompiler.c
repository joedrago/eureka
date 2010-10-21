#include "yapCompiler.h"

#include "yapCode.h"
#include "yapLexer.h"
#include "yapModule.h"
#include "yapParser.h"
#include "yapSyntax.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void *yapParseAlloc();
void yapParse(void *yyp, int id, yapToken token, yapCompiler *compiler);
void yapParseFree(void *p);

yBool yapAssemble(yapCompiler *compiler);

void yapCompilerDestroy(yapCompiler *compiler)
{
    yapFree(compiler);
}

yBool yapCompile(yapCompiler *compiler, const char *text)
{
    yBool success = yFalse;
    yapToken emptyToken = {0};
    void *parser = yapParseAlloc();

    compiler->tree = yapSyntaxTreeCreate();

    yapLex(parser, text, yapParse, compiler);
    yapParse(parser, 0, emptyToken, compiler);

    if(compiler->tree->root && !compiler->errors.count)
    {
        yapAssemble(compiler);
        success = yTrue;
        printf("hurr\n");
        // yapModuleDump(compiler->module);
    };

    yapSyntaxTreeDestroy(compiler->tree);
    yapParseFree(parser);
    return success;
}

void yapCompileError(yapCompiler *compiler, const char *error)
{
    yapArrayPush(&compiler->errors, yapStrdup(error));
}

yapArray * yapCompileIdentifierListCreate(yapCompiler *compiler, struct yapToken *firstIdentifier)
{
    yapArray *list = yapArrayCreate();
    if(firstIdentifier)
        yapArrayPush(list, yapTokenClone(firstIdentifier));
    return list;
}

yapArray * yapCompileIdentifierListAppend(yapCompiler *compiler, yapArray *list, struct yapToken *identifier)
{
    yapArrayPush(list, yapTokenClone(identifier));
    return list;
}

yapArray * yapCompileExpressionListCreate(yapCompiler *compiler, struct yapCode *firstExpression)
{
    yapArray *list = yapArrayCreate();
    if(firstExpression)
        yapArrayPush(list, firstExpression);
    return list;
}

yapArray * yapCompileExpressionListAppend(yapCompiler *compiler, yapArray *list, struct yapCode *expression)
{
    yapArrayPush(list, expression);
    return list;
}

struct yapCode * yapCompileCombine(yapCompiler *compiler, yOpcode op, struct yapCode *code, struct yapCode *expr)
{
    yapCodeAppendExpression(compiler, code, expr, 1);
    yapCodeGrow(code, 1);
    yapCodeAppend(code, op, 0);
    yapCodeDestroy(expr);
    return code;
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
    yapCodeGrow(regFunc, 1);
    yapCodeAppend(regFunc, YOP_PUSHLBLOCK, index);
    yapCodeAppendVar(compiler, regFunc, name, yFalse);
    yapCodeAppendSetVar(regFunc);
    return regFunc;
}

struct yapCode * yapCompileStatementExpressionList(yapCompiler *compiler, yapArray *list)
{
    int i;
    yapCode *code = yapCodeCreate();

    for(i=0; i<list->count; i++)
    {
        yapCodeAppendExpression(compiler, code, (yapCode*)list->data[i], 0);
    }

    yapArrayDestroy(list, (yapDestroyCB)yapCodeDestroy);
    return code;
}

struct yapCode * yapCompileStatementReturn(yapCompiler *compiler, yapArray *list)
{
    int i;
    yapCode *code = yapCodeCreate();

    for(i=0; i<list->count; i++)
    {
        yapCodeAppendExpression(compiler, code, (yapCode*)list->data[i], 1);
    }

    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_RET, list->count);

    yapArrayDestroy(list, (yapDestroyCB)yapCodeDestroy);
    return code;
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
    yapCodeAppendExpression(compiler, code, initialValue, 1);
    yapCodeAppendVar(compiler, code, name, yFalse);
    yapCodeAppendSetVar(code);

    yapCodeDestroy(initialValue);
    return code;
}

struct yapCode * yapCompileStatementAssignment(yapCompiler *compiler, struct yapToken *name, struct yapCode *value)
{
    yapCode *code = yapCodeCreate();
    yapCodeAppendExpression(compiler, code, value, 1);
    yapCodeAppendVarRef(compiler, code, name);
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
        yapCodeAppendExpression(compiler, code, (yapCode*)cond->data[i], (i) ? 0 : 1);
    }

    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_IF, (elseBody) ? 1 : 0);

    yapArrayDestroy(cond, (yapDestroyCB)yapCodeDestroy);
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
            yapCodeAppendExpression(compiler, loop, (yapCode*)init->data[i], 0);
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
            yapCodeAppendExpression(compiler, loop, (yapCode*)incr->data[i], 0);
        }
        loop->ops[skipInstruction].operand = yapCodeLastIndex(loop) - skipInstruction;
    }

    if(cond->count)
    {
        // Only keeps the value of the first expression on the stack for bool testing
        for(i=0; i<cond->count; i++)
        {
            yapCodeAppendExpression(compiler, loop, (yapCode*)cond->data[i], (i) ? 0 : 1);
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
    if(init)
        yapArrayDestroy(init, (yapDestroyCB)yapCodeDestroy);
    if(incr)
        yapArrayDestroy(incr, (yapDestroyCB)yapCodeDestroy);
    yapArrayDestroy(cond, (yapDestroyCB)yapCodeDestroy);
    return code;
}

yapCode * yapCompileAppendOp(yapCompiler *compiler, struct yapCode *code, yOpcode opcode, yOperand operand)
{
    yapCodeGrow(code, 1);
    yapCodeAppend(code, opcode, operand);
    return code;
}

// ---------------------------------------------------------------------------

enum
{
    YAF_HURR
};

typedef yapCode * (*yapAssembleFunc)(yapCompiler *compiler, yapCode *dst, yapSyntax *syntax, int *keep);

yapCode * yapAssembleFoo(yapCompiler *compiler, yapCode *dst, yapSyntax *syntax, int *keep)
{
    return NULL;
}

typedef struct yapAssembleInfo
{
    yU32 flags;
    yapAssembleFunc assemble;
} yapAssembleInfo;

#define ASMFORWARD(NAME) \
yapCode * yapAssemble ## NAME (yapCompiler *compiler, yapCode *dst, yapSyntax *syntax, int *keep)

ASMFORWARD(StatementList);

static yapAssembleInfo asmDispatch[YST_COUNT] =
{
    { 0, yapAssembleFoo },             // YST_ROOT

    { 0, yapAssembleFoo },             // YST_KSTRING
    { 0, yapAssembleFoo },             // YST_KINT
    { 0, yapAssembleFoo },             // YST_IDENTIFIER

    { 0, yapAssembleStatementList },   // YST_STATEMENTLIST
    { 0, yapAssembleFoo },             // YST_EXPRESSIONLIST
    { 0, yapAssembleFoo },             // YST_IDENTIFIERLIST

    { 0, yapAssembleFoo },             // YST_CALL
    { 0, yapAssembleFoo },             // YST_STRINGFORMAT

    { 0, yapAssembleFoo },             // YST_NULL

    { 0, yapAssembleFoo },             // YST_TOSTRING
    { 0, yapAssembleFoo },             // YST_TOINT
    { 0, yapAssembleFoo },             // YST_ADD
    { 0, yapAssembleFoo },             // YST_SUB
    { 0, yapAssembleFoo },             // YST_MUL
    { 0, yapAssembleFoo },             // YST_DIV

    { 0, yapAssembleFoo },             // YST_STATEMENT_EXPR
    { 0, yapAssembleFoo },             // YST_ASSIGNMENT
    { 0, yapAssembleFoo },             // YST_VAR
    { 0, yapAssembleFoo },             // YST_RETURN

    { 0, yapAssembleFoo },             // YST_IFELSE
    { 0, yapAssembleFoo },             // YST_LOOP
    { 0, yapAssembleFoo },             // YST_FUNCTION
};

yapCode * yapAssembleStatementList(yapCompiler *compiler, yapCode *dst, yapSyntax *syntax, int *keep)
{
    int i;
    for(i=0; i<syntax->r.a->count; i++)
    {
        yapSyntax *child = (yapSyntax *)syntax->r.a->data[i];
        asmDispatch[child->type].assemble(compiler, dst, child, NULL);
    }
    return dst;
}

yBool yapAssemble(yapCompiler *compiler)
{
    if(compiler->tree->root && (compiler->tree->root->type == YST_STATEMENTLIST))
    {
        compiler->module = yapModuleCreate();
        compiler->code   = yapCodeCreate();
        yapAssembleStatementList(compiler, compiler->code, compiler->tree->root, NULL);
        printf("moar\n");
    }
    return yTrue;
}

