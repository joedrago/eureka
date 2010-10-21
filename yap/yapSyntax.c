#include "yapSyntax.h"

#include "yapLexer.h"

#include <string.h>
#include <stdio.h>

yapSyntaxTree * yapSyntaxTreeCreate()
{
    yapSyntaxTree *tree = (yapSyntaxTree *)yapAlloc(sizeof(yapSyntaxTree));
    return tree;
}

void yapSyntaxTreeDestroy(yapSyntaxTree *tree)
{
    yapArrayClear(&tree->errors, yapFree);

    if(tree->root)
        yapSyntaxDestroy(tree->root);

    yapFree(tree);
}

void yapSyntaxTreeError(yapSyntaxTree *tree, const char *error)
{
    yapArrayPush(&tree->errors, yapStrdup(error));
}

void yapSyntaxTreeSyntaxError(yapSyntaxTree *tree, struct yapToken *token)
{
    char error[64];
    strcpy(error, "syntax error near '");
    if(token && token->text)
    {
        char temp[32];
        int len = strlen(token->text);
        if(len > 31) len = 31;
        memcpy(temp, token->text, len);
        temp[len] = 0;
        strcat(error, temp);
    }
    else
    {
        strcat(error, "<unknown>");
    }
    strcat(error, "'");

    yapSyntaxTreeError(tree, error);
}

// ---------------------------------------------------------------------------

yapSyntax * yapSyntaxCreate(yU32 type)
{
    yapSyntax *syntax = (yapSyntax *)yapAlloc(sizeof(yapSyntax));
    syntax->type = type;
    return syntax;
}

void yapSyntaxDestroy(yapSyntax *syntax)
{
    // TODO: lots of things

    yapFree(syntax);
}

yapSyntax * yapSyntaxCreateKString(struct yapToken *token)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_KSTRING);
    syntax->v.s = yapTokenToString(token);
    return syntax;
}

yapSyntax * yapSyntaxCreateKInt(struct yapToken *token)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_KINT);
    syntax->v.i = yapTokenToInt(token);
    return syntax;
}

yapSyntax * yapSyntaxCreateIdentifier(struct yapToken *token)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_IDENTIFIER);
    syntax->v.s = yapTokenToString(token);
    return syntax;
}

yapSyntax * yapSyntaxCreateNull()
{
    yapSyntax *syntax = yapSyntaxCreate(YST_NULL);
    return syntax;
}

yapSyntax * yapSyntaxCreateList(yU32 type, yapSyntax *firstExpr)
{
    yapSyntax *syntax = yapSyntaxCreate(type);
    syntax->v.a = yapArrayCreate();
    if(firstExpr)
        yapArrayPush(syntax->v.a, firstExpr);
    return syntax;
}

yapSyntax * yapSyntaxListAppend(yapSyntax *list, yapSyntax *expr)
{
    yapArrayPush(list->v.a, expr);
    return list;
}

yapSyntax * yapSyntaxCreateCall(struct yapToken *name, yapSyntax *args)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_CALL);
    syntax->v.s = yapTokenToString(name);
    syntax->r.a = args->r.a;
    args->r.a   = NULL;

    yapSyntaxDestroy(args);
    return syntax;
}

yapSyntax * yapSyntaxCreateStringFormat(yapSyntax *format, yapSyntax *args)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_STRINGFORMAT);
    syntax->l.p = format;
    syntax->r.a = args->r.a;
    args->r.a   = NULL;

    yapSyntaxDestroy(args);
    return syntax;
}

yapSyntax * yapSyntaxCreateUnary(yU32 type, yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(type);
    syntax->r.p = expr;
    return syntax;
}

yapSyntax * yapSyntaxCreateCombine(yU32 type, yapSyntax *l, yapSyntax *r)
{
    yapSyntax *syntax = yapSyntaxCreate(type);
    syntax->r.p = l;
    syntax->r.p = r;
    return syntax;
}

yapSyntax * yapSyntaxCreateStatementExpr(yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_STATEMENT_EXPR);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax * yapSyntaxCreateAssignment(struct yapToken *token, yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_ASSIGNMENT);
    syntax->v.s = yapTokenToString(token);
    syntax->r.p = expr;
    return syntax;
}

yapSyntax * yapSyntaxCreateVar(struct yapToken *token, yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_VAR);
    syntax->v.s = yapTokenToString(token);
    syntax->r.p = expr;
    return syntax;
}

yapSyntax * yapSyntaxCreateReturn(yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_RETURN);
    syntax->r.p = expr;
    return syntax;
}

yapSyntax * yapSyntaxCreateIfElse(yapSyntax *cond, yapSyntax *ifBody, yapSyntax *elseBody)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_IFELSE);
    syntax->v.p = cond;
    syntax->l.p = ifBody;
    syntax->r.p = elseBody;
    return syntax;
}

yapSyntax * yapSyntaxCreateLoop(yapSyntax *cond, yapSyntax *body)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_LOOP);
    syntax->v.p = cond;
    syntax->l.p = body;
    return syntax;
}

yapSyntax * yapSyntaxCreateFunctionDecl(struct yapToken *name, yapSyntax *args, yapSyntax *body)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_FUNCTION);
    syntax->v.s = yapTokenToString(name);
    syntax->l.p = args;
    syntax->r.p = body;
    return syntax;
}

