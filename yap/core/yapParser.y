// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

%name yapParse

%token_prefix YTT_

%token_type
    { yapToken }

%default_type
    { yapToken }

%extra_argument
    { yapCompiler *C }

%include
{
    #include "yapCompiler.h"
    #include "yapLexer.h"
    #include "yapParser.h"
    #include "yapSyntax.h"
    #include "yapContext.h"

    #include <string.h>
    #include <stdlib.h>

    #undef assert
    #define assert(ignoring_this_function)
    
#ifdef YAP_TRACE_PARSE
    #undef NDEBUG
#endif
}

%fallback GROUPLEFTPAREN LEFTPAREN.
%fallback SCOPESTARTBLOCK STARTBLOCK.
%fallback NEGATIVE DASH.

%left HEREDOC.
%left ELLIPSIS.
%left BREAK.
%left BITWISE_OREQUALS.
%left BITWISE_ANDEQUALS.
%left BITWISE_XOREQUALS.
%left PLUSEQUALS.
%left DASHEQUALS.
%left STAREQUALS.
%left SLASHEQUALS.
%left SHIFTRIGHTEQUALS.
%left SHIFTLEFTEQUALS.

%left NEWLINE.
%left OPENBRACE.
%left CLOSEBRACE.
%left OPENBRACKET.
%left CLOSEBRACKET.
%left SEMI.
%left ASSIGN.
%left CMP.
%left EQUALS.
%left NOTEQUALS.
%left LESSTHAN.
%left LESSTHANOREQUAL.
%left GREATERTHAN.
%left GREATERTHANOREQUAL.
%left INHERITS.
%left PERIOD.
%left LEFTPAREN.
%left COLONCOLON.

%left UNKNOWN.
%left COMMENT.
%left SPACE.
%left EOF.

%left BITWISE_OR.
%left BITWISE_AND.
%left BITWISE_XOR.
%left BITWISE_NOT.
%left SHIFTRIGHT.
%left SHIFTLEFT.

%left PLUS.
%left DASH.
%left STAR.
%left SLASH.
%left AND.
%left OR.
%left THIS.
%left NOT.
%left MOD.

%left IF.
%left ELSE.

%syntax_error { yapCompileSyntaxError(C, &TOKEN); }

// ---------------------------------------------------------------------------
// Chunk

chunk ::= statement_list(L).
    { C->root = L; }

// ---------------------------------------------------------------------------
// Statement List

%type statement_list
    { yapSyntax* }

%destructor statement_list
    { yapSyntaxDestroy(C->Y, $$); }

statement_list(L) ::= statement_list(OL) statement(S).
    { L = yapSyntaxListAppend(C->Y, OL, S, 0); }

statement_list(L) ::= statement(S).
    { L = yapSyntaxCreateList(C->Y, YST_STATEMENTLIST, S); }

// ---------------------------------------------------------------------------
// Statement Block

%type statement_block
    { yapSyntax* }

%destructor statement_block
    { yapSyntaxDestroy(C->Y, $$); }

statement_block(B) ::= STARTBLOCK ENDBLOCK.
    { B = yapSyntaxCreateList(C->Y, YST_STATEMENTLIST, NULL); }

statement_block(B) ::= STARTBLOCK statement_list(L) ENDBLOCK.
    { B = L; }

statement_block(B) ::= statement(S).
    { B = yapSyntaxCreateList(C->Y, YST_STATEMENTLIST, S); }

// ---------------------------------------------------------------------------
// Statement

%type statement
    { yapSyntax* }

%destructor statement
    { yapSyntaxDestroy(C->Y, $$); }

statement(S) ::= BREAK(B) ENDSTATEMENT.
    { S = yapSyntaxCreateBreak(C->Y, B.line); }

statement(S) ::= RETURN expr_list(L) ENDSTATEMENT.
    { S = yapSyntaxCreateReturn(C->Y, L); }

statement(S) ::= RETURN paren_expr_list(L) ENDSTATEMENT.
    { S = yapSyntaxCreateReturn(C->Y, L); }

statement(S) ::= expr_list(L) ENDSTATEMENT.
    { S = yapSyntaxCreateStatementExpr(C->Y, L); }

statement(S) ::= IF expr_list(COND) statement_block(IFBODY) ELSE statement_block(ELSEBODY).
    { S = yapSyntaxCreateIfElse(C->Y, COND, IFBODY, ELSEBODY); }

statement(S) ::= IF expr_list(COND) statement_block(IFBODY).
    { S = yapSyntaxCreateIfElse(C->Y, COND, IFBODY, NULL); }

statement(S) ::= WHILE expr_list(COND) statement_block(BODY).
    { S = yapSyntaxCreateWhile(C->Y, COND, BODY); }

statement(S) ::= FUNCTION(F) IDENTIFIER(I) LEFTPAREN func_args(ARGS) RIGHTPAREN statement_block(BODY).
    { S = yapSyntaxCreateFunctionDecl(C->Y, &I, ARGS, BODY, F.line); }

statement(S) ::= FOR LEFTPAREN ident_list(VARS) IN expression(ITER) RIGHTPAREN statement_block(BODY).
    { S = yapSyntaxCreateFor(C->Y, VARS, ITER, BODY); }

statement(S) ::= lvalue(L) PLUSEQUALS expression(R).
    { S = yapSyntaxCreateBinary(C->Y, YST_ADD, L, R, yTrue); }

statement(S) ::= lvalue(L) DASHEQUALS expression(R).
    { S = yapSyntaxCreateBinary(C->Y, YST_SUB, L, R, yTrue); }

statement(S) ::= lvalue(L) STAREQUALS expression(R).
    { S = yapSyntaxCreateBinary(C->Y, YST_MUL, L, R, yTrue); }

statement(S) ::= lvalue(L) SLASHEQUALS expression(R).
    { S = yapSyntaxCreateBinary(C->Y, YST_DIV, L, R, yTrue); }

statement(S) ::= lvalue(L) BITWISE_OREQUALS expression(R).
    { S = yapSyntaxCreateBinary(C->Y, YST_BITWISE_OR, L, R, yTrue); }

statement(S) ::= lvalue(L) BITWISE_ANDEQUALS expression(R).
    { S = yapSyntaxCreateBinary(C->Y, YST_BITWISE_AND, L, R, yTrue); }

statement(S) ::= lvalue(L) BITWISE_XOREQUALS expression(R).
    { S = yapSyntaxCreateBinary(C->Y, YST_BITWISE_XOR, L, R, yTrue); }

statement(S) ::= lvalue(L) SHIFTLEFTEQUALS expression(R).
    { S = yapSyntaxCreateBinary(C->Y, YST_SHIFTLEFT, L, R, yTrue); }

statement(S) ::= lvalue(L) SHIFTRIGHTEQUALS expression(R).
    { S = yapSyntaxCreateBinary(C->Y, YST_SHIFTRIGHT, L, R, yTrue); }

statement(S) ::= ENDSTATEMENT.
    { S = yapSyntaxCreateList(C->Y, YST_STATEMENTLIST, NULL); }

statement(S) ::= SCOPESTARTBLOCK ENDBLOCK.
    { S = yapSyntaxCreateList(C->Y, YST_STATEMENTLIST, NULL); }

statement(S) ::= SCOPESTARTBLOCK statement_list(L) ENDBLOCK.
    { S = yapSyntaxCreateScope(C->Y, L); }

statement ::= error ENDSTATEMENT.

// ---------------------------------------------------------------------------
// Parenthesized Expression List

%type paren_expr_list
    { yapSyntax* }

%destructor paren_expr_list
    { yapSyntaxDestroy(C->Y, $$); }

paren_expr_list(PEL) ::= LEFTPAREN expr_list(L) RIGHTPAREN.
    { PEL = L; }

paren_expr_list(PEL) ::= LEFTPAREN RIGHTPAREN.
    { PEL = yapSyntaxCreateList(C->Y, YST_EXPRESSIONLIST, NULL); }

// ---------------------------------------------------------------------------
// Expression List

%type expr_list
    { yapSyntax* }

%destructor expr_list
    { yapSyntaxDestroy(C->Y, $$); }

expr_list(EL) ::= expr_list(OL) COMMA expression(E).
    { EL = yapSyntaxListAppend(C->Y, OL, E, 0); }

expr_list(EL) ::= expr_list(OL) FATCOMMA expression(E).
    { EL = yapSyntaxListAppend(C->Y, OL, E, YSLF_AUTOLITERAL); }

expr_list(EL) ::= expression(E).
    { EL = yapSyntaxCreateList(C->Y, YST_EXPRESSIONLIST, E); }

// ---------------------------------------------------------------------------
// Expression

%type expression
    { yapSyntax* }

%destructor expression
    { yapSyntaxDestroy(C->Y, $$); }

expression(EXP) ::= NOT expression(E).
    { EXP = yapSyntaxCreateUnary(C->Y, YST_NOT, E); }

expression(EXP) ::= BITWISE_NOT expression(E).
    { EXP = yapSyntaxCreateUnary(C->Y, YST_BITWISE_NOT, E); }

expression(EXP) ::= expression(L) PLUS expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_ADD, L, R, yFalse); }

expression(EXP) ::= expression(L) DASH expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_SUB, L, R, yFalse); }

expression(EXP) ::= expression(L) STAR expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_MUL, L, R, yFalse); }

expression(EXP) ::= expression(L) SLASH expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_DIV, L, R, yFalse); }

expression(EXP) ::= expression(L) AND expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_AND, L, R, yFalse); }

expression(EXP) ::= expression(L) OR expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_OR, L, R, yFalse); }

expression(EXP) ::= expression(L) CMP expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_CMP, L, R, yFalse); }

expression(EXP) ::= expression(L) EQUALS expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_EQUALS, L, R, yFalse); }

expression(EXP) ::= expression(L) NOTEQUALS expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_NOTEQUALS, L, R, yFalse); }

expression(EXP) ::= expression(L) GREATERTHAN expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_GREATERTHAN, L, R, yFalse); }

expression(EXP) ::= expression(L) GREATERTHANOREQUAL expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_GREATERTHANOREQUAL, L, R, yFalse); }

expression(EXP) ::= expression(L) LESSTHAN expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_LESSTHAN, L, R, yFalse); }

expression(EXP) ::= expression(L) LESSTHANOREQUAL expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_LESSTHANOREQUAL, L, R, yFalse); }

expression(EXP) ::= expression(L) BITWISE_XOR expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_BITWISE_XOR, L, R, yFalse); }

expression(EXP) ::= expression(L) BITWISE_AND expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_BITWISE_AND, L, R, yFalse); }

expression(EXP) ::= expression(L) BITWISE_OR expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_BITWISE_OR, L, R, yFalse); }

expression(EXP) ::= expression(L) SHIFTLEFT expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_SHIFTLEFT, L, R, yFalse); }

expression(EXP) ::= expression(L) SHIFTRIGHT expression(R).
    { EXP = yapSyntaxCreateBinary(C->Y, YST_SHIFTRIGHT, L, R, yFalse); }

expression(EXP) ::= expression(FORMAT) MOD paren_expr_list(ARGS).
    { EXP = yapSyntaxCreateStringFormat(C->Y, FORMAT, ARGS); }

expression(EXP) ::= expression(FORMAT) MOD expression(ARGS).
    { EXP = yapSyntaxCreateStringFormat(C->Y, FORMAT, ARGS); }

expression(E) ::= lvalue(L) ASSIGN expression(R).
    { E = yapSyntaxCreateAssignment(C->Y, L, R); }

expression(E) ::= expression(L) INHERITS expression(R).
    { E = yapSyntaxCreateInherits(C->Y, L, R); }

expression(E) ::= lvalue(LV).
    { E = LV; }

expression(E) ::= INTEGER(I).
    { E = yapSyntaxCreateKInt(C->Y, &I, 0); }

expression(E) ::= NEGATIVE INTEGER(I).
    { E = yapSyntaxCreateKInt(C->Y, &I, CKO_NEGATIVE); }

expression(E) ::= FLOATNUM(F).
    { E = yapSyntaxCreateKFloat(C->Y, &F, 0); }

expression(E) ::= NEGATIVE FLOATNUM(F).
    { E = yapSyntaxCreateKFloat(C->Y, &F, CKO_NEGATIVE); }

expression(E) ::= LITERALSTRING(L).
    { E = yapSyntaxCreateKString(C->Y, &L); }

expression(E) ::= NULL(N).
    { E = yapSyntaxCreateNull(C->Y, N.line); }

expression(E) ::= FUNCTION(F) LEFTPAREN func_args(ARGS) RIGHTPAREN statement_block(BODY).
    { E = yapSyntaxCreateFunctionDecl(C->Y, NULL, ARGS, BODY, F.line); }


// ---------------------------------------------------------------------------
// LValue

%type lvalue
    { yapSyntax* }

%destructor lvalue
    { yapSyntaxDestroy(C->Y, $$); }

lvalue(L) ::= lvalue_indexable(I).
    { L = I; }

lvalue(L) ::= VAR IDENTIFIER(I).
    { L = yapSyntaxCreateVar(C->Y, yapSyntaxCreateIdentifier(C->Y, &I)); }

lvalue(L) ::= GROUPLEFTPAREN expr_list(EL) RIGHTPAREN.
    { L = EL; }

// ---------------------------------------------------------------------------
// LValueIndexable

%type lvalue_indexable
    { yapSyntax* }

%destructor lvalue_indexable
    { yapSyntaxDestroy(C->Y, $$); }

lvalue_indexable(L) ::= THIS(T).
    { L = yapSyntaxCreateThis(C->Y, T.line); }

lvalue_indexable(L) ::= lvalue_indexable(FUNC) paren_expr_list(ARGS).
    { L = yapSyntaxCreateCall(C->Y, FUNC, ARGS); }

lvalue_indexable(L) ::= lvalue_indexable(ARRAY) OPENBRACKET expression(INDEX) CLOSEBRACKET.
    { L = yapSyntaxCreateIndex(C->Y, ARRAY, INDEX, yFalse); }

lvalue_indexable(L) ::= lvalue_indexable(OBJECT) PERIOD IDENTIFIER(MEMBER).
    { L = yapSyntaxCreateIndex(C->Y, OBJECT, yapSyntaxCreateKString(C->Y, &MEMBER), yFalse); }

lvalue_indexable(L) ::= lvalue_indexable(OBJECT) COLONCOLON IDENTIFIER(MEMBER).
    { L = yapSyntaxCreateIndex(C->Y, OBJECT, yapSyntaxCreateKString(C->Y, &MEMBER), yTrue); }

lvalue_indexable(L) ::= IDENTIFIER(I).
    { L = yapSyntaxCreateIdentifier(C->Y, &I); }

// ---------------------------------------------------------------------------
// Identifier List

%type ident_list
    { yapSyntax* }

%destructor ident_list
    { yapSyntaxDestroy(C->Y, $$); }

ident_list(IL) ::= ident_list(OL) COMMA IDENTIFIER(I).
    { IL = yapSyntaxListAppend(C->Y, OL, yapSyntaxCreateIdentifier(C->Y, &I), 0); }

ident_list(IL) ::= IDENTIFIER(I).
    { IL = yapSyntaxCreateList(C->Y, YST_IDENTIFIERLIST, yapSyntaxCreateIdentifier(C->Y, &I)); }

ident_list(IL) ::= .
    { IL = yapSyntaxCreateList(C->Y, YST_IDENTIFIERLIST, NULL); }

// ---------------------------------------------------------------------------
// Function Arguments

%type func_args
    { yapSyntax* }

func_args(ARGS) ::= ident_list(IL).
    { ARGS = yapSyntaxCreateFunctionArgs(C->Y, IL, NULL); }

func_args(ARGS) ::= ident_list(IL) COMMA ELLIPSIS IDENTIFIER(VARARGS).
    { ARGS = yapSyntaxCreateFunctionArgs(C->Y, IL, &VARARGS); }

func_args(ARGS) ::= ELLIPSIS IDENTIFIER(VARARGS).
    { ARGS = yapSyntaxCreateFunctionArgs(C->Y, yapSyntaxCreateList(C->Y, YST_IDENTIFIERLIST, NULL), &VARARGS); }
