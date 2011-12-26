#include "yapSyntax.h"

#include "yapLexer.h"

#include <string.h>
#include <stdio.h>

yapSyntax *yapSyntaxCreate(yU32 type)
{
    yapSyntax *syntax = (yapSyntax *)yapAlloc(sizeof(yapSyntax));
    syntax->type = type;
    return syntax;
}

static void yapSyntaxElementClear(yapSyntaxElement *e)
{
    if(e->p)
        yapSyntaxDestroy(e->p);
    if(e->s)
        yapFree(e->s);
    if(e->a)
        yapArrayDestroy(e->a, (yapDestroyCB)yapSyntaxDestroy);
}

void yapSyntaxDestroy(yapSyntax *syntax)
{
    yapSyntaxElementClear(&syntax->v);
    yapSyntaxElementClear(&syntax->l);
    yapSyntaxElementClear(&syntax->r);

    yapFree(syntax);
}

yapSyntax *yapSyntaxCreateKString(struct yapToken *token)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_KSTRING);
    syntax->v.s = yapTokenToString(token);
    return syntax;
}

yapSyntax *yapSyntaxCreateKInt(struct yapToken *token)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_KINT);
    syntax->v.i = yapTokenToInt(token);
    return syntax;
}

yapSyntax *yapSyntaxCreateKFloat(struct yapToken *token)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_KFLOAT);
    syntax->v.f = yapTokenToFloat(token);
    return syntax;
}

yapSyntax *yapSyntaxCreateIdentifier(struct yapToken *token)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_IDENTIFIER);
    syntax->v.s = yapTokenToString(token);
    return syntax;
}

yapSyntax *yapSyntaxCreateIndex(yapSyntax *array, yapSyntax *index)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_INDEX);
    syntax->l.p = array;
    syntax->r.p = index;
    return syntax;
}

yapSyntax *yapSyntaxCreateNull()
{
    yapSyntax *syntax = yapSyntaxCreate(YST_NULL);
    return syntax;
}

yapSyntax *yapSyntaxCreateList(yU32 type, yapSyntax *firstExpr)
{
    yapSyntax *syntax = yapSyntaxCreate(type);
    syntax->v.a = yapArrayCreate();
    if(firstExpr)
        yapArrayPush(syntax->v.a, firstExpr);
    return syntax;
}

yapSyntax *yapSyntaxListAppend(yapSyntax *list, yapSyntax *expr)
{
    if(expr != NULL)
        yapArrayPush(list->v.a, expr);
    return list;
}

yapSyntax *yapSyntaxCreateIndexedCall(yapSyntax *obj, yapSyntax *func, yapSyntax *args)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_CALL);
    syntax->l.p = obj;
    syntax->v.p = func;
    syntax->r.p = args;
    return syntax;
}

yapSyntax *yapSyntaxCreateCall(yapSyntax *func, yapSyntax *args)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_CALL);
    syntax->v.p = func;
    syntax->r.p = args;
    return syntax;
}

yapSyntax *yapSyntaxCreateStringFormat(yapSyntax *format, yapSyntax *args)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_STRINGFORMAT);
    syntax->l.p = format;
    syntax->r.p = args;
    return syntax;
}

yapSyntax *yapSyntaxCreateUnary(yU32 type, yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(type);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax *yapSyntaxCreateBinary(yU32 type, yapSyntax *l, yapSyntax *r, yBool compound)
{
    yapSyntax *syntax = yapSyntaxCreate(type);
    syntax->l.p = l;
    syntax->r.p = r;
    syntax->v.i = compound;
    return syntax;
}

yapSyntax *yapSyntaxCreateStatementExpr(yapSyntax *expr)
{
    yapSyntax *syntax;

    if((expr->type == YST_EXPRESSIONLIST) && (expr->v.a->count == 0))
    {
        // An empty statement. Without PYTHON_SCOPING, this only happens
        // when there are multiple semicolons in a row. However, when
        // it is enabled, one is created for every blank line!
        yapSyntaxDestroy(expr);
        return NULL;
    }

    syntax = yapSyntaxCreate(YST_STATEMENT_EXPR);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax *yapSyntaxCreateAssignment(yapSyntax *l, yapSyntax *r)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_ASSIGNMENT);
    syntax->l.p = l;
    syntax->r.p = r;
    return syntax;
}

yapSyntax *yapSyntaxCreateInherits(yapSyntax *l, yapSyntax *r)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_INHERITS);
    syntax->l.p = l;
    syntax->r.p = r;
    return syntax;
}

yapSyntax *yapSyntaxCreateVar(yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_VAR);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax *yapSyntaxCreateReturn(yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_RETURN);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax *yapSyntaxCreateIfElse(yapSyntax *cond, yapSyntax *ifBody, yapSyntax *elseBody)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_IFELSE);
    syntax->v.p = cond;
    syntax->l.p = ifBody;
    syntax->r.p = elseBody;
    return syntax;
}

yapSyntax *yapSyntaxCreateWhile(yapSyntax *cond, yapSyntax *body)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_WHILE);
    syntax->v.p = cond;
    syntax->r.p = body;
    return syntax;
}

yapSyntax *yapSyntaxCreateFor(yapSyntax *vars, yapSyntax *iter, yapSyntax *body)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_FOR);
    syntax->v.p = vars;
    syntax->l.p = iter;
    syntax->r.p = body;
    return syntax;
}

yapSyntax *yapSyntaxCreateFunctionDecl(struct yapToken *name, yapSyntax *args, yapSyntax *body)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_FUNCTION);
    syntax->v.s = (name) ? yapTokenToString(name) : NULL;
    syntax->l.p = args;
    syntax->r.p = body;
    return syntax;
}

yapSyntax *yapSyntaxCreateWith(yapSyntax *obj, yapSyntax *body)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_WITH);
    syntax->v.p = obj;
    syntax->r.p = body;
    return syntax;
}
