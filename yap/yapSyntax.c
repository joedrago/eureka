#include "yapSyntax.h"

#include "yapLexer.h"

#include <string.h>
#include <stdio.h>

yapSyntax * yapSyntaxCreate(yU32 type)
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
    syntax->r.p = args;
    return syntax;
}

yapSyntax * yapSyntaxCreateStringFormat(yapSyntax *format, yapSyntax *args)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_STRINGFORMAT);
    syntax->l.p = format;
    syntax->r.p = args;
    return syntax;
}

yapSyntax * yapSyntaxCreateUnary(yU32 type, yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(type);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax * yapSyntaxCreateBinary(yU32 type, yapSyntax *l, yapSyntax *r)
{
    yapSyntax *syntax = yapSyntaxCreate(type);
    syntax->l.p = l;
    syntax->r.p = r;
    return syntax;
}

yapSyntax * yapSyntaxCreateStatementExpr(yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_STATEMENT_EXPR);
    syntax->v.p = expr;
    return syntax;
}

yapSyntax * yapSyntaxCreateAssignment(yapSyntax *l, yapSyntax *r)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_ASSIGNMENT);
    syntax->l.p = l;
    syntax->r.p = r;
    return syntax;
}

yapSyntax * yapSyntaxCreateVar(yapSyntax *expr)
{
    yapSyntax *syntax = yapSyntaxCreate(YST_VAR);
    syntax->v.p = expr;
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

#define REC_CHILD(CHILD) yapSyntaxDotRecurse(CHILD, childLineOpts, syntax);
#define REC_ARRAY(ARRAY) yapSyntaxDotRecurseArray(ARRAY, childLineOpts, syntax);

static void yapSyntaxDotRecurse(yapSyntax *syntax, const char *myLineOpts, yapSyntax *parent);

static void yapSyntaxDotRecurseArray(yapArray *a, const char *myLineOpts, yapSyntax *syntax)
{
    int i;
    const char *childLineOpts = myLineOpts;
    for(i=0; i<a->count; i++)
    {
        REC_CHILD((yapSyntax*)a->data[i]);
    }
}

static void yapSyntaxDotRecurse(yapSyntax *syntax, const char *myLineOpts, yapSyntax *parent)
{
    char label[512];
    const char *myOpts = "shape=ellipse";
    const char *childLineOpts = NULL;

    if(!myLineOpts)
        myLineOpts = "style=solid";

    sprintf(label, "??: %d", syntax->type);

    switch(syntax->type)
    {

    case YST_KSTRING:
    {
        myOpts = "shape=house,color=blueviolet";
        sprintf(label, "\\\"%s\\\"", syntax->v.s);
    }
    break;

    case YST_KINT:
    {
        myOpts = "shape=house,color=blueviolet";
        sprintf(label, "%d", syntax->v.i);
    }
    break;

    case YST_IDENTIFIER:
    {
        myOpts = "shape=house";
        sprintf(label, "\\\"%s\\\"", syntax->v.s);
    }
    break;

    case YST_STATEMENTLIST:
    {
        myOpts = "shape=box";
        strcpy(label,"StatementList");
        REC_ARRAY(syntax->v.a);
    }
    break;

    case YST_EXPRESSIONLIST:
    {
        if(syntax->v.a->count)
        {
            myOpts = "shape=egg";
            strcpy(label, "ExprList");
        }
        else
        {
            myOpts = "shape=egg, color=red";
            strcpy(label, "Stub");
        }
        REC_ARRAY(syntax->v.a);
    }
    break;

    case YST_IDENTIFIERLIST:
    {
        strcpy(label, "IdentList");
        REC_ARRAY(syntax->v.a);
    }
    break;

    case YST_CALL:
    {
        myOpts = "shape=invtrapezium,color=blue";
        sprintf(label, "Call: %s(%d)", syntax->v.s, syntax->r.p->v.a->count);
        childLineOpts = "style=dotted,label=args";
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_STRINGFORMAT:
    {
        strcpy(label, "% (format)");
        childLineOpts = "style=dotted,label=format";
        REC_CHILD(syntax->l.p);
        childLineOpts = "style=dotted,label=args";
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_NULL:
    {
        myOpts = "shape=house,color=blueviolet";
        strcpy(label, "null");
    }
    break;

    case YST_TOSTRING:
    {
        strcpy(label,"ToString");
        REC_CHILD(syntax->v.p);
    }
    break;

    case YST_TOINT:
    {
        strcpy(label,"ToInt");
        REC_CHILD(syntax->v.p);
    }
    break;

    case YST_ADD:
    {
        strcpy(label, "+");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_SUB:
    {
        strcpy(label, "-");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_MUL:
    {
        strcpy(label, "*");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_DIV:
    {
        strcpy(label, "/");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_STATEMENT_EXPR:
    {
        strcpy(label, "StatementExpr");
        REC_CHILD(syntax->v.p);
    }
    break;

    case YST_ASSIGNMENT:
    {
        strcpy(label, "=");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_VAR:
    {
        strcpy(label, "Var");
        REC_CHILD(syntax->v.p);
    }
    break;

    case YST_RETURN:
    {
        strcpy(label, "Return");
        REC_CHILD(syntax->v.p);
    }
    break;

    case YST_IFELSE:
    {
        strcpy(label, "IfElse");
        childLineOpts = "style=dotted,label=condition";
        REC_CHILD(syntax->v.p);
        childLineOpts = "style=dotted,label=true";
        REC_CHILD(syntax->l.p);
        if(syntax->r.p)
        {
            childLineOpts = "style=dotted,label=false";
            REC_CHILD(syntax->r.p);
        }
    }
    break;

    case YST_LOOP:
    {
        strcpy(label, "while");
        childLineOpts = "style=dotted,label=cond";
        REC_CHILD(syntax->v.p);
        childLineOpts = "label=body";
        REC_CHILD(syntax->l.p);
    }
    break;

    case YST_FUNCTION:
    {
        sprintf(label, "Function: \\\"%s\\\"", syntax->v.s);
        childLineOpts = "style=dotted,label=args";
        REC_CHILD(syntax->l.p);
        childLineOpts = "label=body";
        REC_CHILD(syntax->r.p);
    }
    break;
    };

    if(parent)
        printf("s%p -> s%p [%s]\n", parent, syntax, myLineOpts);
    printf("s%p [label=\"%s\",%s]\n", syntax, label, myOpts);
}

void yapSyntaxDot(yapSyntax *syntax)
{
    printf("digraph AST {\n");
    yapSyntaxDotRecurse(syntax, NULL, NULL);
    printf("}\n");
}

