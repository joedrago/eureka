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

ekSyntax *ekSyntaxCreate(struct ekContext *E, ekU32 type, int line)
{
    ekSyntax *syntax = (ekSyntax *)ekAlloc(sizeof(ekSyntax));
    syntax->type = type;
    syntax->line = line;
    return syntax;
}

static void ekSyntaxElementClear(struct ekContext *E, ekSyntaxElement *e)
{
    if(e->p)
    {
        ekSyntaxDestroy(E, e->p);
    }
    if(e->s)
    {
        ekFree(e->s);
    }
    if(e->a)
    {
        ekArrayDestroy(E, &e->a, (ekDestroyCB)ekSyntaxDestroy);
    }
}

void ekSyntaxDestroy(struct ekContext *E, ekSyntax *syntax)
{
    ekSyntaxElementClear(E, &syntax->v);
    ekSyntaxElementClear(E, &syntax->l);
    ekSyntaxElementClear(E, &syntax->r);

    ekFree(syntax);
}

ekSyntax *ekSyntaxCreateKString(struct ekContext *E, struct ekToken *token)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_KSTRING, token->line);
    syntax->v.s = ekTokenToString(E, token);
    return syntax;
}

ekSyntax *ekSyntaxCreateKInt(struct ekContext *E, struct ekToken *token, ekU32 opts)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_KINT, token->line);
    syntax->v.i = ekTokenToInt(E, token);
    if(opts & CKO_NEGATIVE)
    {
        syntax->v.i *= -1;
    }
    return syntax;
}

ekSyntax *ekSyntaxCreateKFloat(struct ekContext *E, struct ekToken *token, ekU32 opts)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_KFLOAT, token->line);
    syntax->v.f = ekTokenToFloat(E, token);
    if(opts & CKO_NEGATIVE)
    {
        syntax->v.f *= -1;
    }
    return syntax;
}

ekSyntax *ekSyntaxCreateIdentifier(struct ekContext *E, struct ekToken *token)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_IDENTIFIER, token->line);
    syntax->v.s = ekTokenToString(E, token);
    return syntax;
}

ekSyntax *ekSyntaxCreateIndex(struct ekContext *E, ekSyntax *array, ekSyntax *index, ekBool pushThis)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_INDEX, array->line);
    syntax->v.i = pushThis;
    syntax->l.p = array;
    syntax->r.p = index;
    return syntax;
}

ekSyntax *ekSyntaxCreateNull(struct ekContext *E, int line)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_NULL, line);
    return syntax;
}

ekSyntax *ekSyntaxCreateThis(struct ekContext *E, int line)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_THIS, line);
    return syntax;
}

ekSyntax *ekSyntaxCreateList(struct ekContext *E, ekU32 type, ekSyntax *firstExpr)
{
    ekSyntax *syntax = ekSyntaxCreate(E, type, (firstExpr) ? firstExpr->line : 0);
    syntax->v.a = NULL;
    if(firstExpr)
    {
        ekArrayPush(E, &syntax->v.a, firstExpr);
    }
    return syntax;
}

ekSyntax *ekSyntaxListAppend(struct ekContext *E, ekSyntax *list, ekSyntax *expr, ekU32 flags)
{
    if(expr != NULL)
    {
        int index;
        if(!list->line)
        {
            list->line = expr->line;
        }
        index = ekArrayPush(E, &list->v.a, expr);
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

ekSyntax *ekSyntaxCreateCall(struct ekContext *E, ekSyntax *func, ekSyntax *args)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_CALL, func->line);
    syntax->v.p = func;
    syntax->r.p = args;
    return syntax;
}

ekSyntax *ekSyntaxCreateStringFormat(struct ekContext *E, ekSyntax *format, ekSyntax *args)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_STRINGFORMAT, format->line);
    syntax->l.p = format;
    syntax->r.p = args;
    return syntax;
}

ekSyntax *ekSyntaxCreateUnary(struct ekContext *E, ekU32 type, ekSyntax *expr)
{
    ekSyntax *syntax = ekSyntaxCreate(E, type, expr->line);
    syntax->v.p = expr;
    return syntax;
}

ekSyntax *ekSyntaxCreateBinary(struct ekContext *E, ekU32 type, ekSyntax *l, ekSyntax *r, ekBool compound)
{
    ekSyntax *syntax = ekSyntaxCreate(E, type, l->line);
    syntax->l.p = l;
    syntax->r.p = r;
    syntax->v.i = compound;
    return syntax;
}

ekSyntax *ekSyntaxCreateStatementExpr(struct ekContext *E, ekSyntax *expr)
{
    ekSyntax *syntax;

    if((expr->type == YST_EXPRESSIONLIST) && (ekArraySize(E, &expr->v.a) == 0))
    {
        // An empty statement. Without PYTHON_SCOPING, this only happens
        // when there are multiple semicolons in a row. However, when
        // it is enabled, one is created for every blank line!
        ekSyntaxDestroy(E, expr);
        return NULL;
    }

    syntax = ekSyntaxCreate(E, YST_STATEMENT_EXPR, expr->line);
    syntax->v.p = expr;
    return syntax;
}

ekSyntax *ekSyntaxCreateAssignment(struct ekContext *E, ekSyntax *l, ekSyntax *r)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_ASSIGNMENT, l->line);
    syntax->l.p = l;
    syntax->r.p = r;
    syntax->line = l->line;
    return syntax;
}

ekSyntax *ekSyntaxCreateInherits(struct ekContext *E, ekSyntax *l, ekSyntax *r)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_INHERITS, l->line);
    syntax->l.p = l;
    syntax->r.p = r;
    return syntax;
}

ekSyntax *ekSyntaxMarkVar(struct ekContext *E, ekSyntax *identList)
{
    identList->v.i = 1;
    return identList;
}

ekSyntax *ekSyntaxCreateBreak(struct ekContext *E, int line)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_BREAK, line);
    return syntax;
}

ekSyntax *ekSyntaxCreateReturn(struct ekContext *E, ekSyntax *expr)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_RETURN, expr->line);
    syntax->v.p = expr;
    return syntax;
}

ekSyntax *ekSyntaxCreateIfElse(struct ekContext *E, ekSyntax *cond, ekSyntax *ifBody, ekSyntax *elseBody, ekBool ternary)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_IFELSE, cond->line);
    syntax->v.i = ternary;
    syntax->v.p = cond;
    syntax->l.p = ifBody;
    syntax->r.p = elseBody;
    return syntax;
}

ekSyntax *ekSyntaxCreateWhile(struct ekContext *E, ekSyntax *cond, ekSyntax *body)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_WHILE, cond->line);
    syntax->v.p = cond;
    syntax->r.p = body;
    return syntax;
}

ekSyntax *ekSyntaxCreateFor(struct ekContext *E, ekSyntax *vars, ekSyntax *iter, ekSyntax *body)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_FOR, vars->line);
    syntax->v.p = vars;
    syntax->l.p = iter;
    syntax->r.p = body;
    return syntax;
}

ekSyntax *ekSyntaxCreateFunctionDecl(struct ekContext *E, struct ekToken *name, ekSyntax *args, ekSyntax *body, int line)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_FUNCTION, line);
    syntax->v.s = (name) ? ekTokenToString(E, name) : NULL;
    syntax->l.p = args;
    syntax->r.p = body;
    syntax->line = line;
    return syntax;
}

ekSyntax *ekSyntaxCreateFunctionArgs(struct ekContext *E, ekSyntax *args, struct ekToken *varargs)
{
    int line = (args) ? args->line : (varargs) ? varargs->line : 0;
    ekSyntax *syntax = ekSyntaxCreate(E, YST_FUNCTION_ARGS, line);
    syntax->l.p = args;
    syntax->v.s = (varargs) ? ekTokenToString(E, varargs) : NULL;
    return syntax;
}

ekSyntax *ekSyntaxCreateScope(struct ekContext *E, ekSyntax *body)
{
    ekSyntax *syntax = ekSyntaxCreate(E, YST_SCOPE, body->line);
    syntax->v.p = body;
    return syntax;
}
