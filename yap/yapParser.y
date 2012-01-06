
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

    #undef assert
    #define assert(ignoring_this_function)
    #define YYNOERRORRECOVERY 1
}

%fallback GROUPLEFTPAREN LEFTPAREN.
%fallback SCOPESTARTBLOCK STARTBLOCK.
%fallback NEGATIVE DASH.

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
%left COLON.

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
%left INT.
%left FLOAT.
%left STRING.
%left NOT.
%left MOD.

%left IF.
%left ELSE.

%syntax_error { yapCompileSyntaxError(compiler, &TOKEN); }

// ---------------------------------------------------------------------------
// Chunk

chunk ::= statement_list(L).
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

statement(S) ::= WHILE expr_list(COND) statement_block(BODY).
    { S = yapSyntaxCreateWhile(COND, BODY); }

statement(S) ::= FUNCTION IDENTIFIER(I) LEFTPAREN ident_list(ARGS) RIGHTPAREN statement_block(BODY).
    { S = yapSyntaxCreateFunctionDecl(&I, ARGS, BODY); }

statement(S) ::= FOR LEFTPAREN ident_list(VARS) IN expression(ITER) RIGHTPAREN statement_block(BODY).
    { S = yapSyntaxCreateFor(VARS, ITER, BODY); }

statement(S) ::= lvalue(L) PLUSEQUALS expression(R).
    { S = yapSyntaxCreateBinary(YST_ADD, L, R, yTrue); }

statement(S) ::= lvalue(L) DASHEQUALS expression(R).
    { S = yapSyntaxCreateBinary(YST_SUB, L, R, yTrue); }

statement(S) ::= lvalue(L) STAREQUALS expression(R).
    { S = yapSyntaxCreateBinary(YST_MUL, L, R, yTrue); }

statement(S) ::= lvalue(L) SLASHEQUALS expression(R).
    { S = yapSyntaxCreateBinary(YST_DIV, L, R, yTrue); }

statement(S) ::= lvalue(L) BITWISE_OREQUALS expression(R).
    { S = yapSyntaxCreateBinary(YST_BITWISE_OR, L, R, yTrue); }

statement(S) ::= lvalue(L) BITWISE_ANDEQUALS expression(R).
    { S = yapSyntaxCreateBinary(YST_BITWISE_AND, L, R, yTrue); }

statement(S) ::= lvalue(L) BITWISE_XOREQUALS expression(R).
    { S = yapSyntaxCreateBinary(YST_BITWISE_XOR, L, R, yTrue); }

statement(S) ::= lvalue(L) SHIFTLEFTEQUALS expression(R).
    { S = yapSyntaxCreateBinary(YST_SHIFTLEFT, L, R, yTrue); }

statement(S) ::= lvalue(L) SHIFTRIGHTEQUALS expression(R).
    { S = yapSyntaxCreateBinary(YST_SHIFTRIGHT, L, R, yTrue); }

statement(S) ::= ENDSTATEMENT.
    { S = yapSyntaxCreateList(YST_STATEMENTLIST, NULL); }

statement(S) ::= SCOPESTARTBLOCK ENDBLOCK.
    { S = yapSyntaxCreateList(YST_STATEMENTLIST, NULL); }

statement(S) ::= SCOPESTARTBLOCK statement_list(L) ENDBLOCK.
    { S = yapSyntaxCreateScope(L); }

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

expression(C) ::= FLOAT expression(E).
    { C = yapSyntaxCreateUnary(YST_TOFLOAT, E); }

expression(C) ::= STRING expression(E).
    { C = yapSyntaxCreateUnary(YST_TOSTRING, E); }

expression(C) ::= NOT expression(E).
    { C = yapSyntaxCreateUnary(YST_NOT, E); }

expression(C) ::= BITWISE_NOT expression(E).
    { C = yapSyntaxCreateUnary(YST_BITWISE_NOT, E); }

expression(C) ::= expression(L) PLUS expression(R).
    { C = yapSyntaxCreateBinary(YST_ADD, L, R, yFalse); }

expression(C) ::= expression(L) DASH expression(R).
    { C = yapSyntaxCreateBinary(YST_SUB, L, R, yFalse); }

expression(C) ::= expression(L) STAR expression(R).
    { C = yapSyntaxCreateBinary(YST_MUL, L, R, yFalse); }

expression(C) ::= expression(L) SLASH expression(R).
    { C = yapSyntaxCreateBinary(YST_DIV, L, R, yFalse); }

expression(C) ::= expression(L) AND expression(R).
    { C = yapSyntaxCreateBinary(YST_AND, L, R, yFalse); }

expression(C) ::= expression(L) OR expression(R).
    { C = yapSyntaxCreateBinary(YST_OR, L, R, yFalse); }

expression(C) ::= expression(L) CMP expression(R).
    { C = yapSyntaxCreateBinary(YST_CMP, L, R, yFalse); }

expression(C) ::= expression(L) EQUALS expression(R).
    { C = yapSyntaxCreateBinary(YST_EQUALS, L, R, yFalse); }

expression(C) ::= expression(L) NOTEQUALS expression(R).
    { C = yapSyntaxCreateBinary(YST_NOTEQUALS, L, R, yFalse); }

expression(C) ::= expression(L) GREATERTHAN expression(R).
    { C = yapSyntaxCreateBinary(YST_GREATERTHAN, L, R, yFalse); }

expression(C) ::= expression(L) GREATERTHANOREQUAL expression(R).
    { C = yapSyntaxCreateBinary(YST_GREATERTHANOREQUAL, L, R, yFalse); }

expression(C) ::= expression(L) LESSTHAN expression(R).
    { C = yapSyntaxCreateBinary(YST_LESSTHAN, L, R, yFalse); }

expression(C) ::= expression(L) LESSTHANOREQUAL expression(R).
    { C = yapSyntaxCreateBinary(YST_LESSTHANOREQUAL, L, R, yFalse); }

expression(C) ::= expression(L) BITWISE_XOR expression(R).
    { C = yapSyntaxCreateBinary(YST_BITWISE_XOR, L, R, yFalse); }

expression(C) ::= expression(L) BITWISE_AND expression(R).
    { C = yapSyntaxCreateBinary(YST_BITWISE_AND, L, R, yFalse); }

expression(C) ::= expression(L) BITWISE_OR expression(R).
    { C = yapSyntaxCreateBinary(YST_BITWISE_OR, L, R, yFalse); }

expression(C) ::= expression(L) SHIFTLEFT expression(R).
    { C = yapSyntaxCreateBinary(YST_SHIFTLEFT, L, R, yFalse); }

expression(C) ::= expression(L) SHIFTRIGHT expression(R).
    { C = yapSyntaxCreateBinary(YST_SHIFTRIGHT, L, R, yFalse); }

expression(C) ::= expression(FORMAT) MOD paren_expr_list(ARGS).
    { C = yapSyntaxCreateStringFormat(FORMAT, ARGS); }

expression(C) ::= GROUPLEFTPAREN expr_list(EL) RIGHTPAREN.
    { C = EL; }

expression(E) ::= lvalue(L) ASSIGN expression(R).
    { E = yapSyntaxCreateAssignment(L, R); }

expression(E) ::= lvalue(L) INHERITS expression(R).
    { E = yapSyntaxCreateInherits(L, R); }

expression(E) ::= lvalue(LV).
    { E = LV; }

expression(E) ::= INTEGER(I).
    { E = yapSyntaxCreateKInt(&I, 0); }

expression(E) ::= NEGATIVE INTEGER(I).
    { E = yapSyntaxCreateKInt(&I, CKO_NEGATIVE); }

expression(E) ::= FLOATNUM(F).
    { E = yapSyntaxCreateKFloat(&F, 0); }

expression(E) ::= NEGATIVE FLOATNUM(F).
    { E = yapSyntaxCreateKFloat(&F, CKO_NEGATIVE); }

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
