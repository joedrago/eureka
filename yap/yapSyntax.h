#ifndef YAPSYNTAX_H
#define YAPSYNTAX_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapToken;

// ---------------------------------------------------------------------------

// Note: This enum must stay sync'd with yapCompiler's yapAssembleInfo
enum
{
    YST_NOP = 0,

    YST_KSTRING,                       // s
    YST_KINT,                          // i
    YST_IDENTIFIER,                    // s

    YST_STATEMENTLIST,                 // r: array of syntax
    YST_EXPRESSIONLIST,                // r: array of syntax
    YST_IDENTIFIERLIST,                // r: array of syntax

    YST_CALL,                          // s: funcname,  r: array of arg exprs
    YST_STRINGFORMAT,                  // l: format expr, r: array of arg exprs

    YST_NULL,

    YST_TOSTRING,                      // r: expression to convert
    YST_TOINT,                         // r: expression to convert
    YST_ADD,                           // l+r
    YST_SUB,                           // l-r
    YST_MUL,                           // l*r
    YST_DIV,                           // l/r

    YST_STATEMENT_EXPR,                // r: expr
    YST_ASSIGNMENT,                    // s = r
    YST_VAR,                           // var s (= r)
    YST_RETURN,                        // return a:r

    YST_IFELSE,                        // if v: l else r
    YST_LOOP,                          // while v: l
    YST_FUNCTION,                      // s(l) { r }

    YST_COUNT
};

typedef struct yapSyntaxElement
{
    struct yapSyntax *p;               // ptr to syntax value
    char *s;                           // string value
    int   i;                           // int value
    yapArray *a;                       // array value
} yapSyntaxElement;

typedef struct yapSyntax
{
    yU32 type;
    yapSyntaxElement v;                // "value" - name / value / main expr / cond expr
    yapSyntaxElement l;                // "left"  - lvalue / ifBody / loop body
    yapSyntaxElement r;                // "right" - rvalue / simple lists
} yapSyntax;

yapSyntax * yapSyntaxCreate(yU32 type);
yapSyntax * yapSyntaxCreateKString(struct yapToken *token);
yapSyntax * yapSyntaxCreateKInt(struct yapToken *token);
yapSyntax * yapSyntaxCreateIdentifier(struct yapToken *token);
yapSyntax * yapSyntaxCreateNull();
yapSyntax * yapSyntaxCreateList(yU32 type, yapSyntax *firstExpr);
yapSyntax * yapSyntaxListAppend(yapSyntax *list, yapSyntax *expr);
yapSyntax * yapSyntaxCreateCall(struct yapToken *name, yapSyntax *args);
yapSyntax * yapSyntaxCreateStringFormat(yapSyntax *format, yapSyntax *args);
yapSyntax * yapSyntaxCreateUnary(yU32 type, yapSyntax *expr);
yapSyntax * yapSyntaxCreateBinary(yU32 type, yapSyntax *l, yapSyntax *r);
yapSyntax * yapSyntaxCreateStatementExpr(yapSyntax *expr);
yapSyntax * yapSyntaxCreateAssignment(struct yapToken *token, yapSyntax *expr);
yapSyntax * yapSyntaxCreateVar(struct yapToken *token, yapSyntax *expr);
yapSyntax * yapSyntaxCreateReturn(yapSyntax *expr);
yapSyntax * yapSyntaxCreateIfElse(yapSyntax *cond, yapSyntax *ifBody, yapSyntax *elseBody);
yapSyntax * yapSyntaxCreateLoop(yapSyntax *cond, yapSyntax *body);
yapSyntax * yapSyntaxCreateFunctionDecl(struct yapToken *name, yapSyntax *args, yapSyntax *body);

void yapSyntaxDestroy(yapSyntax *syntax);

#endif
