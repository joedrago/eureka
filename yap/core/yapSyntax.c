#include "yapSyntax.h"

#include "yapLexer.h"

#include <string.h>
#include <stdio.h>

yapSyntax *yapSyntaxCreate(yU32 type, int line)
{
    yapSyntax *syntax = (yapSyntax *)yapAlloc(sizeof(yapSyntax));
    syntax->type = type;
    syntax->line = line;
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
    yapSyntax *syntax = yapSyntaxCreate(YST_KSTRING, token->line);
    syntax->v.s = yapTokenToString(token);
    return syntax;
}

yapSyntax *yapSyntaxCreateKInt(struct yapToken *token, yU32 opts)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_KINT, token->line);
    syntax->v.i = yapTokenToInt(token);
    if(opts & CKO_NEGATIVE)
        syntax->v.i *= -1;
    return syntax;
}

yapSyntax *yapSyntaxCreateKFloat(struct yapToken *token, yU32 opts)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_KFLOAT, token->line);
    syntax->v.f = yapTokenToFloat(token);
    if(opts & CKO_NEGATIVE)
        syntax->v.f *= -1;
    return syntax;
}

yapSyntax *yapSyntaxCreateIdentifier(struct yapToken *token)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_IDENTIFIER, token->line);
    syntax->v.s = yapTokenToString(token);
    return syntax;
}

yapSyntax *yapSyntaxCreateIndex(yapSyntax *array, yapSyntax *index, yBool pushThis)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_INDEX, array->line);
    syntax->v.i = pushThis;
    syntax->l.p = array;
    syntax->r.p = index;
    return syntax;
}

yapSyntax *yapSyntaxCreateNull(int line)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_NULL, line);
    return syntax;
}

yapSyntax *yapSyntaxCreateThis(int line)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_THIS, line);
    return syntax;
}

yapSyntax *yapSyntaxCreateList(yU32 type, yapSyntax *firstExpr)
{
    yapSyntax *syntax = yapSyntaxCreate(type, (firstExpr) ? firstExpr->line : 0);
    syntax->v.a = yapArrayCreate();
    if(firstExpr)
        yapArrayPush(syntax->v.a, firstExpr);
    return syntax;
}

yapSyntax *yapSyntaxListAppend(yapSyntax *list, yapSyntax *expr, yU32 flags)
{
    if(expr != NULL)
    {
        int index;
        if(!list->line)
            list->line = expr->line;
        index = yapArrayPush(list->v.a, expr);
        if((flags & YSLF_AUTOLITERAL) && (index > 0))
        {
            yapSyntax *toLiteral = (yapSyntax *)list->v.a->data[index - 1];
            if(toLiteral->type == YST_IDENTIFIER)
                toLiteral->type = YST_KSTRING;
        }
    }
    return list;
}

yapSyntax *yapSyntaxCreateCall(yapSyntax *func, yapSyntax *args)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_CALL, func->line);
    syntax->v.p = func;
    syntax->r.p = args;
    return syntax;
}

yapSyntax *yapSyntaxCreateStringFormat(yapSyntax *format, yapSyntax *args)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_STRINGFORMAT, format->line);
    syntax->l.p = format;
    syntax->r.p = args;
    return syntax;
}

yapSyntax *yapSyntaxCreateUnary(yU32 type, yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(type, expr->line);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax *yapSyntaxCreateBinary(yU32 type, yapSyntax *l, yapSyntax *r, yBool compound)
{
    yapSyntax *syntax = yapSyntaxCreate(type, l->line);
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

    syntax = yapSyntaxCreate(YST_STATEMENT_EXPR, expr->line);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax *yapSyntaxCreateAssignment(yapSyntax *l, yapSyntax *r)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_ASSIGNMENT, l->line);
    syntax->l.p = l;
    syntax->r.p = r;
    syntax->line = l->line;
    return syntax;
}

yapSyntax *yapSyntaxCreateInherits(yapSyntax *l, yapSyntax *r)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_INHERITS, l->line);
    syntax->l.p = l;
    syntax->r.p = r;
    return syntax;
}

yapSyntax *yapSyntaxCreateVar(yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_VAR, expr->line);
    syntax->v.p = expr;
    syntax->line = expr->line;
    return syntax;
}

yapSyntax *yapSyntaxCreateReturn(yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_RETURN, expr->line);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax *yapSyntaxCreateIfElse(yapSyntax *cond, yapSyntax *ifBody, yapSyntax *elseBody)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_IFELSE, cond->line);
    syntax->v.p = cond;
    syntax->l.p = ifBody;
    syntax->r.p = elseBody;
    return syntax;
}

yapSyntax *yapSyntaxCreateWhile(yapSyntax *cond, yapSyntax *body)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_WHILE, cond->line);
    syntax->v.p = cond;
    syntax->r.p = body;
    return syntax;
}

yapSyntax *yapSyntaxCreateFor(yapSyntax *vars, yapSyntax *iter, yapSyntax *body)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_FOR, vars->line);
    syntax->v.p = vars;
    syntax->l.p = iter;
    syntax->r.p = body;
    return syntax;
}

yapSyntax *yapSyntaxCreateFunctionDecl(struct yapToken *name, yapSyntax *args, yapSyntax *body, int line)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_FUNCTION, line);
    syntax->v.s = (name) ? yapTokenToString(name) : NULL;
    syntax->l.p = args;
    syntax->r.p = body;
    syntax->line = line;
    return syntax;
}

yapSyntax *yapSyntaxCreateFunctionArgs(yapSyntax *args, struct yapToken *varargs)
{
    int line = (args) ? args->line : (varargs) ? varargs->line : 0;
    yapSyntax *syntax = yapSyntaxCreate(YST_FUNCTION_ARGS, line);
    syntax->l.p = args;
    syntax->v.s = (varargs) ? yapTokenToString(varargs) : NULL;
    return syntax;
}

yapSyntax *yapSyntaxCreateScope(yapSyntax *body)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_SCOPE, body->line);
    syntax->v.p = body;
    return syntax;
}
