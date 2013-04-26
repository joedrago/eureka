// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKSYNTAX_H
#define EKSYNTAX_H

#include "ekTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekToken;

// ---------------------------------------------------------------------------

// Note: This enum must stay sync'd with ekCompiler's ekAssembleInfo
enum
{
    EST_NOP = 0,

    EST_KSTRING,                       // s
    EST_KINT,                          // i
    EST_KFLOAT,                        // f
    EST_IDENTIFIER,                    // s

    EST_INDEX,                         // l[r]  (v.i is whether to push 'this' instead of the object, if a call is requested)

    EST_STATEMENTLIST,                 // v.a: array of syntax
    EST_EXPRESSIONLIST,                // v.a: array of syntax
    EST_IDENTIFIERLIST,                // v.a: array of syntax

    EST_ARRAY,                         // v.p: contents of a new array
    EST_MAP,                           // v.p: contents of a new map

    EST_CALL,                          // s: funcname,  r: array of arg exprs
    EST_STRINGFORMAT,                  // l: format expr, r: array of arg exprs

    EST_NULL,
    EST_THIS,

    EST_NOT,                           // !v

    EST_BITWISE_NOT,                   // ~v
    EST_BITWISE_XOR,                   // l^r
    EST_BITWISE_AND,                   // l&r
    EST_BITWISE_OR,                    // l|r
    EST_SHIFTLEFT,                     // l<<r
    EST_SHIFTRIGHT,                    // l>>r

    EST_ADD,                           // l+r
    EST_SUB,                           // l-r
    EST_MUL,                           // l*r
    EST_DIV,                           // l/r

    EST_AND,                           // l && r
    EST_OR,                            // l || r

    EST_CMP,                           // l <=> r  (compare)
    EST_EQUALS,                        // l == r
    EST_NOTEQUALS,                     // l != r
    EST_GREATERTHAN,                   // l > r
    EST_GREATERTHANOREQUAL,            // l >= r
    EST_LESSTHAN,                      // l < r
    EST_LESSTHANOREQUAL,               // l <= r

    EST_STATEMENT_EXPR,                // r: expr
    EST_ASSIGNMENT,                    // l = r
    EST_INHERITS,                      // ensures l is an object, and then makes r inherit from l
    EST_BREAK,                         // break;
    EST_RETURN,                        // return a:r

    EST_IFELSE,                        // if v: l else r (if v.i, it is a ternary operator)
    EST_WHILE,                         // while v: l
    EST_FOR,                           // for vars in iter \n    body
    EST_FUNCTION,                      // s(l) { r }
    EST_FUNCTION_ARGS,                 // l.p are args, v.s is the optional name of the varargs variable

    EST_SCOPE,                         // arbitrary scope block executing v.p statement list

    EST_COUNT
};

typedef struct ekSyntaxElement
{
    struct ekSyntax *p;               // ptr to syntax value
    char *s;                           // string value
    ekS32   i;                           // ekS32 value
    ekF32  f;                           // float value
    struct ekSyntax **a;              // array value
} ekSyntaxElement;

typedef struct ekSyntax
{
    ekU32 type;
    ekS32 line;
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
    ESLF_AUTOLITERAL = (1 << 0)        // Fat comma support, as in Perl. Morphs the bareword identifier prior to the fat comma into a literal string
};

ekSyntax *ekSyntaxCreate(struct ekContext *E, ekU32 type, ekS32 line);
ekSyntax *ekSyntaxCreateKString(struct ekContext *E, struct ekToken *token, int isRegex);
ekSyntax *ekSyntaxCreateKInt(struct ekContext *E, struct ekToken *token, ekU32 opts);
ekSyntax *ekSyntaxCreateKFloat(struct ekContext *E, struct ekToken *token, ekU32 opts);
ekSyntax *ekSyntaxCreateIdentifier(struct ekContext *E, struct ekToken *token);
ekSyntax *ekSyntaxCreateIndex(struct ekContext *E, ekSyntax *array, ekSyntax *index, ekBool pushThis);
ekSyntax *ekSyntaxCreateNull(struct ekContext *E, ekS32 line);
ekSyntax *ekSyntaxCreateThis(struct ekContext *E, ekS32 line);
ekSyntax *ekSyntaxCreateList(struct ekContext *E, ekU32 type, ekSyntax *firstExpr);
ekSyntax *ekSyntaxListAppend(struct ekContext *E, ekSyntax *list, ekSyntax *expr, ekU32 flags);
ekSyntax *ekSyntaxCreateCall(struct ekContext *E, ekSyntax *func, ekSyntax *args);
ekSyntax *ekSyntaxCreateStringFormat(struct ekContext *E, ekSyntax *format, ekSyntax *args);
ekSyntax *ekSyntaxCreateUnary(struct ekContext *E, ekU32 type, ekSyntax *expr, ekS32 line);
ekSyntax *ekSyntaxCreateBinary(struct ekContext *E, ekU32 type, ekSyntax *l, ekSyntax *r, ekBool compound);
ekSyntax *ekSyntaxCreateStatementExpr(struct ekContext *E, ekSyntax *expr);
ekSyntax *ekSyntaxCreateAssignment(struct ekContext *E, ekSyntax *l, ekSyntax *r);
ekSyntax *ekSyntaxCreateInherits(struct ekContext *E, ekSyntax *l, ekSyntax *r);
ekSyntax *ekSyntaxCreateBreak(struct ekContext *E, ekS32 line);
ekSyntax *ekSyntaxCreateReturn(struct ekContext *E, ekSyntax *expr);
ekSyntax *ekSyntaxCreateIfElse(struct ekContext *E, ekSyntax *cond, ekSyntax *ifBody, ekSyntax *elseBody, ekBool ternary);
ekSyntax *ekSyntaxCreateWhile(struct ekContext *E, ekSyntax *cond, ekSyntax *body);
ekSyntax *ekSyntaxCreateFor(struct ekContext *E, ekSyntax *vars, ekSyntax *iter, ekSyntax *body);
ekSyntax *ekSyntaxCreateFunctionDecl(struct ekContext *E, struct ekToken *name, ekSyntax *args, ekSyntax *body, ekS32 line);
ekSyntax *ekSyntaxCreateFunctionArgs(struct ekContext *E, ekSyntax *args, struct ekToken *varargs);
ekSyntax *ekSyntaxCreateScope(struct ekContext *E, ekSyntax *body);
ekSyntax *ekSyntaxMarkVar(struct ekContext *E, ekSyntax *identList);

void ekSyntaxDestroy(struct ekContext *E, ekSyntax *syntax);

#endif
