
%token_prefix YTT_

%token_type {char*}
%default_type {char*}

%name yapTokenize

%include {
#include "yapParser.h"
#define assert(X)
} // end %include

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

input(A) ::= identlist.                       { char *wut = A; printf("identlist1: %s\n", wut); }
identlist(A) ::= identlist SPACE IDENTIFIER.  { char *wut = A; printf("identlist2: %s\n", wut); }
identlist(A) ::= IDENTIFIER.                  { char *wut = A; printf("identlist3: %s\n", wut); }
