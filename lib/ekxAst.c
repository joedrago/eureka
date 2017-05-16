// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekxAst.h"

#include "ekArray.h"
#include "ekSyntax.h"

#include <stdio.h>
#include <string.h>

#define REC_CHILD(CHILD, DEPTH) ekSyntaxAstRecurse(E, CHILD, DEPTH);
#define REC_ARRAY(ARRAY, DEPTH) ekSyntaxAstRecurseArray(E, ARRAY, DEPTH);
#define PD() for (depthLoop = 0; depthLoop < depth; ++depthLoop) { printf("  "); } //printf(" * ");

static void ekSyntaxAstRecurse(struct ekContext * E, ekSyntax * syntax, int depth);

static void ekSyntaxAstRecurseArray(struct ekContext * E, ekSyntax ** a, int depth)
{
    ekS32 i;
    for (i = 0; i < ekArraySize(E, &a); i++) {
        REC_CHILD(a[i], depth);
    }
}

static void ekSyntaxAstRecurse(struct ekContext * E, ekSyntax * syntax, int depth)
{
    char temp[512];
    int depthLoop;

    switch (syntax->type) {

        case EST_KSTRING:
        {
            char * c;
            strncpy(temp, syntax->v.s, sizeof(temp));
            temp[sizeof(temp) - 1] = 0;
            c = temp;
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
            PD(); printf("K: \"%s\"\n", temp);
            break;
        }

        case EST_KINT:
            PD(); printf("%d\n", syntax->v.i);
            break;

        case EST_KFLOAT:
            PD(); printf("%3.3f\n", syntax->v.f);
            break;

        case EST_BOOL:
            if (syntax->v.i) {
                PD(); printf("true\n");
            } else {
                PD(); printf("false\n");
            }
            break;

        case EST_IDENTIFIER:
            PD(); printf("V: \"%s\"\n", syntax->v.s);
            break;

        case EST_INDEX:
            PD(); printf("Index\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_STATEMENTLIST:
            PD(); printf("StatementList\n");
            REC_ARRAY(syntax->v.a, depth + 1);
            break;

        case EST_EXPRESSIONLIST:
            if (ekArraySize(E, &syntax->v.a)) {
                PD(); printf("ExprList\n");
            } else {
                PD(); printf("Stub\n");
            }
            REC_ARRAY(syntax->v.a, depth + 1);
            break;

        case EST_IDENTIFIERLIST:
            PD(); printf("IdentList");
            if (syntax->v.i) {
                printf(" [var]");
            }
            printf("\n");
            REC_ARRAY(syntax->v.a, depth + 1);
            break;

        case EST_ARRAY:
            PD(); printf("Array\n");
            REC_CHILD(syntax->v.p, depth + 1);
            break;

        case EST_MAP:
            PD(); printf("Map\n");
            REC_CHILD(syntax->v.p, depth + 1);
            break;

        case EST_CALL:
            PD(); printf("Call: %s(%d)\n", (syntax->v.s) ? syntax->v.s : "CFUNC", (ekS32)ekArraySize(E, &syntax->r.p->v.a));
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_STRINGFORMAT:
            PD(); printf("%% (format)\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_NULL:
            PD(); printf("null\n");
            break;

        case EST_THIS:
            PD(); printf("this\n");
            break;

        case EST_NOT:
            PD(); printf("Not (logical)\n");
            REC_CHILD(syntax->v.p, depth + 1);
            break;

        case EST_BITWISE_NOT:
            PD(); printf("Not (bitwise)\n");
            REC_CHILD(syntax->v.p, depth + 1);
            break;

        case EST_BITWISE_XOR:
            if (syntax->v.i) {
                PD(); printf("^= (xor)\n");
            } else {
                PD(); printf("xor\n");
            }
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_BITWISE_AND:
            if (syntax->v.i) {
                PD(); printf("&= (bitwise and)\n");
            } else {
                PD(); printf("And (bitwise)\n");
            }
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_BITWISE_OR:
            if (syntax->v.i) {
                PD(); printf("|= (bitwise or)\n");
            } else {
                PD(); printf("Or (bitwise)\n");
            }
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_SHIFTLEFT:
            if (syntax->v.i) {
                PD(); printf("<<=\n");
            } else {
                PD(); printf("<<\n");
            }
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_SHIFTRIGHT:
            if (syntax->v.i) {
                PD(); printf(">>=\n");
            } else {
                PD(); printf(">>\n");
            }
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_AND:
            PD(); printf("And (logical)\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_OR:
            PD(); printf("Or (logical)\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_ADD:
            if (syntax->v.i) {
                PD(); printf("+=\n");
            } else {
                PD(); printf("+\n");
            }
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_SUB:
            if (syntax->v.i) {
                PD(); printf("-=\n");
            } else {
                PD(); printf("-\n");
            }
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_MUL:
            if (syntax->v.i) {
                PD(); printf("*=\n");
            } else {
                PD(); printf("*\n");
            }
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_DIV:
            if (syntax->v.i) {
                PD(); printf("/=\n");
            } else {
                PD(); printf("/\n");
            }
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_STATEMENT_EXPR:
            PD(); printf("StatementExpr\n");
            REC_CHILD(syntax->v.p, depth + 1);
            break;

        case EST_ASSIGNMENT:
            PD(); printf("=\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_CMP:
            PD(); printf("cmp\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_EQUALS:
            PD(); printf("==\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_NOTEQUALS:
            PD(); printf("!=\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_GREATERTHAN:
            PD(); printf(">\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_GREATERTHANOREQUAL:
            PD(); printf(">=\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_LESSTHAN:
            PD(); printf("<\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_LESSTHANOREQUAL:
            PD(); printf("<=\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_INHERITS:
            PD(); printf("Inherits\n");
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_BREAK:
            PD(); printf("Break\n");
            break;

        case EST_RETURN:
            PD(); printf("Return\n");
            if (syntax->v.p)
                REC_CHILD(syntax->v.p, depth + 1);
            break;

        case EST_IFELSE:
            PD(); printf("IfElse\n");
            REC_CHILD(syntax->v.p, depth + 1);
            REC_CHILD(syntax->l.p, depth + 1);
            if (syntax->r.p) {
                REC_CHILD(syntax->r.p, depth + 1);
            }
            break;

        case EST_WHILE:
            PD(); printf("while\n");
            REC_CHILD(syntax->v.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_FOR:
            PD(); printf("for\n");
            REC_CHILD(syntax->v.p, depth + 1);
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_FUNCTION:
            PD(); printf("Function: \"%s\"\n", syntax->v.s);
            REC_CHILD(syntax->l.p, depth + 1);
            REC_CHILD(syntax->r.p, depth + 1);
            break;

        case EST_FUNCTION_ARGS:
            PD(); printf("Function Args\n");
            REC_CHILD(syntax->l.p, depth + 1);
            break;

        case EST_SCOPE:
            PD(); printf("scope\n");
            REC_CHILD(syntax->v.p, depth + 1);
            break;
    }
}

void ekSyntaxAst(struct ekContext * E, struct ekSyntax * syntax)
{
    ekSyntaxAstRecurse(E, syntax, 0);
}
