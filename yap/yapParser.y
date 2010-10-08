
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
    #include "yapBlock.h"
    #include "yapCode.h"
    #include "yapCompiler.h"
    #include "yapLexer.h"
    #include "yapModule.h"
    #include "yapOp.h"
    #include "yapParser.h"

    #include <string.h>
    #include <stdlib.h>

    #define assert(ignoring_this_function)
    #define YYNOERRORRECOVERY 1
}

%left UNKNOWN.
%left COMMENT.
%left SPACE.
%left EOF.

%syntax_error { yapCompileSyntaxError(compiler, TOKEN.text); }

// ---------------------------------------------------------------------------
// Module

module ::= statement_list(L).
    { yapCompileModuleStatementList(compiler, L); }

// ---------------------------------------------------------------------------
// Statement List

%type statement_list
    { yapCode* }

%destructor statement_list
    { yapCodeDestroy($$); }

statement_list(L) ::= statement_list(OL) statement(S).
    { L = yapCompileStatementListAppend(compiler, OL, S); }

statement_list(L) ::= statement(S).
    { L = S; }

// ---------------------------------------------------------------------------
// Statement

%type statement
    { yapCode* }

%destructor statement
    { yapCodeDestroy($$); }

statement(S) ::= IDENTIFIER(I) EQUALS expression(E) NEWLINE.
    { S = yapCompileStatementAssignment(compiler, &I, E); }

statement(S) ::= VAR IDENTIFIER(I) EQUALS expression(E) NEWLINE.
    { S = yapCompileStatementVarInit(compiler, &I, E); }

statement(S) ::= VAR IDENTIFIER(I) NEWLINE.
    { S = yapCompileStatementVar(compiler, &I); }

statement(S) ::= RETURN expr_list(L) NEWLINE.
    { S = yapCompileStatementReturn(compiler, L); }

statement(S) ::= expr_list(L) NEWLINE.
    { S = yapCompileStatementExpressionList(compiler, L); }

statement(S) ::= IF expr_list(COND) NEWLINE INDENT statement_list(IFBODY) DEDENT ELSE NEWLINE INDENT statement_list(ELSEBODY) DEDENT.
    { S = yapCompileStatementIfElse(compiler, COND, IFBODY, ELSEBODY); }

statement(S) ::= IF expr_list(COND) NEWLINE INDENT statement_list(IFBODY) DEDENT.
    { S = yapCompileStatementIfElse(compiler, COND, IFBODY, NULL); }

statement(S) ::= WHILE expr_list(COND) NEWLINE INDENT statement_list(BODY) DEDENT.
    { S = yapCompileStatementLoop(compiler, NULL, COND, NULL, BODY); }

statement(S) ::= FOR LEFTPAREN expr_list(INIT) SEMI expr_list(COND) SEMI expr_list(INCR) RIGHTPAREN NEWLINE INDENT statement_list(BODY) DEDENT.
    { S = yapCompileStatementLoop(compiler, INIT, COND, INCR, BODY); }

statement(S) ::= FUNCTION IDENTIFIER(I) LEFTPAREN ident_list(ARGS) RIGHTPAREN NEWLINE INDENT statement_list(BODY) DEDENT.
    { S = yapCompileStatementFunctionDecl(compiler, &I, ARGS, BODY); }

// ---------------------------------------------------------------------------
// Expression List

%type expr_list
    { yapArray* }

%destructor expr_list
    { yapArrayDestroy($$, (yapDestroyCB)yapCodeDestroy); }

expr_list(EL) ::= LEFTPAREN expr_list(OL) RIGHTPAREN.
    { EL = OL; }

expr_list(EL) ::= expr_list(OL) COMMA expression(E).
    { EL = yapCompileExpressionListAppend(compiler, OL, E); }

expr_list(EL) ::= expression(E).
    { EL = yapCompileExpressionListCreate(compiler, E); }

expr_list(EL) ::= .
    { EL = yapCompileExpressionListCreate(compiler, NULL); }

// ---------------------------------------------------------------------------
// Expression

%type expression
    { yapCode* }

%destructor expression
    { yapCodeDestroy($$); }

expression(E) ::= IDENTIFIER(F) LEFTPAREN expr_list(ARGS) RIGHTPAREN.
    { E = yapCodeCreateCall(compiler, &F, ARGS); }

expression(E) ::= INTEGER(I).
    { E = yapCodeCreateInteger(compiler, &I); }

expression(E) ::= LITERALSTRING(L).
    { E = yapCodeCreateLiteralString(compiler, &L); }

expression(E) ::= IDENTIFIER(I).
    { E = yapCodeCreateIdentifier(compiler, &I); }

expression(E) ::= NULL.
    { E = yapCodeCreateNull(compiler); }

// ---------------------------------------------------------------------------
// Identifier List

%type ident_list
    { yapArray* }

%destructor ident_list 
    { yapArrayDestroy($$, (yapDestroyCB)yapTokenDestroy); }

ident_list(IL) ::= ident_list(OL) COMMA IDENTIFIER(I).
    { IL = yapCompileIdentifierListAppend(compiler, OL, &I); }

ident_list(IL) ::= IDENTIFIER(I).
    { IL = yapCompileIdentifierListCreate(compiler, &I); }

ident_list(IL) ::= .
    { IL = yapCompileIdentifierListCreate(compiler, NULL); }

