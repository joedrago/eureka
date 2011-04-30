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

    YST_INDEX,                         // l[r]

    YST_STATEMENTLIST,                 // v.a: array of syntax
    YST_EXPRESSIONLIST,                // v.a: array of syntax
    YST_IDENTIFIERLIST,                // v.a: array of syntax

    YST_CALL,                          // s: funcname,  r: array of arg exprs
    YST_STRINGFORMAT,                  // l: format expr, r: array of arg exprs

    YST_NULL,
    YST_THIS,

    YST_TOSTRING,                      // r: expression to convert
    YST_TOINT,                         // r: expression to convert
    YST_NOT,                           // !v

    YST_ADD,                           // l+r
    YST_SUB,                           // l-r
    YST_MUL,                           // l*r
    YST_DIV,                           // l/r

    YST_AND,                           // l && r
    YST_OR,                            // l || r

    YST_STATEMENT_EXPR,                // r: expr
    YST_ASSIGNMENT,                    // s = r
    YST_VAR,                           // var s (= r)
    YST_RETURN,                        // return a:r

    YST_IFELSE,                        // if v: l else r
    YST_WHILE,                         // while v: l
    YST_FOR,                           // for vars in iter \n    body
    YST_FUNCTION,                      // s(l) { r }

    YST_CLASS,                         // class v.s \n body r

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
yapSyntax * yapSyntaxCreateIndex(yapSyntax *array, yapSyntax *index);
yapSyntax * yapSyntaxCreateNull();
yapSyntax * yapSyntaxCreateThis();
yapSyntax * yapSyntaxCreateList(yU32 type, yapSyntax *firstExpr);
yapSyntax * yapSyntaxListAppend(yapSyntax *list, yapSyntax *expr);
yapSyntax * yapSyntaxCreateIndexedCall(yapSyntax *obj, yapSyntax *func, yapSyntax *args);
yapSyntax * yapSyntaxCreateCall(yapSyntax *func, yapSyntax *args);
yapSyntax * yapSyntaxCreateStringFormat(yapSyntax *format, yapSyntax *args);
yapSyntax * yapSyntaxCreateUnary(yU32 type, yapSyntax *expr);
yapSyntax * yapSyntaxCreateBinary(yU32 type, yapSyntax *l, yapSyntax *r);
yapSyntax * yapSyntaxCreateStatementExpr(yapSyntax *expr);
yapSyntax * yapSyntaxCreateAssignment(yapSyntax *l, yapSyntax *r);
yapSyntax * yapSyntaxCreateVar(yapSyntax *expr);
yapSyntax * yapSyntaxCreateReturn(yapSyntax *expr);
yapSyntax * yapSyntaxCreateIfElse(yapSyntax *cond, yapSyntax *ifBody, yapSyntax *elseBody);
yapSyntax * yapSyntaxCreateWhile(yapSyntax *cond, yapSyntax *body);
yapSyntax * yapSyntaxCreateFor(yapSyntax *vars, yapSyntax *iter, yapSyntax *body);
yapSyntax * yapSyntaxCreateFunctionDecl(struct yapToken *name, yapSyntax *args, yapSyntax *body);
yapSyntax * yapSyntaxCreateClass(yapSyntax *name, yapSyntax *isa, yapSyntax *body);

void yapSyntaxDestroy(yapSyntax *syntax);

#endif
