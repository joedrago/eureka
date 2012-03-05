// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPSYNTAX_H
#define YAPSYNTAX_H

#include "yapArray.h"

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
    YST_VAR,                           // var s (= r)
    YST_BREAK,                         // break;
    YST_RETURN,                        // return a:r

    YST_IFELSE,                        // if v: l else r
    YST_WHILE,                         // while v: l
    YST_FOR,                           // for vars in iter \n    body
    YST_FUNCTION,                      // s(l) { r }
    YST_FUNCTION_ARGS,                 // l.p are args, v.s is the optional name of the varargs variable

    YST_SCOPE,                         // arbitrary scope block executing v.p statement list

    YST_COUNT
};

typedef struct yapSyntaxElement
{
    struct yapSyntax *p;               // ptr to syntax value
    char *s;                           // string value
    int   i;                           // int value
    yF32  f;                           // float value
    yapArray *a;                       // array value
} yapSyntaxElement;

typedef struct yapSyntax
{
    yU32 type;
    int line;
    yapSyntaxElement v;                // "value" - name / value / main expr / cond expr
    yapSyntaxElement l;                // "left"  - lvalue / ifBody / loop body
    yapSyntaxElement r;                // "right" - rvalue / simple lists
} yapSyntax;

// Create KTable Options
enum
{
    CKO_NONE = 0,
    CKO_NEGATIVE = (1 << 0)
};

// yapSyntax List Flags
enum
{
    YSLF_AUTOLITERAL = (1 << 0)        // Fat comma support, as in Perl. Morphs the bareword identifier prior to the fat comma into a literal string
};

yapSyntax *yapSyntaxCreate(struct yapContext *Y, yU32 type, int line);
yapSyntax *yapSyntaxCreateKString(struct yapContext *Y, struct yapToken *token);
yapSyntax *yapSyntaxCreateKInt(struct yapContext *Y, struct yapToken *token, yU32 opts);
yapSyntax *yapSyntaxCreateKFloat(struct yapContext *Y, struct yapToken *token, yU32 opts);
yapSyntax *yapSyntaxCreateIdentifier(struct yapContext *Y, struct yapToken *token);
yapSyntax *yapSyntaxCreateIndex(struct yapContext *Y, yapSyntax *array, yapSyntax *index, yBool pushThis);
yapSyntax *yapSyntaxCreateNull(struct yapContext *Y, int line);
yapSyntax *yapSyntaxCreateThis(struct yapContext *Y, int line);
yapSyntax *yapSyntaxCreateList(struct yapContext *Y, yU32 type, yapSyntax *firstExpr);
yapSyntax *yapSyntaxListAppend(struct yapContext *Y, yapSyntax *list, yapSyntax *expr, yU32 flags);
yapSyntax *yapSyntaxCreateCall(struct yapContext *Y, yapSyntax *func, yapSyntax *args);
yapSyntax *yapSyntaxCreateStringFormat(struct yapContext *Y, yapSyntax *format, yapSyntax *args);
yapSyntax *yapSyntaxCreateUnary(struct yapContext *Y, yU32 type, yapSyntax *expr);
yapSyntax *yapSyntaxCreateBinary(struct yapContext *Y, yU32 type, yapSyntax *l, yapSyntax *r, yBool compound);
yapSyntax *yapSyntaxCreateStatementExpr(struct yapContext *Y, yapSyntax *expr);
yapSyntax *yapSyntaxCreateAssignment(struct yapContext *Y, yapSyntax *l, yapSyntax *r);
yapSyntax *yapSyntaxCreateInherits(struct yapContext *Y, yapSyntax *l, yapSyntax *r);
yapSyntax *yapSyntaxCreateVar(struct yapContext *Y, yapSyntax *expr);
yapSyntax *yapSyntaxCreateBreak(struct yapContext *Y, int line);
yapSyntax *yapSyntaxCreateReturn(struct yapContext *Y, yapSyntax *expr);
yapSyntax *yapSyntaxCreateIfElse(struct yapContext *Y, yapSyntax *cond, yapSyntax *ifBody, yapSyntax *elseBody);
yapSyntax *yapSyntaxCreateWhile(struct yapContext *Y, yapSyntax *cond, yapSyntax *body);
yapSyntax *yapSyntaxCreateFor(struct yapContext *Y, yapSyntax *vars, yapSyntax *iter, yapSyntax *body);
yapSyntax *yapSyntaxCreateFunctionDecl(struct yapContext *Y, struct yapToken *name, yapSyntax *args, yapSyntax *body, int line);
yapSyntax *yapSyntaxCreateFunctionArgs(struct yapContext *Y, yapSyntax *args, struct yapToken *varargs);
yapSyntax *yapSyntaxCreateScope(struct yapContext *Y, yapSyntax *body);

void yapSyntaxDestroy(struct yapContext *Y, yapSyntax *syntax);

#endif
