
%name yapParse

%token_prefix YTT_

%token_type
    { yapToken }

%default_type
    { yapToken }

%extra_argument 
    { yapCompiler *compiler }

%include
{
    #include "yapCompiler.h"
    #include "yapLexer.h"
    #include "yapParser.h"
    #include "yapSyntax.h"

    #include <string.h>
    #include <stdlib.h>

    #define assert(ignoring_this_function)
    #define YYNOERRORRECOVERY 1
}

%left NEWLINE.
%left OPENBRACE.
%left CLOSEBRACE.
%left SEMI.

%left UNKNOWN.
%left COMMENT.
%left SPACE.
%left EOF.
%left PLUS.
%left DASH.
%left STAR.
%left SLASH.
%left INT.
%left STRING.
%left MOD.

%syntax_error { yapSyntaxTreeSyntaxError(compiler->tree, &TOKEN); }

// ---------------------------------------------------------------------------
// Module

module ::= statement_list(L).
    { compiler->tree->root = L; }

// ---------------------------------------------------------------------------
// Statement List

%type statement_list
    { yapSyntax* }

%destructor statement_list
    { yapSyntaxDestroy($$); }

statement_list(L) ::= statement_list(OL) statement(S).
    { L = yapSyntaxListAppend(OL, S); }

statement_list(L) ::= statement(S).
    { L = yapSyntaxCreateList(YST_STATEMENTLIST, S); }

// ---------------------------------------------------------------------------
// Statement

%type statement
    { yapSyntax* }

%destructor statement
    { yapSyntaxDestroy($$); }

statement(S) ::= IDENTIFIER(I) EQUALS complex_expression(E) ENDSTATEMENT.
    { S = yapSyntaxCreateAssignment(&I, E); }

statement(S) ::= VAR IDENTIFIER(I) EQUALS complex_expression(E) ENDSTATEMENT.
    { S = yapSyntaxCreateVar(&I, E); }

statement(S) ::= VAR IDENTIFIER(I) ENDSTATEMENT.
    { S = yapSyntaxCreateVar(&I, NULL); }

statement(S) ::= RETURN expr_list(L) ENDSTATEMENT.
    { S = yapSyntaxCreateReturn(L); }

statement(S) ::= expr_list(L) ENDSTATEMENT.
    { S = yapSyntaxCreateStatementExpr(L); }

statement(S) ::= IF expr_list(COND) STARTBLOCK statement_list(IFBODY) ENDBLOCK ELSE STARTBLOCK statement_list(ELSEBODY) ENDBLOCK.
    { S = yapSyntaxCreateIfElse(COND, IFBODY, ELSEBODY); }

statement(S) ::= IF expr_list(COND) STARTBLOCK statement_list(IFBODY) ENDBLOCK.
    { S = yapSyntaxCreateIfElse(COND, IFBODY, NULL); }

statement(S) ::= WHILE expr_list(COND) STARTBLOCK statement_list(BODY) ENDBLOCK.
    { S = yapSyntaxCreateLoop(COND, BODY); }

statement(S) ::= FUNCTION IDENTIFIER(I) LEFTPAREN ident_list(ARGS) RIGHTPAREN STARTBLOCK statement_list(BODY) ENDBLOCK.
    { S = yapSyntaxCreateFunctionDecl(&I, ARGS, BODY); }

// ---------------------------------------------------------------------------
// Expression List

%type expr_list
    { yapSyntax* }

%destructor expr_list
    { yapSyntaxDestroy($$); }

expr_list(EL) ::= expr_list(OL) COMMA complex_expression(E).
    { EL = yapSyntaxListAppend(OL, E); }

expr_list(EL) ::= complex_expression(E).
    { EL = yapSyntaxCreateList(YST_EXPRESSIONLIST, E); }

expr_list(EL) ::= .
    { EL = yapSyntaxCreateList(YST_EXPRESSIONLIST, NULL); }

// ---------------------------------------------------------------------------
// Complex Expressions

%type complex_expression
    { yapSyntax* }

%destructor expression
    { yapSyntaxDestroy($$); }

complex_expression(C) ::= INT complex_expression(E).
    { C = yapSyntaxCreateUnary(YST_TOINT, E); }

complex_expression(C) ::= STRING complex_expression(E).
    { C = yapSyntaxCreateUnary(YST_TOSTRING, E); }

complex_expression(C) ::= complex_expression(OC) PLUS complex_expression(E).
    { C = yapSyntaxCreateCombine(YST_ADD, OC, E); }

complex_expression(C) ::= complex_expression(OC) DASH complex_expression(E).
    { C = yapSyntaxCreateCombine(YST_SUB, OC, E); }

complex_expression(C) ::= complex_expression(OC) STAR complex_expression(E).
    { C = yapSyntaxCreateCombine(YST_MUL, OC, E); }

complex_expression(C) ::= complex_expression(OC) SLASH complex_expression(E).
    { C = yapSyntaxCreateCombine(YST_DIV, OC, E); }

complex_expression(C) ::= complex_expression(FORMAT) MOD LEFTPAREN expr_list(ARGS) RIGHTPAREN.
    { C = yapSyntaxCreateStringFormat(FORMAT, ARGS); }

complex_expression(C) ::= LEFTPAREN complex_expression(E) RIGHTPAREN.
    { C = E; }

complex_expression(C) ::= expression(E).
    { C = E; }

// ---------------------------------------------------------------------------
// Expression

%type expression
    { yapSyntax* }

%destructor expression
    { yapSyntaxDestroy($$); }

expression(E) ::= IDENTIFIER(FUNCNAME) LEFTPAREN expr_list(ARGS) RIGHTPAREN.
    { E = yapSyntaxCreateCall(&FUNCNAME, ARGS); }

expression(E) ::= INTEGER(I).
    { E = yapSyntaxCreateKInt(&I); }

expression(E) ::= LITERALSTRING(L).
    { E = yapSyntaxCreateKString(&L); }

expression(E) ::= IDENTIFIER(I).
    { E = yapSyntaxCreateIdentifier(&I); }

expression(E) ::= NULL.
    { E = yapSyntaxCreateNull(); }

// ---------------------------------------------------------------------------
// Identifier List

%type ident_list
    { yapSyntax* }

%destructor ident_list 
    { yapSyntaxDestroy($$); }

ident_list(IL) ::= ident_list(OL) COMMA IDENTIFIER(I).
    { IL = yapSyntaxListAppend(OL, yapSyntaxCreateIdentifier(&I)); }

ident_list(IL) ::= IDENTIFIER(I).
    { IL = yapSyntaxCreateList(YST_IDENTIFIERLIST, yapSyntaxCreateIdentifier(&I)); }

ident_list(IL) ::= .
    { IL = yapSyntaxCreateList(YST_IDENTIFIERLIST, NULL); }

