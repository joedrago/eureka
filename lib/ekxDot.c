// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekxDot.h"

#include "ekArray.h"
#include "ekSyntax.h"

#include <stdio.h>
#include <string.h>

#define REC_CHILD(CHILD) ekSyntaxDotRecurse(E, CHILD, childLineOpts, syntax);
#define REC_ARRAY(ARRAY) ekSyntaxDotRecurseArray(E, ARRAY, childLineOpts, syntax);

static void ekSyntaxDotRecurse(struct ekContext * E, ekSyntax * syntax, const char * myLineOpts, ekSyntax * parent);

static void ekSyntaxDotRecurseArray(struct ekContext * E, ekSyntax ** a, const char * myLineOpts, ekSyntax * syntax)
{
    ekS32 i;
    const char * childLineOpts = myLineOpts;
    for (i = 0; i < ekArraySize(E, &a); i++) {
        REC_CHILD(a[i]);
    }
}

static void ekSyntaxDotRecurse(struct ekContext * E, ekSyntax * syntax, const char * myLineOpts, ekSyntax * parent)
{
    char label[512];
    const char * myOpts = "shape=ellipse";
    const char * childLineOpts = NULL;

    if (!myLineOpts) {
        myLineOpts = "style=solid";
    }

    sprintf(label, "??: %d", syntax->type);

    switch (syntax->type) {

        case EST_KSTRING:
        {
            char * c;
            myOpts = "shape=house,color=blueviolet";
            sprintf(label, "K: \\\"%s\\\"", syntax->v.s);
            c = label;
            while (*c) {
                switch (*c) {
                    case '\n':
                    case '\t':
                    case '\r':
                        *c = ' ';
                        break;
                }
                c++; // this never gets old.
            }
            break;
        }

        case EST_KINT:
            myOpts = "shape=house,color=blueviolet";
            sprintf(label, "%d", syntax->v.i);
            break;

        case EST_KFLOAT:
            myOpts = "shape=house,color=blueviolet";
            sprintf(label, "%3.3f", syntax->v.f);
            break;

        case EST_BOOL:
            myOpts = "shape=house,color=blueviolet";
            if (syntax->v.i)
                sprintf(label, "true");
            else
                sprintf(label, "false");
            break;

        case EST_IDENTIFIER:
            myOpts = "shape=house";
            sprintf(label, "V: \\\"%s\\\"", syntax->v.s);
            break;

        case EST_INDEX:
            strcpy(label, "Index");
            childLineOpts = "style=dotted,label=array";
            REC_CHILD(syntax->l.p);
            childLineOpts = "style=dotted,label=index";
            REC_CHILD(syntax->r.p);
            break;

        case EST_STATEMENTLIST:
            myOpts = "shape=box";
            strcpy(label, "StatementList");
            REC_ARRAY(syntax->v.a);
            break;

        case EST_EXPRESSIONLIST:
            if (ekArraySize(E, &syntax->v.a)) {
                myOpts = "shape=egg";
                strcpy(label, "ExprList");
            } else {
                myOpts = "shape=egg, color=red";
                strcpy(label, "Stub");
            }
            REC_ARRAY(syntax->v.a);
            break;

        case EST_IDENTIFIERLIST:
            strcpy(label, "IdentList");
            if (syntax->v.i) {
                strcat(label, " [var]");
            }
            REC_ARRAY(syntax->v.a);
            break;

        case EST_ARRAY:
            strcpy(label, "Array");
            if(syntax->v.p) {
                REC_CHILD(syntax->v.p);
            }
            break;

        case EST_MAP:
            strcpy(label, "Map");
            if(syntax->v.p) {
                REC_CHILD(syntax->v.p);
            }
            break;

        case EST_CALL:
            myOpts = "shape=invtrapezium,color=blue";
            sprintf(label, "Call: %s(%d)", (syntax->v.s) ? syntax->v.s : "CFUNC", (ekS32)ekArraySize(E, &syntax->r.p->v.a));
            childLineOpts = "style=dotted,label=args";
            REC_CHILD(syntax->r.p);
            break;

        case EST_STRINGFORMAT:
            strcpy(label, "% (format)");
            childLineOpts = "style=dotted,label=format";
            REC_CHILD(syntax->l.p);
            childLineOpts = "style=dotted,label=args";
            REC_CHILD(syntax->r.p);
            break;

        case EST_NULL:
            myOpts = "shape=house,color=blueviolet";
            strcpy(label, "null");
            break;

        case EST_THIS:
            myOpts = "shape=house,color=blueviolet";
            strcpy(label, "this");
            break;

        case EST_NOT:
            strcpy(label, "Not (logical)");
            REC_CHILD(syntax->v.p);
            break;

        case EST_BITWISE_NOT:
            strcpy(label, "Not (bitwise)");
            REC_CHILD(syntax->v.p);
            break;

        case EST_BITWISE_XOR:
            if (syntax->v.i) {
                strcpy(label, "^= (xor)");
            } else {
                strcpy(label, "xor");
            }
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_BITWISE_AND:
            if (syntax->v.i) {
                strcpy(label, "&= (bitwise and)");
            } else {
                strcpy(label, "And (bitwise)");
            }
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_BITWISE_OR:
            if (syntax->v.i) {
                strcpy(label, "|= (bitwise or)");
            } else {
                strcpy(label, "Or (bitwise)");
            }
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_SHIFTLEFT:
            if (syntax->v.i) {
                strcpy(label, "<<=");
            } else {
                strcpy(label, "<<");
            }
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_SHIFTRIGHT:
            if (syntax->v.i) {
                strcpy(label, ">>=");
            } else {
                strcpy(label, ">>");
            }
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_AND:
            strcpy(label, "And (logical)");
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_OR:
            strcpy(label, "Or (logical)");
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_ADD:
            if (syntax->v.i) {
                strcpy(label, "+=");
            } else {
                strcpy(label, "+");
            }
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_SUB:
            if (syntax->v.i) {
                strcpy(label, "-=");
            } else {
                strcpy(label, "-");
            }
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_MUL:
            if (syntax->v.i) {
                strcpy(label, "*=");
            } else {
                strcpy(label, "*");
            }
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_DIV:
            if (syntax->v.i) {
                strcpy(label, "/=");
            } else {
                strcpy(label, "/");
            }
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_STATEMENT_EXPR:
            strcpy(label, "StatementExpr");
            REC_CHILD(syntax->v.p);
            break;

        case EST_ASSIGNMENT:
            strcpy(label, "=");
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_CMP:
            strcpy(label, "cmp");
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_EQUALS:
            strcpy(label, "==");
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_NOTEQUALS:
            strcpy(label, "!=");
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_GREATERTHAN:
            strcpy(label, ">");
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_GREATERTHANOREQUAL:
            strcpy(label, ">=");
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_LESSTHAN:
            strcpy(label, "<");
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_LESSTHANOREQUAL:
            strcpy(label, "<=");
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_INHERITS:
            strcpy(label, "Inherits");
            REC_CHILD(syntax->l.p);
            REC_CHILD(syntax->r.p);
            break;

        case EST_BREAK:
            strcpy(label, "Break");
            break;

        case EST_RETURN:
            strcpy(label, "Return");
            if (syntax->v.p)
                REC_CHILD(syntax->v.p);
            break;

        case EST_IFELSE:
            strcpy(label, "IfElse");
            childLineOpts = "style=dotted,label=condition";
            REC_CHILD(syntax->v.p);
            childLineOpts = "style=dotted,label=true";
            REC_CHILD(syntax->l.p);
            if (syntax->r.p) {
                childLineOpts = "style=dotted,label=false";
                REC_CHILD(syntax->r.p);
            }
            break;

        case EST_WHILE:
            strcpy(label, "while");
            childLineOpts = "style=dotted,label=cond";
            REC_CHILD(syntax->v.p);
            childLineOpts = "label=body";
            REC_CHILD(syntax->r.p);
            break;

        case EST_FOR:
            strcpy(label, "for");
            childLineOpts = "style=dotted,label=vars";
            REC_CHILD(syntax->v.p);
            childLineOpts = "style=dotted,label=iter";
            REC_CHILD(syntax->l.p);
            childLineOpts = "label=body";
            REC_CHILD(syntax->r.p);
            break;

        case EST_FUNCTION:
            sprintf(label, "Function: \\\"%s\\\"", syntax->v.s);
            childLineOpts = "style=dotted,label=args";
            REC_CHILD(syntax->l.p);
            childLineOpts = "label=body";
            REC_CHILD(syntax->r.p);
            break;

        case EST_FUNCTION_ARGS:
            sprintf(label, "Function Args");
            childLineOpts = "style=dotted,label=args";
            REC_CHILD(syntax->l.p);
            break;

        case EST_SCOPE:
            sprintf(label, "scope");
            childLineOpts = "label=body";
            REC_CHILD(syntax->v.p);
            break;
    }

    if (parent) {
        printf("s%p -> s%p [%s]\n", parent, syntax, myLineOpts);
    }
    printf("s%p [label=\"%s\",%s]\n", syntax, label, myOpts);
}

void ekSyntaxDot(struct ekContext * E, struct ekSyntax * syntax)
{
    printf("digraph AST {\n");
    ekSyntaxDotRecurse(E, syntax, NULL, NULL);
    printf("}\n");
}
