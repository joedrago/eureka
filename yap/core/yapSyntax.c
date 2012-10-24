// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapSyntax.h"
#include "yapContext.h"

#include "yapLexer.h"

#include <string.h>
#include <stdio.h>

yapSyntax *yapSyntaxCreate(struct yapContext *Y, yU32 type, int line)
{
    yapSyntax *syntax = (yapSyntax *)yapAlloc(sizeof(yapSyntax));
    syntax->type = type;
    syntax->line = line;
    return syntax;
}

static void yapSyntaxElementClear(struct yapContext *Y, yapSyntaxElement *e)
{
    if(e->p)
    {
        yapSyntaxDestroy(Y, e->p);
    }
    if(e->s)
    {
        yapFree(e->s);
    }
    if(e->a)
    {
        yapArrayDestroy(Y, &e->a, (yapDestroyCB)yapSyntaxDestroy);
    }
}

void yapSyntaxDestroy(struct yapContext *Y, yapSyntax *syntax)
{
    yapSyntaxElementClear(Y, &syntax->v);
    yapSyntaxElementClear(Y, &syntax->l);
    yapSyntaxElementClear(Y, &syntax->r);

    yapFree(syntax);
}

yapSyntax *yapSyntaxCreateKString(struct yapContext *Y, struct yapToken *token)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_KSTRING, token->line);
    syntax->v.s = yapTokenToString(Y, token);
    return syntax;
}

yapSyntax *yapSyntaxCreateKInt(struct yapContext *Y, struct yapToken *token, yU32 opts)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_KINT, token->line);
    syntax->v.i = yapTokenToInt(Y, token);
    if(opts & CKO_NEGATIVE)
    {
        syntax->v.i *= -1;
    }
    return syntax;
}

yapSyntax *yapSyntaxCreateKFloat(struct yapContext *Y, struct yapToken *token, yU32 opts)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_KFLOAT, token->line);
    syntax->v.f = yapTokenToFloat(Y, token);
    if(opts & CKO_NEGATIVE)
    {
        syntax->v.f *= -1;
    }
    return syntax;
}

yapSyntax *yapSyntaxCreateIdentifier(struct yapContext *Y, struct yapToken *token)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_IDENTIFIER, token->line);
    syntax->v.s = yapTokenToString(Y, token);
    return syntax;
}

yapSyntax *yapSyntaxCreateIndex(struct yapContext *Y, yapSyntax *array, yapSyntax *index, yBool pushThis)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_INDEX, array->line);
    syntax->v.i = pushThis;
    syntax->l.p = array;
    syntax->r.p = index;
    return syntax;
}

yapSyntax *yapSyntaxCreateNull(struct yapContext *Y, int line)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_NULL, line);
    return syntax;
}

yapSyntax *yapSyntaxCreateThis(struct yapContext *Y, int line)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_THIS, line);
    return syntax;
}

yapSyntax *yapSyntaxCreateList(struct yapContext *Y, yU32 type, yapSyntax *firstExpr)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, type, (firstExpr) ? firstExpr->line : 0);
    syntax->v.a = NULL;
    if(firstExpr)
    {
        yapArrayPush(Y, &syntax->v.a, firstExpr);
    }
    return syntax;
}

yapSyntax *yapSyntaxListAppend(struct yapContext *Y, yapSyntax *list, yapSyntax *expr, yU32 flags)
{
    if(expr != NULL)
    {
        int index;
        if(!list->line)
        {
            list->line = expr->line;
        }
        index = yapArrayPush(Y, &list->v.a, expr);
        if((flags & YSLF_AUTOLITERAL) && (index > 0))
        {
            yapSyntax *toLiteral = list->v.a[index - 1];
            if(toLiteral->type == YST_IDENTIFIER)
            {
                toLiteral->type = YST_KSTRING;
            }
        }
    }
    return list;
}

yapSyntax *yapSyntaxCreateCall(struct yapContext *Y, yapSyntax *func, yapSyntax *args)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_CALL, func->line);
    syntax->v.p = func;
    syntax->r.p = args;
    return syntax;
}

yapSyntax *yapSyntaxCreateStringFormat(struct yapContext *Y, yapSyntax *format, yapSyntax *args)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_STRINGFORMAT, format->line);
    syntax->l.p = format;
    syntax->r.p = args;
    return syntax;
}

yapSyntax *yapSyntaxCreateUnary(struct yapContext *Y, yU32 type, yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, type, expr->line);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax *yapSyntaxCreateBinary(struct yapContext *Y, yU32 type, yapSyntax *l, yapSyntax *r, yBool compound)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, type, l->line);
    syntax->l.p = l;
    syntax->r.p = r;
    syntax->v.i = compound;
    return syntax;
}

yapSyntax *yapSyntaxCreateStatementExpr(struct yapContext *Y, yapSyntax *expr)
{
    yapSyntax *syntax;

    if((expr->type == YST_EXPRESSIONLIST) && (yapArraySize(Y, &expr->v.a) == 0))
    {
        // An empty statement. Without PYTHON_SCOPING, this only happens
        // when there are multiple semicolons in a row. However, when
        // it is enabled, one is created for every blank line!
        yapSyntaxDestroy(Y, expr);
        return NULL;
    }

    syntax = yapSyntaxCreate(Y, YST_STATEMENT_EXPR, expr->line);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax *yapSyntaxCreateAssignment(struct yapContext *Y, yapSyntax *l, yapSyntax *r)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_ASSIGNMENT, l->line);
    syntax->l.p = l;
    syntax->r.p = r;
    syntax->line = l->line;
    return syntax;
}

yapSyntax *yapSyntaxCreateInherits(struct yapContext *Y, yapSyntax *l, yapSyntax *r)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_INHERITS, l->line);
    syntax->l.p = l;
    syntax->r.p = r;
    return syntax;
}

yapSyntax *yapSyntaxCreateVar(struct yapContext *Y, yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_VAR, expr->line);
    syntax->v.p = expr;
    syntax->line = expr->line;
    return syntax;
}

yapSyntax *yapSyntaxCreateBreak(struct yapContext *Y, int line)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_BREAK, line);
    return syntax;
}

yapSyntax *yapSyntaxCreateReturn(struct yapContext *Y, yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_RETURN, expr->line);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax *yapSyntaxCreateIfElse(struct yapContext *Y, yapSyntax *cond, yapSyntax *ifBody, yapSyntax *elseBody)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_IFELSE, cond->line);
    syntax->v.p = cond;
    syntax->l.p = ifBody;
    syntax->r.p = elseBody;
    return syntax;
}

yapSyntax *yapSyntaxCreateWhile(struct yapContext *Y, yapSyntax *cond, yapSyntax *body)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_WHILE, cond->line);
    syntax->v.p = cond;
    syntax->r.p = body;
    return syntax;
}

yapSyntax *yapSyntaxCreateFor(struct yapContext *Y, yapSyntax *vars, yapSyntax *iter, yapSyntax *body)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_FOR, vars->line);
    syntax->v.p = vars;
    syntax->l.p = iter;
    syntax->r.p = body;
    return syntax;
}

yapSyntax *yapSyntaxCreateFunctionDecl(struct yapContext *Y, struct yapToken *name, yapSyntax *args, yapSyntax *body, int line)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_FUNCTION, line);
    syntax->v.s = (name) ? yapTokenToString(Y, name) : NULL;
    syntax->l.p = args;
    syntax->r.p = body;
    syntax->line = line;
    return syntax;
}

yapSyntax *yapSyntaxCreateFunctionArgs(struct yapContext *Y, yapSyntax *args, struct yapToken *varargs)
{
    int line = (args) ? args->line : (varargs) ? varargs->line : 0;
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_FUNCTION_ARGS, line);
    syntax->l.p = args;
    syntax->v.s = (varargs) ? yapTokenToString(Y, varargs) : NULL;
    return syntax;
}

yapSyntax *yapSyntaxCreateScope(struct yapContext *Y, yapSyntax *body)
{
    yapSyntax *syntax = yapSyntaxCreate(Y, YST_SCOPE, body->line);
    syntax->v.p = body;
    return syntax;
}
