
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

%fallback GROUPLEFTPAREN LEFTPAREN.

%left NEWLINE.
%left OPENBRACE.
%left CLOSEBRACE.
%left OPENBRACKET.
%left CLOSEBRACKET.
%left SEMI.
%left EQUALS.
%left INHERITS.
%left PERIOD.
%left LEFTPAREN.
%left COLON.

%left UNKNOWN.
%left COMMENT.
%left SPACE.
%left EOF.
%left PLUS.
%left DASH.
%left STAR.
%left SLASH.
%left AND.
%left OR.
%left INT.
%left STRING.
%left NOT.
%left MOD.

%left IF.
%left ELSE.

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
// Statement Block

%type statement_block
    { yapSyntax* }

%destructor statement_block
    { yapSyntaxDestroy($$); }

statement_block(B) ::= STARTBLOCK ENDBLOCK.
    { B = yapSyntaxCreateList(YST_STATEMENTLIST, NULL); }

statement_block(B) ::= STARTBLOCK statement_list(L) ENDBLOCK.
    { B = L; }

statement_block(B) ::= statement(S).
    { B = yapSyntaxCreateList(YST_STATEMENTLIST, S); }

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

statement(S) ::= IF expr_list(COND) statement_block(IFBODY) ELSE statement_block(ELSEBODY).
    { S = yapSyntaxCreateIfElse(COND, IFBODY, ELSEBODY); }

statement(S) ::= IF expr_list(COND) statement_block(IFBODY).
    { S = yapSyntaxCreateIfElse(COND, IFBODY, NULL); }

statement(S) ::= WITH expr_list(OBJ) statement_block(BODY).
    { S = yapSyntaxCreateWith(OBJ, BODY); }

statement(S) ::= WHILE expr_list(COND) statement_block(BODY).
    { S = yapSyntaxCreateWhile(COND, BODY); }

statement(S) ::= FUNCTION IDENTIFIER(I) LEFTPAREN ident_list(ARGS) RIGHTPAREN statement_block(BODY).
    { S = yapSyntaxCreateFunctionDecl(&I, ARGS, BODY); }

statement(S) ::= FOR LEFTPAREN ident_list(VARS) IN expression(ITER) RIGHTPAREN statement_block(BODY).
    { S = yapSyntaxCreateFor(VARS, ITER, BODY); }

statement(S) ::= ENDSTATEMENT.
    { S = yapSyntaxCreateList(YST_STATEMENTLIST, NULL); }

// ---------------------------------------------------------------------------
// Parenthesized Expression List

%type paren_expr_list
    { yapSyntax* }

%destructor paren_expr_list
    { yapSyntaxDestroy($$); }

paren_expr_list(PEL) ::= LEFTPAREN expr_list(L) RIGHTPAREN.
    { PEL = L; }

paren_expr_list(PEL) ::= LEFTPAREN RIGHTPAREN.
    { PEL = yapSyntaxCreateList(YST_EXPRESSIONLIST, NULL); }

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

expression(C) ::= NOT expression(E).
    { C = yapSyntaxCreateUnary(YST_NOT, E); }

expression(C) ::= expression(OC) PLUS expression(E).
    { C = yapSyntaxCreateBinary(YST_ADD, OC, E); }

expression(C) ::= expression(OC) DASH expression(E).
    { C = yapSyntaxCreateBinary(YST_SUB, OC, E); }

expression(C) ::= expression(OC) STAR expression(E).
    { C = yapSyntaxCreateBinary(YST_MUL, OC, E); }

expression(C) ::= expression(OC) SLASH expression(E).
    { C = yapSyntaxCreateBinary(YST_DIV, OC, E); }

expression(C) ::= expression(OC) AND expression(E).
    { C = yapSyntaxCreateBinary(YST_AND, OC, E); }

expression(C) ::= expression(OC) OR expression(E).
    { C = yapSyntaxCreateBinary(YST_OR, OC, E); }

expression(C) ::= expression(FORMAT) MOD paren_expr_list(ARGS).
    { C = yapSyntaxCreateStringFormat(FORMAT, ARGS); }

expression(C) ::= GROUPLEFTPAREN expr_list(EL) RIGHTPAREN.
    { C = EL; }

expression(E) ::= lvalue(L) EQUALS expression(R).
    { E = yapSyntaxCreateAssignment(L, R); }

expression(E) ::= lvalue(L) INHERITS expression(R).
    { E = yapSyntaxCreateInherits(L, R); }

expression(E) ::= lvalue(LV).
    { E = LV; }

expression(E) ::= INTEGER(I).
    { E = yapSyntaxCreateKInt(&I); }

expression(E) ::= LITERALSTRING(L).
    { E = yapSyntaxCreateKString(&L); }

expression(E) ::= NULL.
    { E = yapSyntaxCreateNull(); }

expression(E) ::= FUNCTION LEFTPAREN ident_list(ARGS) RIGHTPAREN statement_block(BODY).
    { E = yapSyntaxCreateFunctionDecl(NULL, ARGS, BODY); }


// ---------------------------------------------------------------------------
// LValue

%type lvalue
    { yapSyntax* }

%destructor lvalue
    { yapSyntaxDestroy($$); }

lvalue(L) ::= lvalue_indexable(I).
    { L = I; }

lvalue(L) ::= VAR IDENTIFIER(I).
    { L = yapSyntaxCreateVar(yapSyntaxCreateIdentifier(&I)); }

// ---------------------------------------------------------------------------
// LValueIndexable

%type lvalue_indexable
    { yapSyntax* }

%destructor lvalue_indexable
    { yapSyntaxDestroy($$); }

lvalue_indexable(L) ::= lvalue_indexable(FUNC) paren_expr_list(ARGS).
    { L = yapSyntaxCreateCall(FUNC, ARGS); }

lvalue_indexable(L) ::= lvalue_indexable(ARRAY) OPENBRACKET expression(INDEX) CLOSEBRACKET.
    { L = yapSyntaxCreateIndex(ARRAY, INDEX); }

lvalue_indexable(L) ::= lvalue_indexable(OBJ) COLON IDENTIFIER(MEMBER) paren_expr_list(ARGS).
    { L = yapSyntaxCreateIndexedCall(OBJ, yapSyntaxCreateKString(&MEMBER), ARGS); }

lvalue_indexable(L) ::= lvalue_indexable(OBJECT) PERIOD IDENTIFIER(MEMBER).
    { L = yapSyntaxCreateIndex(OBJECT, yapSyntaxCreateKString(&MEMBER)); }

lvalue_indexable(L) ::= IDENTIFIER(I).
    { L = yapSyntaxCreateIdentifier(&I); }

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
