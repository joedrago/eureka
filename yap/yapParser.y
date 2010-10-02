
%token_prefix YTT_

%token_type {yapToken}
%default_type {yapToken}

%name yapParse

%include {
#include "yapCompiler.h"
#include "yapParser.h"
#include "yapLexer.h"
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
    char temp[32];
    int len = strlen(TOKEN.text);
    if(len > 31) len = 31;
    memcpy(temp, TOKEN.text, len);
    temp[len] = 0;
  printf( "syntax error '%s'\n", temp );
  compiler->error = yTrue;
}

%type identlist {yapArray*}

file ::= module.

module ::= module NEWLINE.
module ::= identlist(L).
    {
        compiler->list = L;
    }

identlist(L) ::= identlist(PL) SPACE IDENTIFIER(I).
    {
        L = PL;
        yapArrayPush(L, yapTokenToString(&I)); 
    }

identlist(L) ::= IDENTIFIER(I).              
    {
        L = yapAlloc(sizeof(yapArray)); 
        yapArrayPush(L, yapTokenToString(&I)); 
    }

