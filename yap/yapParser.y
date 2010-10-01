
%token_prefix YTT_

%token_type {char*}
%default_type {char*}

%name yapTokenize

%include {
#include "yapParser.h"
#define assert(ignoring_this_function)
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

input ::= identlist.                       { printf("MAKE INPUT\n"); }
identlist ::= identlist SPACE IDENTIFIER.  { printf("APPEND IDENTIFIER TO EXISTING LIST\n"); }
identlist ::= IDENTIFIER.                  { printf("MAKE LIST, APPEND IDENTIFIER\n"); }
