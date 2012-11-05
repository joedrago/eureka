// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

%name ekParse

%token_prefix YTT_

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
%fallback NEGATIVE DASH.

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
    { ekSyntaxDestroy(C->Y, $$); }

statement_list(L) ::= statement_list(OL) statement(S).
    { L = ekSyntaxListAppend(C->Y, OL, S, 0); }

statement_list(L) ::= statement(S).
    { L = ekSyntaxCreateList(C->Y, YST_STATEMENTLIST, S); }

// ---------------------------------------------------------------------------
// Statement Block

%type statement_block
    { ekSyntax* }

%destructor statement_block
    { ekSyntaxDestroy(C->Y, $$); }

statement_block(B) ::= STARTBLOCK ENDBLOCK.
    { B = ekSyntaxCreateList(C->Y, YST_STATEMENTLIST, NULL); }

statement_block(B) ::= STARTBLOCK statement_list(L) ENDBLOCK.
    { B = L; }

statement_block(B) ::= statement(S).
    { B = ekSyntaxCreateList(C->Y, YST_STATEMENTLIST, S); }

// ---------------------------------------------------------------------------
// Statement

%type statement
    { ekSyntax* }

%destructor statement
    { ekSyntaxDestroy(C->Y, $$); }

statement(S) ::= BREAK(B) ENDSTATEMENT.
    { S = ekSyntaxCreateBreak(C->Y, B.line); }

statement(S) ::= RETURN expr_list(L) ENDSTATEMENT.
    { S = ekSyntaxCreateReturn(C->Y, L); }

statement(S) ::= RETURN paren_expr_list(L) ENDSTATEMENT.
    { S = ekSyntaxCreateReturn(C->Y, L); }

statement(S) ::= expr_list(L) ENDSTATEMENT.
    { S = ekSyntaxCreateStatementExpr(C->Y, L); }

statement(S) ::= IF expr_list(COND) statement_block(IFBODY) ELSE statement_block(ELSEBODY).
    { S = ekSyntaxCreateIfElse(C->Y, COND, IFBODY, ELSEBODY, yFalse); }

statement(S) ::= IF expr_list(COND) statement_block(IFBODY).
    { S = ekSyntaxCreateIfElse(C->Y, COND, IFBODY, NULL, yFalse); }

statement(S) ::= WHILE expr_list(COND) statement_block(BODY).
    { S = ekSyntaxCreateWhile(C->Y, COND, BODY); }

statement(S) ::= FUNCTION(F) IDENTIFIER(I) LEFTPAREN func_args(ARGS) RIGHTPAREN statement_block(BODY).
    { S = ekSyntaxCreateFunctionDecl(C->Y, &I, ARGS, BODY, F.line); }

statement(S) ::= FOR LEFTPAREN ident_list(VARS) IN expression(ITER) RIGHTPAREN statement_block(BODY).
    { S = ekSyntaxCreateFor(C->Y, VARS, ITER, BODY); }

statement(S) ::= lvalue(L) PLUSEQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->Y, YST_ADD, L, R, yTrue); }

statement(S) ::= lvalue(L) DASHEQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->Y, YST_SUB, L, R, yTrue); }

statement(S) ::= lvalue(L) STAREQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->Y, YST_MUL, L, R, yTrue); }

statement(S) ::= lvalue(L) SLASHEQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->Y, YST_DIV, L, R, yTrue); }

statement(S) ::= lvalue(L) BITWISE_OREQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->Y, YST_BITWISE_OR, L, R, yTrue); }

statement(S) ::= lvalue(L) BITWISE_ANDEQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->Y, YST_BITWISE_AND, L, R, yTrue); }

statement(S) ::= lvalue(L) BITWISE_XOREQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->Y, YST_BITWISE_XOR, L, R, yTrue); }

statement(S) ::= lvalue(L) SHIFTLEFTEQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->Y, YST_SHIFTLEFT, L, R, yTrue); }

statement(S) ::= lvalue(L) SHIFTRIGHTEQUALS expression(R).
    { S = ekSyntaxCreateBinary(C->Y, YST_SHIFTRIGHT, L, R, yTrue); }

statement(S) ::= ENDSTATEMENT.
    { S = ekSyntaxCreateList(C->Y, YST_STATEMENTLIST, NULL); }

statement(S) ::= SCOPESTARTBLOCK ENDBLOCK.
    { S = ekSyntaxCreateList(C->Y, YST_STATEMENTLIST, NULL); }

statement(S) ::= SCOPESTARTBLOCK statement_list(L) ENDBLOCK.
    { S = ekSyntaxCreateScope(C->Y, L); }

statement ::= error ENDSTATEMENT.

// ---------------------------------------------------------------------------
// Parenthesized Expression List

%type paren_expr_list
    { ekSyntax* }

%destructor paren_expr_list
    { ekSyntaxDestroy(C->Y, $$); }

paren_expr_list(PEL) ::= LEFTPAREN expr_list(L) RIGHTPAREN.
    { PEL = L; }

paren_expr_list(PEL) ::= LEFTPAREN RIGHTPAREN.
    { PEL = ekSyntaxCreateList(C->Y, YST_EXPRESSIONLIST, NULL); }

// ---------------------------------------------------------------------------
// Expression List

%type expr_list
    { ekSyntax* }

%destructor expr_list
    { ekSyntaxDestroy(C->Y, $$); }

expr_list(EL) ::= expr_list(OL) COMMA expression(E).
    { EL = ekSyntaxListAppend(C->Y, OL, E, 0); }

expr_list(EL) ::= expr_list(OL) FATCOMMA expression(E).
    { EL = ekSyntaxListAppend(C->Y, OL, E, YSLF_AUTOLITERAL); }

expr_list(EL) ::= expression(E).
    { EL = ekSyntaxCreateList(C->Y, YST_EXPRESSIONLIST, E); }

// ---------------------------------------------------------------------------
// Expression

%type expression
    { ekSyntax* }

%destructor expression
    { ekSyntaxDestroy(C->Y, $$); }

expression(EXP) ::= NOT expression(E).
    { EXP = ekSyntaxCreateUnary(C->Y, YST_NOT, E); }

expression(EXP) ::= BITWISE_NOT expression(E).
    { EXP = ekSyntaxCreateUnary(C->Y, YST_BITWISE_NOT, E); }

expression(EXP) ::= expression(L) PLUS expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_ADD, L, R, yFalse); }

expression(EXP) ::= expression(L) DASH expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_SUB, L, R, yFalse); }

expression(EXP) ::= expression(L) STAR expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_MUL, L, R, yFalse); }

expression(EXP) ::= expression(L) SLASH expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_DIV, L, R, yFalse); }

expression(EXP) ::= expression(L) AND expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_AND, L, R, yFalse); }

expression(EXP) ::= expression(L) OR expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_OR, L, R, yFalse); }

expression(EXP) ::= expression(L) CMP expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_CMP, L, R, yFalse); }

expression(EXP) ::= expression(L) EQUALS expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_EQUALS, L, R, yFalse); }

expression(EXP) ::= expression(L) NOTEQUALS expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_NOTEQUALS, L, R, yFalse); }

expression(EXP) ::= expression(L) GREATERTHAN expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_GREATERTHAN, L, R, yFalse); }

expression(EXP) ::= expression(L) GREATERTHANOREQUAL expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_GREATERTHANOREQUAL, L, R, yFalse); }

expression(EXP) ::= expression(L) LESSTHAN expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_LESSTHAN, L, R, yFalse); }

expression(EXP) ::= expression(L) LESSTHANOREQUAL expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_LESSTHANOREQUAL, L, R, yFalse); }

expression(EXP) ::= expression(L) BITWISE_XOR expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_BITWISE_XOR, L, R, yFalse); }

expression(EXP) ::= expression(L) BITWISE_AND expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_BITWISE_AND, L, R, yFalse); }

expression(EXP) ::= expression(L) BITWISE_OR expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_BITWISE_OR, L, R, yFalse); }

expression(EXP) ::= expression(L) SHIFTLEFT expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_SHIFTLEFT, L, R, yFalse); }

expression(EXP) ::= expression(L) SHIFTRIGHT expression(R).
    { EXP = ekSyntaxCreateBinary(C->Y, YST_SHIFTRIGHT, L, R, yFalse); }

expression(EXP) ::= expression(FORMAT) MOD paren_expr_list(ARGS).
    { EXP = ekSyntaxCreateStringFormat(C->Y, FORMAT, ARGS); }

expression(EXP) ::= expression(FORMAT) MOD expression(ARGS).
    { EXP = ekSyntaxCreateStringFormat(C->Y, FORMAT, ARGS); }

expression(E) ::= lvalue(L) ASSIGN expression(R).
    { E = ekSyntaxCreateAssignment(C->Y, L, R); }

expression(E) ::= expression(L) INHERITS expression(R).
    { E = ekSyntaxCreateInherits(C->Y, L, R); }

expression(E) ::= lvalue(LV).
    { E = LV; }

expression(E) ::= INTEGER(I).
    { E = ekSyntaxCreateKInt(C->Y, &I, 0); }

expression(E) ::= NEGATIVE INTEGER(I).
    { E = ekSyntaxCreateKInt(C->Y, &I, CKO_NEGATIVE); }

expression(E) ::= FLOATNUM(F).
    { E = ekSyntaxCreateKFloat(C->Y, &F, 0); }

expression(E) ::= NEGATIVE FLOATNUM(F).
    { E = ekSyntaxCreateKFloat(C->Y, &F, CKO_NEGATIVE); }

expression(E) ::= LITERALSTRING(L).
    { E = ekSyntaxCreateKString(C->Y, &L); }

expression(E) ::= NULL(N).
    { E = ekSyntaxCreateNull(C->Y, N.line); }

expression(E) ::= FUNCTION(F) LEFTPAREN func_args(ARGS) RIGHTPAREN statement_block(BODY).
    { E = ekSyntaxCreateFunctionDecl(C->Y, NULL, ARGS, BODY, F.line); }

expression(E) ::= expression(COND) QUESTIONMARK expression(IFBODY) COLON expression(ELSEBODY).
    { E = ekSyntaxCreateIfElse(C->Y, COND, IFBODY, ELSEBODY, yTrue); }


// ---------------------------------------------------------------------------
// LValue

%type lvalue
    { ekSyntax* }

%destructor lvalue
    { ekSyntaxDestroy(C->Y, $$); }

lvalue(L) ::= lvalue_indexable(I).
    { L = I; }

lvalue(L) ::= VAR IDENTIFIER(I).
    { L = ekSyntaxMarkVar(C->Y, ekSyntaxCreateList(C->Y, YST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->Y, &I))); }

lvalue(L) ::= VAR GROUPLEFTPAREN ident_list(I) RIGHTPAREN.
    { L = ekSyntaxMarkVar(C->Y, I); }

lvalue(L) ::= GROUPLEFTPAREN expr_list(EL) RIGHTPAREN.
    { L = EL; }

// ---------------------------------------------------------------------------
// LValueIndexable

%type lvalue_indexable
    { ekSyntax* }

%destructor lvalue_indexable
    { ekSyntaxDestroy(C->Y, $$); }

lvalue_indexable(L) ::= THIS(T).
    { L = ekSyntaxCreateThis(C->Y, T.line); }

lvalue_indexable(L) ::= lvalue_indexable(FUNC) paren_expr_list(ARGS).
    { L = ekSyntaxCreateCall(C->Y, FUNC, ARGS); }

lvalue_indexable(L) ::= lvalue_indexable(ARRAY) OPENBRACKET expression(INDEX) CLOSEBRACKET.
    { L = ekSyntaxCreateIndex(C->Y, ARRAY, INDEX, yFalse); }

lvalue_indexable(L) ::= lvalue_indexable(OBJECT) PERIOD IDENTIFIER(MEMBER).
    { L = ekSyntaxCreateIndex(C->Y, OBJECT, ekSyntaxCreateKString(C->Y, &MEMBER), yFalse); }

lvalue_indexable(L) ::= lvalue_indexable(OBJECT) COLONCOLON IDENTIFIER(MEMBER).
    { L = ekSyntaxCreateIndex(C->Y, OBJECT, ekSyntaxCreateKString(C->Y, &MEMBER), yTrue); }

lvalue_indexable(L) ::= IDENTIFIER(I).
    { L = ekSyntaxCreateIdentifier(C->Y, &I); }

// ---------------------------------------------------------------------------
// Identifier List

%type ident_list
    { ekSyntax* }

%destructor ident_list
    { ekSyntaxDestroy(C->Y, $$); }

ident_list(IL) ::= ident_list(OL) COMMA IDENTIFIER(I).
    { IL = ekSyntaxListAppend(C->Y, OL, ekSyntaxCreateIdentifier(C->Y, &I), 0); }

ident_list(IL) ::= IDENTIFIER(I).
    { IL = ekSyntaxCreateList(C->Y, YST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->Y, &I)); }

ident_list(IL) ::= .
    { IL = ekSyntaxCreateList(C->Y, YST_IDENTIFIERLIST, NULL); }

// ---------------------------------------------------------------------------
// Function Arguments

%type func_args
    { ekSyntax* }

func_args(ARGS) ::= ident_list(IL).
    { ARGS = ekSyntaxCreateFunctionArgs(C->Y, IL, NULL); }

func_args(ARGS) ::= ident_list(IL) COMMA ELLIPSIS IDENTIFIER(VARARGS).
    { ARGS = ekSyntaxCreateFunctionArgs(C->Y, IL, &VARARGS); }

func_args(ARGS) ::= ELLIPSIS IDENTIFIER(VARARGS).
    { ARGS = ekSyntaxCreateFunctionArgs(C->Y, ekSyntaxCreateList(C->Y, YST_IDENTIFIERLIST, NULL), &VARARGS); }
