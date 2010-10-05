
%token_prefix YTT_

%token_type {yapToken}
%default_type {yapToken}

%name yapParse

%include {
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
} // end %include

%extra_argument {yapCompiler *compiler}

%left UNKNOWN.
%left COMMENT.
%left IDENTIFIER.
%left SPACE.
%left NEWLINE.
%left LITERALSTRING.
%left LEFTPAREN.
%left RIGHTPAREN.
%left WHILE.
%left EOF.

%syntax_error {
	if(TOKEN.text)
	{
	    char temp[32];
	    int len = strlen(TOKEN.text);
	    if(len > 31) len = 31;
	    memcpy(temp, TOKEN.text, len);
	    temp[len] = 0;
		printf( "syntax error near '%s'\n", temp );
	}
	compiler->error = yTrue;
}

%type module {yapCode*}

module(M) ::= statement_list(L).
    {
        M = L;
        yapModuleDump(compiler->module);
        yapOpsDump(M->ops, M->count);
    }

%type statement_list {yapCode*}

statement_list(L) ::= statement_list(OL) statement(S).
    {
        L = OL;
        yapCodeAppendCode(L, S);
    }

statement_list(L) ::= statement(S).
    {
        L = S;
    }

%type statement {yapCode*}

statement(S) ::= IDENTIFIER(I) EQUALS expression(E) NEWLINE.
    {
        S = yapCodeCreate();
        yapCodeAppendVarRef(compiler, S, &I);
        yapCodeAppendExpression(compiler, S, E);
        yapCodeAppendSetVar(S);
    }

statement(S) ::= VAR IDENTIFIER(I) EQUALS expression(E) NEWLINE.
    {
        S = yapCodeCreate();
        yapCodeAppendVar(compiler, S, &I, yFalse);
        yapCodeAppendExpression(compiler, S, E);
        yapCodeAppendSetVar(S);
    }

statement(S) ::= VAR IDENTIFIER(I) NEWLINE.
    {
        S = yapCodeCreate();
        yapCodeAppendVar(compiler, S, &I, yTrue);
    }

statement(S) ::= expression(E) NEWLINE.
    {
        S = yapCodeCreate();
        yapCodeAppendExpression(compiler, S, E); // calling a function that returns void?

        // TODO: probably need to add something here that cleans out the value stack, or
        //       pass AppendExpression something that indicates a special CALL with "leave
        //       nothing on the stack" is added
    }

statement(S) ::= NEWLINE.
    {
        S = yapCodeCreate();
    }

%type expression {yapExpression*}

expression(E) ::= LITERALSTRING(L).
    {
        E = yapExpressionCreateLiteralString(&L);
    }

expression(E) ::= IDENTIFIER(I).
    {
        E = yapExpressionCreateIdentifier(&I);
    }

