// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

%name ekParse

%token_prefix ETT_

%token_type
    { ekToken }

%default_type
    { ekToken }

%extra_argument
    { ekCompiler *C }

%include
{
    #include "ekCompiler.h"
    #include "ekLexer.h"
    #include "ekParser.h"
    #include "ekSyntax.h"
    #include "ekContext.h"

    #include <string.h>
    #include <stdlib.h>

    #undef assert
    #define assert(ignoring_this_function)
    
#ifdef EUREKA_TRACE_PARSER
    #undef NDEBUG
#endif
}

%fallback GROUPLEFTPAREN LEFTPAREN.
%fallback SCOPESTARTBLOCK STARTBLOCK.
%fallback MAPSTARTBLOCK SCOPESTARTBLOCK.
%fallback NEGATIVE DASH.
%fallback ARRAYOPENBRACKET OPENBRACKET.

// Operators, ordered from lowest precedence to highest

// Stuff that does not come into play with precedence is "lowest". They are only listed to establish left/nonassoc/right.
%left NEWLINE.
%left SEMI.
%left ASSIGN.
%left QUESTIONMARK.
%left COLON.
%left IF.
%left ELSE.
%left HEREDOC.
%nonassoc ELLIPSIS.
%nonassoc BREAK.
%nonassoc THIS.
%left UNKNOWN.
%left COMMENT.
%left SPACE.
%left EOF.

// Compound operators
%left BITWISE_OREQUALS.
%left BITWISE_XOREQUALS.
%left BITWISE_ANDEQUALS.
%left SHIFTRIGHTEQUALS.
%left SHIFTLEFTEQUALS.
%left PLUSEQUALS.
%left SLASHEQUALS.
%left DASHEQUALS.
%left STAREQUALS.

// Comparisons: ORs are lowest, then ANDs, then inequality and inherits
%left OR.
%left BITWISE_OR.
%left BITWISE_XOR.
%left AND.
%left BITWISE_AND.
%right BITWISE_NOT.
%right NOT.
%left EQUALS.
%left NOTEQUALS.
%left CMP.
%left LESSTHAN.
%left LESSTHANOREQUAL.
%left GREATERTHAN.
%left GREATERTHANOREQUAL.
%left INHERITS.

// Bit shifting
%left SHIFTRIGHT.
%left SHIFTLEFT.

// Arithmetic
%left PLUS.
%left DASH.
%left STAR.
%left SLASH.
%left MOD.

// Indexing
%left PERIOD.
%left COLONCOLON.

// Function calls and whatnot
%left LEFTPAREN.
%left OPENBRACE.
%left CLOSEBRACE.
%left OPENBRACKET.
%left CLOSEBRACKET.

// End of operators (highest precedence here)

%syntax_error { ekCompileSyntaxError(C, &TOKEN); }

// ---------------------------------------------------------------------------
// Chunk

chunk ::= statement_list(L).
    { C->root = L; }

// ---------------------------------------------------------------------------
// Statement List

%type statement_list
    { ekSyntax* }

%destructor statement_list
    { ekSyntaxDestroy(C->E, $$); }

statement_list(L) ::= statement_list(OL) statement(S).
    { L = ekSyntaxListAppend(C->E, OL, S, 0); }

statement_list(L) ::= statement(S).
    { L = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, S); }

// ---------------------------------------------------------------------------
// Statement Block

%type statement_block
    { ekSyntax* }

%destructor statement_block
    { ekSyntaxDestroy(C->E, $$); }

statement_block(B) ::= STARTBLOCK ENDBLOCK.
    { B = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, NULL); }

statement_block(B) ::= STARTBLOCK statement_list(L) ENDBLOCK.
    { B = L; }

statement_block(B) ::= statement(S).
    { B = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, S); }

// ---------------------------------------------------------------------------
// Statement

%type statement
    { ekSyntax* }

%destructor statement
    { ekSyntaxDestroy(C->E, $$); }

statement(S) ::= BREAK(B) ENDSTATEMENT.
    { S = ekSyntaxCreateBreak(C->E, B.line); }

statement(S) ::= RETURN expr_list(L) ENDSTATEMENT.
    { S = ekSyntaxCreateReturn(C->E, L); }

statement(S) ::= RETURN paren_expr_list(L) ENDSTATEMENT.
    { S = ekSyntaxCreateReturn(C->E, L); }

statement(S) ::= expr_list(L) ENDSTATEMENT.
    { S = ekSyntaxCreateStatementExpr(C->E, L); }

statement(S) ::= IF expr_list(COND) statement_block(IFBODY) ELSE statement_block(ELSEBODY).
    { S = ekSyntaxCreateIfElse(C->E, COND, IFBODY, ELSEBODY, ekFalse); }

statement(S) ::= IF expr_list(COND) statement_block(IFBODY).
    { S = ekSyntaxCreateIfElse(C->E, COND, IFBODY, NULL, ekFalse); }

statement(S) ::= WHILE expr_list(COND) statement_block(BODY).
    { S = ekSyntaxCreateWhile(C->E, COND, BODY); }

statement(S) ::= FUNCTION(F) IDENTIFIER(I) LEFTPAREN func_args(ARGS) RIGHTPAREN statement_block(BODY).
    { S = ekSyntaxCreateFunctionDecl(C->E, &I, ARGS, BODY, F.line); }

statement(S) ::= FOR LEFTPAREN ident_list(VARS) IN expression(ITER) RIGHTPAREN statement_block(BODY).
    { S = ekSyntaxCreateFor(C->E, VARS, ITER, BODY); }

statement(S) ::= lvalue(L) PLUSEQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->E, EST_ADD, L, R, ekTrue); }

statement(S) ::= lvalue(L) DASHEQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->E, EST_SUB, L, R, ekTrue); }

statement(S) ::= lvalue(L) STAREQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->E, EST_MUL, L, R, ekTrue); }

statement(S) ::= lvalue(L) SLASHEQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->E, EST_DIV, L, R, ekTrue); }

statement(S) ::= lvalue(L) BITWISE_OREQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, L, R, ekTrue); }

statement(S) ::= lvalue(L) BITWISE_ANDEQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, L, R, ekTrue); }

statement(S) ::= lvalue(L) BITWISE_XOREQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, L, R, ekTrue); }

statement(S) ::= lvalue(L) SHIFTLEFTEQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, L, R, ekTrue); }

statement(S) ::= lvalue(L) SHIFTRIGHTEQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, L, R, ekTrue); }

statement(S) ::= ENDSTATEMENT.
    { S = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, NULL); }

statement(S) ::= SCOPESTARTBLOCK ENDBLOCK.
    { S = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, NULL); }

statement(S) ::= SCOPESTARTBLOCK statement_list(L) ENDBLOCK.
    { S = ekSyntaxCreateScope(C->E, L); }

statement ::= error ENDSTATEMENT.

// ---------------------------------------------------------------------------
// Parenthesized Expression List

%type paren_expr_list
    { ekSyntax* }

%destructor paren_expr_list
    { ekSyntaxDestroy(C->E, $$); }

paren_expr_list(PEL) ::= LEFTPAREN expr_list(L) RIGHTPAREN.
    { PEL = L; }

paren_expr_list(PEL) ::= LEFTPAREN RIGHTPAREN.
    { PEL = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, NULL); }

// ---------------------------------------------------------------------------
// Expression List

%type expr_list
    { ekSyntax* }

%destructor expr_list
    { ekSyntaxDestroy(C->E, $$); }

expr_list(EL) ::= expr_list(OL) COMMA expression(EXPR).
    { EL = ekSyntaxListAppend(C->E, OL, EXPR, 0); }

expr_list(EL) ::= expr_list(OL) FATCOMMA expression(EXPR).
    { EL = ekSyntaxListAppend(C->E, OL, EXPR, ESLF_AUTOLITERAL); }

expr_list(EL) ::= expression(EXPR).
    { EL = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, EXPR); }

// ---------------------------------------------------------------------------
// Expression

%type expression
    { ekSyntax* }

%destructor expression
    { ekSyntaxDestroy(C->E, $$); }

expression(EXP) ::= NOT expression(EXPR).
    { EXP = ekSyntaxCreateUnary(C->E, EST_NOT, EXPR, EXPR->line); }

expression(EXP) ::= BITWISE_NOT expression(EXPR).
    { EXP = ekSyntaxCreateUnary(C->E, EST_BITWISE_NOT, EXPR, EXPR->line); }

expression(EXP) ::= expression(L) PLUS expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_ADD, L, R, ekFalse); }

expression(EXP) ::= expression(L) DASH expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_SUB, L, R, ekFalse); }

expression(EXP) ::= expression(L) STAR expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_MUL, L, R, ekFalse); }

expression(EXP) ::= expression(L) SLASH expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_DIV, L, R, ekFalse); }

expression(EXP) ::= expression(L) AND expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_AND, L, R, ekFalse); }

expression(EXP) ::= expression(L) OR expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_OR, L, R, ekFalse); }

expression(EXP) ::= expression(L) CMP expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_CMP, L, R, ekFalse); }

expression(EXP) ::= expression(L) EQUALS expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_EQUALS, L, R, ekFalse); }

expression(EXP) ::= expression(L) NOTEQUALS expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_NOTEQUALS, L, R, ekFalse); }

expression(EXP) ::= expression(L) GREATERTHAN expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_GREATERTHAN, L, R, ekFalse); }

expression(EXP) ::= expression(L) GREATERTHANOREQUAL expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_GREATERTHANOREQUAL, L, R, ekFalse); }

expression(EXP) ::= expression(L) LESSTHAN expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_LESSTHAN, L, R, ekFalse); }

expression(EXP) ::= expression(L) LESSTHANOREQUAL expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_LESSTHANOREQUAL, L, R, ekFalse); }

expression(EXP) ::= expression(L) BITWISE_XOR expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, L, R, ekFalse); }

expression(EXP) ::= expression(L) BITWISE_AND expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, L, R, ekFalse); }

expression(EXP) ::= expression(L) BITWISE_OR expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, L, R, ekFalse); }

expression(EXP) ::= expression(L) SHIFTLEFT expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, L, R, ekFalse); }

expression(EXP) ::= expression(L) SHIFTRIGHT expression(R).
    { EXP = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, L, R, ekFalse); }

expression(EXP) ::= expression(FORMAT) MOD paren_expr_list(ARGS).
    { EXP = ekSyntaxCreateStringFormat(C->E, FORMAT, ARGS); }

expression(EXP) ::= expression(FORMAT) MOD expression(ARGS).
    { EXP = ekSyntaxCreateStringFormat(C->E, FORMAT, ARGS); }

expression(EXPR) ::= ARRAYOPENBRACKET(AOB) CLOSEBRACKET.
    { EXPR = ekSyntaxCreateUnary(C->E, EST_ARRAY, NULL, AOB.line); }

expression(EXPR) ::= ARRAYOPENBRACKET expr_list(EL) CLOSEBRACKET.
    { EXPR = ekSyntaxCreateUnary(C->E, EST_ARRAY, EL, EL->line); }

expression(EXPR) ::= MAPSTARTBLOCK(MSB) ENDBLOCK.
    { EXPR = ekSyntaxCreateUnary(C->E, EST_MAP, NULL, MSB.line); }

expression(EXPR) ::= MAPSTARTBLOCK expr_list(EL) ENDBLOCK.
    { EXPR = ekSyntaxCreateUnary(C->E, EST_MAP, EL, EL->line); }

expression(EXPR) ::= lvalue(L) ASSIGN expression(R).
    { EXPR = ekSyntaxCreateAssignment(C->E, L, R); }

expression(EXPR) ::= expression(L) INHERITS expression(R).
    { EXPR = ekSyntaxCreateInherits(C->E, L, R); }

expression(EXPR) ::= lvalue(LV).
    { EXPR = LV; }

expression(EXPR) ::= INTEGER(I).
    { EXPR = ekSyntaxCreateKInt(C->E, &I, 0); }

expression(EXPR) ::= NEGATIVE INTEGER(I).
    { EXPR = ekSyntaxCreateKInt(C->E, &I, CKO_NEGATIVE); }

expression(EXPR) ::= FLOATNUM(F).
    { EXPR = ekSyntaxCreateKFloat(C->E, &F, 0); }

expression(EXPR) ::= NEGATIVE FLOATNUM(F).
    { EXPR = ekSyntaxCreateKFloat(C->E, &F, CKO_NEGATIVE); }

expression(EXPR) ::= LITERALSTRING(L).
    { EXPR = ekSyntaxCreateKString(C->E, &L, 0); }

expression(EXPR) ::= REGEXSTRING(R).
    { EXPR = ekSyntaxCreateKString(C->E, &R, 1); }

expression(EXPR) ::= TRUE(T).
    { EXPR = ekSyntaxCreateBool(C->E, &T, ekTrue); }

expression(EXPR) ::= FALSE(F).
    { EXPR = ekSyntaxCreateBool(C->E, &F, ekFalse); }

expression(EXPR) ::= NULL(N).
    { EXPR = ekSyntaxCreateNull(C->E, N.line); }

expression(EXPR) ::= FUNCTION(F) LEFTPAREN func_args(ARGS) RIGHTPAREN statement_block(BODY).
    { EXPR = ekSyntaxCreateFunctionDecl(C->E, NULL, ARGS, BODY, F.line); }

expression(EXPR) ::= expression(COND) QUESTIONMARK expression(IFBODY) COLON expression(ELSEBODY).
    { EXPR = ekSyntaxCreateIfElse(C->E, COND, IFBODY, ELSEBODY, ekTrue); }


// ---------------------------------------------------------------------------
// LValue

%type lvalue
    { ekSyntax* }

%destructor lvalue
    { ekSyntaxDestroy(C->E, $$); }

lvalue(L) ::= lvalue_indexable(I).
    { L = I; }

lvalue(L) ::= VAR IDENTIFIER(I).
    { L = ekSyntaxMarkVar(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &I))); }

lvalue(L) ::= VAR GROUPLEFTPAREN ident_list(I) RIGHTPAREN.
    { L = ekSyntaxMarkVar(C->E, I); }

lvalue(L) ::= GROUPLEFTPAREN expr_list(EL) RIGHTPAREN.
    { L = EL; }

// ---------------------------------------------------------------------------
// LValueIndexable

%type lvalue_indexable
    { ekSyntax* }

%destructor lvalue_indexable
    { ekSyntaxDestroy(C->E, $$); }

lvalue_indexable(L) ::= THIS(T).
    { L = ekSyntaxCreateThis(C->E, T.line); }

lvalue_indexable(L) ::= lvalue_indexable(FUNC) paren_expr_list(ARGS).
    { L = ekSyntaxCreateCall(C->E, FUNC, ARGS); }

lvalue_indexable(L) ::= lvalue_indexable(ARRAY) OPENBRACKET expression(INDEX) CLOSEBRACKET.
    { L = ekSyntaxCreateIndex(C->E, ARRAY, INDEX, ekFalse); }

lvalue_indexable(L) ::= lvalue_indexable(OBJECT) PERIOD IDENTIFIER(MEMBER).
    { L = ekSyntaxCreateIndex(C->E, OBJECT, ekSyntaxCreateKString(C->E, &MEMBER, 0), ekFalse); }

lvalue_indexable(L) ::= lvalue_indexable(OBJECT) COLONCOLON IDENTIFIER(MEMBER).
    { L = ekSyntaxCreateIndex(C->E, OBJECT, ekSyntaxCreateKString(C->E, &MEMBER, 0), ekTrue); }

lvalue_indexable(L) ::= IDENTIFIER(I).
    { L = ekSyntaxCreateIdentifier(C->E, &I); }

// ---------------------------------------------------------------------------
// Identifier List

%type ident_list
    { ekSyntax* }

%destructor ident_list
    { ekSyntaxDestroy(C->E, $$); }

ident_list(IL) ::= ident_list(OL) COMMA IDENTIFIER(I).
    { IL = ekSyntaxListAppend(C->E, OL, ekSyntaxCreateIdentifier(C->E, &I), 0); }

ident_list(IL) ::= IDENTIFIER(I).
    { IL = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &I)); }

ident_list(IL) ::= .
    { IL = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL); }

// ---------------------------------------------------------------------------
// Function Arguments

%type func_args
    { ekSyntax* }

func_args(ARGS) ::= ident_list(IL).
    { ARGS = ekSyntaxCreateFunctionArgs(C->E, IL, NULL); }

func_args(ARGS) ::= ident_list(IL) COMMA ELLIPSIS IDENTIFIER(VARARGS).
    { ARGS = ekSyntaxCreateFunctionArgs(C->E, IL, &VARARGS); }

func_args(ARGS) ::= ELLIPSIS IDENTIFIER(VARARGS).
    { ARGS = ekSyntaxCreateFunctionArgs(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL), &VARARGS); }
