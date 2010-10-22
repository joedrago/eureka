
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
%left EQUALS.

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

%syntax_error { yapCompileSyntaxError(compiler, &TOKEN); }

// ---------------------------------------------------------------------------
// Module

module ::= statement_list(L).
    { compiler->root = L; }

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

expr_list(EL) ::= expr_list(OL) COMMA expression(E).
    { EL = yapSyntaxListAppend(OL, E); }

expr_list(EL) ::= expression(E).
    { EL = yapSyntaxCreateList(YST_EXPRESSIONLIST, E); }

expr_list(EL) ::= .
    { EL = yapSyntaxCreateList(YST_EXPRESSIONLIST, NULL); }

// ---------------------------------------------------------------------------
// Expression

%type expression
    { yapSyntax* }

%destructor expression
    { yapSyntaxDestroy($$); }

expression(C) ::= INT expression(E).
    { C = yapSyntaxCreateUnary(YST_TOINT, E); }

expression(C) ::= STRING expression(E).
    { C = yapSyntaxCreateUnary(YST_TOSTRING, E); }

expression(C) ::= expression(OC) PLUS expression(E).
    { C = yapSyntaxCreateBinary(YST_ADD, OC, E); }

expression(C) ::= expression(OC) DASH expression(E).
    { C = yapSyntaxCreateBinary(YST_SUB, OC, E); }

expression(C) ::= expression(OC) STAR expression(E).
    { C = yapSyntaxCreateBinary(YST_MUL, OC, E); }

expression(C) ::= expression(OC) SLASH expression(E).
    { C = yapSyntaxCreateBinary(YST_DIV, OC, E); }

expression(C) ::= expression(FORMAT) MOD LEFTPAREN expr_list(ARGS) RIGHTPAREN.
    { C = yapSyntaxCreateStringFormat(FORMAT, ARGS); }

expression(C) ::= LEFTPAREN expression(E) RIGHTPAREN.
    { C = E; }

expression(E) ::= lvalue(L) EQUALS expression(R).
    { E = yapSyntaxCreateAssignment(L, R); }

expression(E) ::= lvalue(LV).
    { E = LV; }

expression(E) ::= IDENTIFIER(FUNCNAME) LEFTPAREN expr_list(ARGS) RIGHTPAREN.
    { E = yapSyntaxCreateCall(&FUNCNAME, ARGS); }

expression(E) ::= INTEGER(I).
    { E = yapSyntaxCreateKInt(&I); }

expression(E) ::= LITERALSTRING(L).
    { E = yapSyntaxCreateKString(&L); }

expression(E) ::= NULL.
    { E = yapSyntaxCreateNull(); }

// ---------------------------------------------------------------------------
// LValue

%type lvalue
    { yapSyntax* }

%destructor lvalue
    { yapSyntaxDestroy($$); }

lvalue(L) ::= IDENTIFIER(I).
    { L = yapSyntaxCreateIdentifier(&I); }

lvalue(L) ::= VAR IDENTIFIER(I).
    { L = yapSyntaxCreateVar(yapSyntaxCreateIdentifier(&I)); }

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
