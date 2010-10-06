
%token_prefix YTT_

%token_type {yapToken}
%default_type {yapToken}

%name yapParse

%include {
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
} // end %include

%extra_argument {yapCompiler *compiler}

%left UNKNOWN.
%left COMMENT.
%left SPACE.
%left EOF.

%syntax_error {
    if(TOKEN.text)
    {
        char temp[32];
        int len = strlen(TOKEN.text);
        if(len > 31) len = 31;
        memcpy(temp, TOKEN.text, len);
        temp[len] = 0;
        yapTrace(( "syntax error near '%s'\n", temp ));
    }
    compiler->error = yTrue;
}

module ::= statement_list(L).
    {
        yOperand index;
        yapCodeAppendRet(L, 0);
        index = yapBlockConvertCode(L, compiler->module, 0);
        compiler->module->block = compiler->module->blocks.data[index];
    }

%type statement_list {yapCode*}
%destructor statement_list { yapCodeDestroy($$); }

statement_list(L) ::= statement_list(OL) statement(S).
    {
        L = OL;
        yapCodeAppendCode(L, S);
        
        yapCodeDestroy(S);
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
        yapCodeAppendExpression(compiler, S, E, 1);
        yapCodeAppendSetVar(S);
        
        yapExpressionDestroy(E);
    }

statement(S) ::= VAR IDENTIFIER(I) EQUALS expression(E) NEWLINE.
    {
        S = yapCodeCreate();
        yapCodeAppendVar(compiler, S, &I, yFalse);
        yapCodeAppendExpression(compiler, S, E, 1);
        yapCodeAppendSetVar(S);
        
        yapExpressionDestroy(E);
    }

statement(S) ::= VAR IDENTIFIER(I) NEWLINE.
    {
        S = yapCodeCreate();
        yapCodeAppendVar(compiler, S, &I, yTrue);
    }

statement(S) ::= RETURN expr_list(EL) NEWLINE.
    {
        int i;
        S = yapCodeCreate();
        for(i=0; i<EL->count; i++)
        {
            yapCodeAppendExpression(compiler, S, (yapExpression*)EL->data[i], 1);
        }
        yapCodeGrow(S, 1);
        yapCodeAppend(S, YOP_RET, EL->count);
        
        yapArrayClear(EL, yapExpressionDestroy);
        yapFree(EL);
    }

statement(S) ::= expr_list(L) NEWLINE.
    {
        int i;
        S = yapCodeCreate();
        for(i=0; i<L->count; i++)
        {
            yapCodeAppendExpression(compiler, S, (yapExpression*)L->data[i], 0);
        }
        
        yapArrayClear(L, yapExpressionDestroy);
        yapFree(L);
    }

statement(S) ::= FUNCTION IDENTIFIER(I) LEFTPAREN ident_list(ARGS) RIGHTPAREN NEWLINE INDENT statement_list(B) DEDENT.
    {
        yOperand index;

        if(ARGS->count)
        {
            int i;
            yapCode *code = yapCodeCreate();
            for(i=ARGS->count-1; i>=0; i--)
            {
                yapExpression *arg = (yapExpression*)ARGS->data[i];
                yapCodeAppendNamedArg(compiler, code, arg);
            }
            yapCodeAppendCode(code, B);
            yapCodeDestroy(B);
            B = code;
        }

        yapCodeAppendRet(B, 0);
        index = yapBlockConvertCode(B, compiler->module, ARGS->count);
        
        yapArrayClear(ARGS, yapExpressionDestroy);
        yapFree(ARGS);

        S = yapCodeCreate();
        yapCodeAppendVar(compiler, S, &I, yFalse);
        yapCodeGrow(S, 1);
        yapCodeAppend(S, YOP_PUSHLBLOCK, index);
        yapCodeAppendSetVar(S);
        yapTrace(("function created. block %d\n", index));
    }

statement(S) ::= NEWLINE.
    {
        S = yapCodeCreate();
    }

%type expr_list {yapArray*}
%destructor expr_list { yapArrayDestroy($$, yapExpressionDestroy); }

expr_list(EL) ::= LEFTPAREN expr_list(OL) RIGHTPAREN.
    {
        EL = OL;
    }

expr_list(EL) ::= expr_list(OL) COMMA expression(E).
    {
        EL = OL;
        yapArrayPush(EL, E);
    }

expr_list(EL) ::= expression(E).
    {
        EL = (yapArray*)yapAlloc(sizeof(yapArray));
        yapArrayPush(EL, E);
    }

expr_list(EL) ::= .
    {
        EL = (yapArray*)yapAlloc(sizeof(yapArray));
    }

%type expression {yapExpression*}
%destructor expression { yapExpressionDestroy($$); }

expression(E) ::= IDENTIFIER(F) LEFTPAREN expr_list(ARGS) RIGHTPAREN.
    {
        E = yapExpressionCreateCall(&F, ARGS);
    }

expression(E) ::= LITERALSTRING(L).
    {
        E = yapExpressionCreateLiteralString(&L);
    }

expression(E) ::= IDENTIFIER(I).
    {
        E = yapExpressionCreateIdentifier(&I);
    }

expression(E) ::= NULL.
    {
        E = yapExpressionCreateNull();
    }

%type ident_list {yapArray*}
%destructor ident_list { yapArrayClear($$, yapExpressionDestroy); yapFree($$); }

ident_list(IL) ::= ident_list(OL) COMMA IDENTIFIER(I).
    {
        IL = OL;
        yapArrayPush(IL, yapExpressionCreateIdentifier(&I));
    }

ident_list(IL) ::= IDENTIFIER(I).
    {
        IL = (yapArray*)yapAlloc(sizeof(yapArray));
        yapArrayPush(IL, yapExpressionCreateIdentifier(&I));
    }

ident_list(IL) ::= .
    {
        IL = (yapArray*)yapAlloc(sizeof(yapArray));
    }
