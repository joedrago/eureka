#include "yapxDot.h"

#include "yapSyntax.h"

#include <stdio.h>
#include <string.h>

#define REC_CHILD(CHILD) yapSyntaxDotRecurse(CHILD, childLineOpts, syntax);
#define REC_ARRAY(ARRAY) yapSyntaxDotRecurseArray(ARRAY, childLineOpts, syntax);

static void yapSyntaxDotRecurse(yapSyntax *syntax, const char *myLineOpts, yapSyntax *parent);

static void yapSyntaxDotRecurseArray(yapArray *a, const char *myLineOpts, yapSyntax *syntax)
{
    int i;
    const char *childLineOpts = myLineOpts;
    for(i = 0; i < a->count; i++)
    {
        REC_CHILD((yapSyntax *)a->data[i]);
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
        char *c;
        myOpts = "shape=house,color=blueviolet";
        sprintf(label, "K: \\\"%s\\\"", syntax->v.s);
        c = label;
        while(*c)
        {
            switch(*c)
            {
            case '\n':
            case '\t':
            case '\r':
                *c = ' ';
                break;
            };
            c++; // this never gets old.
        };
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
        sprintf(label, "V: \\\"%s\\\"", syntax->v.s);
    }
    break;

    case YST_INDEX:
    {
        strcpy(label, "Index");
        childLineOpts = "style=dotted,label=array";
        REC_CHILD(syntax->l.p);
        childLineOpts = "style=dotted,label=index";
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_STATEMENTLIST:
    {
        myOpts = "shape=box";
        strcpy(label, "StatementList");
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
        sprintf(label, "Call: %s(%d)", (syntax->v.s) ? syntax->v.s : "CFUNC", syntax->r.p->v.a->count);
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

    case YST_THIS:
    {
        myOpts = "shape=house,color=blueviolet";
        strcpy(label, "this");
    }
    break;

    case YST_TOSTRING:
    {
        strcpy(label, "ToString");
        REC_CHILD(syntax->v.p);
    }
    break;

    case YST_TOINT:
    {
        strcpy(label, "ToInt");
        REC_CHILD(syntax->v.p);
    }
    break;

    case YST_NOT:
    {
        strcpy(label, "Not (logical)");
        REC_CHILD(syntax->v.p);
    }
    break;

    case YST_BITWISE_NOT:
    {
        strcpy(label, "Not (bitwise)");
        REC_CHILD(syntax->v.p);
    }
    break;

    case YST_BITWISE_XOR:
    {
        if(syntax->v.i)
            strcpy(label, "^= (xor)");
        else
            strcpy(label, "xor");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_BITWISE_AND:
    {
        if(syntax->v.i)
            strcpy(label, "&= (bitwise and)");
        else
        strcpy(label, "And (bitwise)");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_BITWISE_OR:
    {
        if(syntax->v.i)
            strcpy(label, "|= (bitwise or)");
        else
            strcpy(label, "Or (bitwise)");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_SHIFTLEFT:
    {
        if(syntax->v.i)
            strcpy(label, "<<=");
        else
            strcpy(label, "<<");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_SHIFTRIGHT:
    {
        if(syntax->v.i)
            strcpy(label, ">>=");
        else
            strcpy(label, ">>");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_AND:
    {
        strcpy(label, "And (logical)");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_OR:
    {
        strcpy(label, "Or (logical)");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_ADD:
    {
        if(syntax->v.i)
            strcpy(label, "+=");
        else
            strcpy(label, "+");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_SUB:
    {
        if(syntax->v.i)
            strcpy(label, "-=");
        else
            strcpy(label, "-");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_MUL:
    {
        if(syntax->v.i)
            strcpy(label, "*=");
        else
            strcpy(label, "*");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_DIV:
    {
        if(syntax->v.i)
            strcpy(label, "/=");
        else
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

    case YST_CMP:
    {
        strcpy(label, "cmp");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_EQUALS:
    {
        strcpy(label, "==");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_NOTEQUALS:
    {
        strcpy(label, "!=");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_GREATERTHAN:
    {
        strcpy(label, ">");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_GREATERTHANOREQUAL:
    {
        strcpy(label, ">=");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_LESSTHAN:
    {
        strcpy(label, "<");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_LESSTHANOREQUAL:
    {
        strcpy(label, "<=");
        REC_CHILD(syntax->l.p);
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_INHERITS:
    {
        strcpy(label, "Inherits");
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

    case YST_WHILE:
    {
        strcpy(label, "while");
        childLineOpts = "style=dotted,label=cond";
        REC_CHILD(syntax->v.p);
        childLineOpts = "label=body";
        REC_CHILD(syntax->r.p);
    }
    break;

    case YST_FOR:
    {
        strcpy(label, "for");
        childLineOpts = "style=dotted,label=vars";
        REC_CHILD(syntax->v.p);
        childLineOpts = "style=dotted,label=iter";
        REC_CHILD(syntax->l.p);
        childLineOpts = "label=body";
        REC_CHILD(syntax->r.p);
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

    case YST_FUNCTION_ARGS:
    {
        sprintf(label, "Function Args");
        childLineOpts = "style=dotted,label=args";
        REC_CHILD(syntax->l.p);
    }
    break;

    case YST_SCOPE:
    {
        sprintf(label, "SCOPE");
        childLineOpts = "label=body";
        REC_CHILD(syntax->v.p);
    }
    break;
    };

    if(parent)
        printf("s%p -> s%p [%s]\n", parent, syntax, myLineOpts);
    printf("s%p [label=\"%s\",%s]\n", syntax, label, myOpts);
}

void yapSyntaxDot(struct yapSyntax *syntax)
{
    printf("digraph AST {\n");
    yapSyntaxDotRecurse(syntax, NULL, NULL);
    printf("}\n");
}
