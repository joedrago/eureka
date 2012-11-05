// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekSyntax.h"
#include "ekContext.h"

#include "ekLexer.h"

#include <string.h>
#include <stdio.h>

ekSyntax *ekSyntaxCreate(struct ekContext *Y, yU32 type, int line)
{
    ekSyntax *syntax = (ekSyntax *)ekAlloc(sizeof(ekSyntax));
    syntax->type = type;
    syntax->line = line;
    return syntax;
}

static void ekSyntaxElementClear(struct ekContext *Y, ekSyntaxElement *e)
{
    if(e->p)
    {
        ekSyntaxDestroy(Y, e->p);
    }
    if(e->s)
    {
        ekFree(e->s);
    }
    if(e->a)
    {
        ekArrayDestroy(Y, &e->a, (ekDestroyCB)ekSyntaxDestroy);
    }
}

void ekSyntaxDestroy(struct ekContext *Y, ekSyntax *syntax)
{
    ekSyntaxElementClear(Y, &syntax->v);
    ekSyntaxElementClear(Y, &syntax->l);
    ekSyntaxElementClear(Y, &syntax->r);

    ekFree(syntax);
}

ekSyntax *ekSyntaxCreateKString(struct ekContext *Y, struct ekToken *token)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_KSTRING, token->line);
    syntax->v.s = ekTokenToString(Y, token);
    return syntax;
}

ekSyntax *ekSyntaxCreateKInt(struct ekContext *Y, struct ekToken *token, yU32 opts)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_KINT, token->line);
    syntax->v.i = ekTokenToInt(Y, token);
    if(opts & CKO_NEGATIVE)
    {
        syntax->v.i *= -1;
    }
    return syntax;
}

ekSyntax *ekSyntaxCreateKFloat(struct ekContext *Y, struct ekToken *token, yU32 opts)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_KFLOAT, token->line);
    syntax->v.f = ekTokenToFloat(Y, token);
    if(opts & CKO_NEGATIVE)
    {
        syntax->v.f *= -1;
    }
    return syntax;
}

ekSyntax *ekSyntaxCreateIdentifier(struct ekContext *Y, struct ekToken *token)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_IDENTIFIER, token->line);
    syntax->v.s = ekTokenToString(Y, token);
    return syntax;
}

ekSyntax *ekSyntaxCreateIndex(struct ekContext *Y, ekSyntax *array, ekSyntax *index, yBool pushThis)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_INDEX, array->line);
    syntax->v.i = pushThis;
    syntax->l.p = array;
    syntax->r.p = index;
    return syntax;
}

ekSyntax *ekSyntaxCreateNull(struct ekContext *Y, int line)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_NULL, line);
    return syntax;
}

ekSyntax *ekSyntaxCreateThis(struct ekContext *Y, int line)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_THIS, line);
    return syntax;
}

ekSyntax *ekSyntaxCreateList(struct ekContext *Y, yU32 type, ekSyntax *firstExpr)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, type, (firstExpr) ? firstExpr->line : 0);
    syntax->v.a = NULL;
    if(firstExpr)
    {
        ekArrayPush(Y, &syntax->v.a, firstExpr);
    }
    return syntax;
}

ekSyntax *ekSyntaxListAppend(struct ekContext *Y, ekSyntax *list, ekSyntax *expr, yU32 flags)
{
    if(expr != NULL)
    {
        int index;
        if(!list->line)
        {
            list->line = expr->line;
        }
        index = ekArrayPush(Y, &list->v.a, expr);
        if((flags & YSLF_AUTOLITERAL) && (index > 0))
        {
            ekSyntax *toLiteral = list->v.a[index - 1];
            if(toLiteral->type == YST_IDENTIFIER)
            {
                toLiteral->type = YST_KSTRING;
            }
        }
    }
    return list;
}

ekSyntax *ekSyntaxCreateCall(struct ekContext *Y, ekSyntax *func, ekSyntax *args)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_CALL, func->line);
    syntax->v.p = func;
    syntax->r.p = args;
    return syntax;
}

ekSyntax *ekSyntaxCreateStringFormat(struct ekContext *Y, ekSyntax *format, ekSyntax *args)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_STRINGFORMAT, format->line);
    syntax->l.p = format;
    syntax->r.p = args;
    return syntax;
}

ekSyntax *ekSyntaxCreateUnary(struct ekContext *Y, yU32 type, ekSyntax *expr)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, type, expr->line);
    syntax->v.p = expr;
    return syntax;
}

ekSyntax *ekSyntaxCreateBinary(struct ekContext *Y, yU32 type, ekSyntax *l, ekSyntax *r, yBool compound)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, type, l->line);
    syntax->l.p = l;
    syntax->r.p = r;
    syntax->v.i = compound;
    return syntax;
}

ekSyntax *ekSyntaxCreateStatementExpr(struct ekContext *Y, ekSyntax *expr)
{
    ekSyntax *syntax;

    if((expr->type == YST_EXPRESSIONLIST) && (ekArraySize(Y, &expr->v.a) == 0))
    {
        // An empty statement. Without PYTHON_SCOPING, this only happens
        // when there are multiple semicolons in a row. However, when
        // it is enabled, one is created for every blank line!
        ekSyntaxDestroy(Y, expr);
        return NULL;
    }

    syntax = ekSyntaxCreate(Y, YST_STATEMENT_EXPR, expr->line);
    syntax->v.p = expr;
    return syntax;
}

ekSyntax *ekSyntaxCreateAssignment(struct ekContext *Y, ekSyntax *l, ekSyntax *r)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_ASSIGNMENT, l->line);
    syntax->l.p = l;
    syntax->r.p = r;
    syntax->line = l->line;
    return syntax;
}

ekSyntax *ekSyntaxCreateInherits(struct ekContext *Y, ekSyntax *l, ekSyntax *r)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_INHERITS, l->line);
    syntax->l.p = l;
    syntax->r.p = r;
    return syntax;
}

ekSyntax *ekSyntaxMarkVar(struct ekContext *Y, ekSyntax *identList)
{
    identList->v.i = 1;
    return identList;
}

ekSyntax *ekSyntaxCreateBreak(struct ekContext *Y, int line)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_BREAK, line);
    return syntax;
}

ekSyntax *ekSyntaxCreateReturn(struct ekContext *Y, ekSyntax *expr)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_RETURN, expr->line);
    syntax->v.p = expr;
    return syntax;
}

ekSyntax *ekSyntaxCreateIfElse(struct ekContext *Y, ekSyntax *cond, ekSyntax *ifBody, ekSyntax *elseBody, yBool ternary)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_IFELSE, cond->line);
    syntax->v.i = ternary;
    syntax->v.p = cond;
    syntax->l.p = ifBody;
    syntax->r.p = elseBody;
    return syntax;
}

ekSyntax *ekSyntaxCreateWhile(struct ekContext *Y, ekSyntax *cond, ekSyntax *body)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_WHILE, cond->line);
    syntax->v.p = cond;
    syntax->r.p = body;
    return syntax;
}

ekSyntax *ekSyntaxCreateFor(struct ekContext *Y, ekSyntax *vars, ekSyntax *iter, ekSyntax *body)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_FOR, vars->line);
    syntax->v.p = vars;
    syntax->l.p = iter;
    syntax->r.p = body;
    return syntax;
}

ekSyntax *ekSyntaxCreateFunctionDecl(struct ekContext *Y, struct ekToken *name, ekSyntax *args, ekSyntax *body, int line)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_FUNCTION, line);
    syntax->v.s = (name) ? ekTokenToString(Y, name) : NULL;
    syntax->l.p = args;
    syntax->r.p = body;
    syntax->line = line;
    return syntax;
}

ekSyntax *ekSyntaxCreateFunctionArgs(struct ekContext *Y, ekSyntax *args, struct ekToken *varargs)
{
    int line = (args) ? args->line : (varargs) ? varargs->line : 0;
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_FUNCTION_ARGS, line);
    syntax->l.p = args;
    syntax->v.s = (varargs) ? ekTokenToString(Y, varargs) : NULL;
    return syntax;
}

ekSyntax *ekSyntaxCreateScope(struct ekContext *Y, ekSyntax *body)
{
    ekSyntax *syntax = ekSyntaxCreate(Y, YST_SCOPE, body->line);
    syntax->v.p = body;
    return syntax;
}
