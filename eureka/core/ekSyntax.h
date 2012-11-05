// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EUREKASYNTAX_H
#define EUREKASYNTAX_H

#include "ekTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekToken;

// ---------------------------------------------------------------------------

// Note: This enum must stay sync'd with ekCompiler's ekAssembleInfo
enum
{
    YST_NOP = 0,

    YST_KSTRING,                       // s
    YST_KINT,                          // i
    YST_KFLOAT,                        // f
    YST_IDENTIFIER,                    // s

    YST_INDEX,                         // l[r]  (v.i is whether to push 'this' instead of the object, if a call is requested)

    YST_STATEMENTLIST,                 // v.a: array of syntax
    YST_EXPRESSIONLIST,                // v.a: array of syntax
    YST_IDENTIFIERLIST,                // v.a: array of syntax

    YST_CALL,                          // s: funcname,  r: array of arg exprs
    YST_STRINGFORMAT,                  // l: format expr, r: array of arg exprs

    YST_NULL,
    YST_THIS,

    YST_NOT,                           // !v

    YST_BITWISE_NOT,                   // ~v
    YST_BITWISE_XOR,                   // l^r
    YST_BITWISE_AND,                   // l&r
    YST_BITWISE_OR,                    // l|r
    YST_SHIFTLEFT,                     // l<<r
    YST_SHIFTRIGHT,                    // l>>r

    YST_ADD,                           // l+r
    YST_SUB,                           // l-r
    YST_MUL,                           // l*r
    YST_DIV,                           // l/r

    YST_AND,                           // l && r
    YST_OR,                            // l || r

    YST_CMP,                           // l <=> r  (compare)
    YST_EQUALS,                        // l == r
    YST_NOTEQUALS,                     // l != r
    YST_GREATERTHAN,                   // l > r
    YST_GREATERTHANOREQUAL,            // l >= r
    YST_LESSTHAN,                      // l < r
    YST_LESSTHANOREQUAL,               // l <= r

    YST_STATEMENT_EXPR,                // r: expr
    YST_ASSIGNMENT,                    // l = r
    YST_INHERITS,                      // ensures l is an object, and then makes r inherit from l
    YST_BREAK,                         // break;
    YST_RETURN,                        // return a:r

    YST_IFELSE,                        // if v: l else r (if v.i, it is a ternary operator)
    YST_WHILE,                         // while v: l
    YST_FOR,                           // for vars in iter \n    body
    YST_FUNCTION,                      // s(l) { r }
    YST_FUNCTION_ARGS,                 // l.p are args, v.s is the optional name of the varargs variable

    YST_SCOPE,                         // arbitrary scope block executing v.p statement list

    YST_COUNT
};

typedef struct ekSyntaxElement
{
    struct ekSyntax *p;               // ptr to syntax value
    char *s;                           // string value
    int   i;                           // int value
    ekF32  f;                           // float value
    struct ekSyntax **a;              // array value
} ekSyntaxElement;

typedef struct ekSyntax
{
    ekU32 type;
    int line;
    ekSyntaxElement v;                // "value" - name / value / main expr / cond expr
    ekSyntaxElement l;                // "left"  - lvalue / ifBody / loop body
    ekSyntaxElement r;                // "right" - rvalue / simple lists
} ekSyntax;

// Create KTable Options
enum
{
    CKO_NONE = 0,
    CKO_NEGATIVE = (1 << 0)
};

// ekSyntax List Flags
enum
{
    YSLF_AUTOLITERAL = (1 << 0)        // Fat comma support, as in Perl. Morphs the bareword identifier prior to the fat comma into a literal string
};

ekSyntax *ekSyntaxCreate(struct ekContext *Y, ekU32 type, int line);
ekSyntax *ekSyntaxCreateKString(struct ekContext *Y, struct ekToken *token);
ekSyntax *ekSyntaxCreateKInt(struct ekContext *Y, struct ekToken *token, ekU32 opts);
ekSyntax *ekSyntaxCreateKFloat(struct ekContext *Y, struct ekToken *token, ekU32 opts);
ekSyntax *ekSyntaxCreateIdentifier(struct ekContext *Y, struct ekToken *token);
ekSyntax *ekSyntaxCreateIndex(struct ekContext *Y, ekSyntax *array, ekSyntax *index, ekBool pushThis);
ekSyntax *ekSyntaxCreateNull(struct ekContext *Y, int line);
ekSyntax *ekSyntaxCreateThis(struct ekContext *Y, int line);
ekSyntax *ekSyntaxCreateList(struct ekContext *Y, ekU32 type, ekSyntax *firstExpr);
ekSyntax *ekSyntaxListAppend(struct ekContext *Y, ekSyntax *list, ekSyntax *expr, ekU32 flags);
ekSyntax *ekSyntaxCreateCall(struct ekContext *Y, ekSyntax *func, ekSyntax *args);
ekSyntax *ekSyntaxCreateStringFormat(struct ekContext *Y, ekSyntax *format, ekSyntax *args);
ekSyntax *ekSyntaxCreateUnary(struct ekContext *Y, ekU32 type, ekSyntax *expr);
ekSyntax *ekSyntaxCreateBinary(struct ekContext *Y, ekU32 type, ekSyntax *l, ekSyntax *r, ekBool compound);
ekSyntax *ekSyntaxCreateStatementExpr(struct ekContext *Y, ekSyntax *expr);
ekSyntax *ekSyntaxCreateAssignment(struct ekContext *Y, ekSyntax *l, ekSyntax *r);
ekSyntax *ekSyntaxCreateInherits(struct ekContext *Y, ekSyntax *l, ekSyntax *r);
ekSyntax *ekSyntaxCreateBreak(struct ekContext *Y, int line);
ekSyntax *ekSyntaxCreateReturn(struct ekContext *Y, ekSyntax *expr);
ekSyntax *ekSyntaxCreateIfElse(struct ekContext *Y, ekSyntax *cond, ekSyntax *ifBody, ekSyntax *elseBody, ekBool ternary);
ekSyntax *ekSyntaxCreateWhile(struct ekContext *Y, ekSyntax *cond, ekSyntax *body);
ekSyntax *ekSyntaxCreateFor(struct ekContext *Y, ekSyntax *vars, ekSyntax *iter, ekSyntax *body);
ekSyntax *ekSyntaxCreateFunctionDecl(struct ekContext *Y, struct ekToken *name, ekSyntax *args, ekSyntax *body, int line);
ekSyntax *ekSyntaxCreateFunctionArgs(struct ekContext *Y, ekSyntax *args, struct ekToken *varargs);
ekSyntax *ekSyntaxCreateScope(struct ekContext *Y, ekSyntax *body);
ekSyntax *ekSyntaxMarkVar(struct ekContext *Y, ekSyntax *identList);

void ekSyntaxDestroy(struct ekContext *Y, ekSyntax *syntax);

#endif
