
%token_prefix YTT_

%token_type {yapToken}
%default_type {yapToken}

%name yapParse

%include {
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

// %type identlist {yapArray*}

module ::= statement_list.

statement_list ::= statement_list statement.
statement_list ::= statement.

%type statement {yapNugget*}

statement ::= IDENTIFIER EQUALS expression NEWLINE.
statement(S) ::= VAR IDENTIFIER EQUALS expression NEWLINE.
	{
		// yopExpressionCompile()  -- generates ops into a yapNugget	

		S = (yapNugget*)yapAlloc(sizeof(yapNugget));
		yapNuggetGrowOps(S, 2);
		yapNuggetAppendOp(S, YOP_VARREG_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, I.text, I.len));
		yapNuggetAppendOp(S, YOP_POP, 1); // nobody wants it yet
	}

statement(S) ::= VAR IDENTIFIER(I) NEWLINE.
	{
		S = (yapNugget*)yapAlloc(sizeof(yapNugget));
		yapNuggetGrowOps(S, 2);
		yapNuggetAppendOp(S, YOP_VARREG_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, I.text, I.len));
		yapNuggetAppendOp(S, YOP_POP, 1); // nobody wants it yet
	}
	
statement ::= expression NEWLINE.
statement ::= NEWLINE.

%type expression {yapExpression*}

expression(E) ::= LITERALSTRING(S).
	{
		E = yapExpressionCreate();
		E->text = yapTokenToString(&S);

		// E = (yapNugget*)yapAlloc(sizeof(yapNugget));
		// E->ops = yapOpsAlloc(1);
		// E->ops[0].opcode = YOP_PUSH_KS;
		// E->ops[0].operand = yapArrayPushUniqueStringLen(&compiler->module->kStrings, S.text, S.len);
	}

expression(E) ::= IDENTIFIER(I).
	{
		E = yapExpressionCreate();
		E->text = yapTokenToString(&I);

		// E = (yapNugget*)yapAlloc(sizeof(yapNugget));
		// E->ops = yapOpsAlloc(2);
		// E->ops[0].opcode = YOP_VARREF_KS;
		// E->ops[0].operand = yapArrayPushUniqueStringLen(&compiler->module->kStrings, I.text, I.len);
		// E->ops[1].opcode = YOP_REFVAL;
		// E->ops[1].operand = 0;
	}
