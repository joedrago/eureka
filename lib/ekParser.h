// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKPARSER_H
#define EKPARSER_H

#include "ekTypes.h"
#include "ekString.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekSyntax;

// ---------------------------------------------------------------------------

typedef enum ekTokenType
{
    ETT_UNKNOWN = 0,

    ETT_AND,
    ETT_ARRAYOPENBRACKET,
    ETT_ASSIGN,
    ETT_BITWISE_AND,
    ETT_BITWISE_ANDEQUALS,
    ETT_BITWISE_NOT,
    ETT_BITWISE_OR,
    ETT_BITWISE_OREQUALS,
    ETT_BITWISE_XOR,
    ETT_BITWISE_XOREQUALS,
    ETT_BREAK,
    ETT_CLOSEBRACE,
    ETT_CLOSEBRACKET,
    ETT_CMP,
    ETT_COLON,
    ETT_COLONCOLON,
    ETT_COMMA,
    ETT_COMMENT,
    ETT_DASH,
    ETT_DASHEQUALS,
    ETT_ELLIPSIS,
    ETT_ELSE,
    ETT_ENDBLOCK,
    ETT_ENDSTATEMENT,
    ETT_EOF,
    ETT_EQUALS,
    ETT_FALSE,
    ETT_FATCOMMA,
    ETT_FLOATNUM,
    ETT_FOR,
    ETT_FUNCTION,
    ETT_GREATERTHAN,
    ETT_GREATERTHANOREQUAL,
    ETT_GROUPLEFTPAREN,
    ETT_HEREDOC,
    ETT_IDENTIFIER,
    ETT_IF,
    ETT_IN,
    ETT_INHERITS,
    ETT_INTEGER,
    ETT_LEFTPAREN,
    ETT_LESSTHAN,
    ETT_LESSTHANOREQUAL,
    ETT_LITERALSTRING,
    ETT_MAPSTARTBLOCK,
    ETT_MOD,
    ETT_NEGATIVE,
    ETT_NEWLINE,
    ETT_NOT,
    ETT_NOTEQUALS,
    ETT_NULL,
    ETT_OPENBRACE,
    ETT_OPENBRACKET,
    ETT_OR,
    ETT_PERIOD,
    ETT_PLUS,
    ETT_PLUSEQUALS,
    ETT_QUESTIONMARK,
    ETT_REGEXSTRING,
    ETT_RETURN,
    ETT_RIGHTPAREN,
    ETT_SCOPESTARTBLOCK,
    ETT_SEMI,
    ETT_SHIFTLEFT,
    ETT_SHIFTLEFTEQUALS,
    ETT_SHIFTRIGHT,
    ETT_SHIFTRIGHTEQUALS,
    ETT_SLASH,
    ETT_SLASHEQUALS,
    ETT_SPACE,
    ETT_STAR,
    ETT_STAREQUALS,
    ETT_STARTBLOCK,
    ETT_THIS,
    ETT_TRUE,
    ETT_VAR,
    ETT_WHILE,

    ETT_COUNT
} ekTokenType;

// ---------------------------------------------------------------------------

typedef struct ekError
{
    ekString line;
    ekString explanation;
    ekString filename;
    ekS32 lineNo;
    ekS32 col;
} ekError;

ekError * ekErrorCreate(struct ekContext * E, const char * filename, ekS32 line, const char * source, const char * loc, const char * explanation);
void ekErrorDestroy(struct ekContext * E, ekError * error);

typedef struct ekParser
{
    const char * sourcePath; // only valid during ekCompile()
    const char * source;     // only valid during ekCompile()
    ekError ** errors;
} ekParser;

ekParser * ekParserCreate(struct ekContext * E);
void ekParserDestroy(struct ekContext * E, struct ekParser * parser);

struct ekSyntax * ekParserParse(struct ekContext * E, struct ekParser * parser, const char * sourcePath, const char * source);

void ekParserSyntaxError(struct ekContext * E, ekParser * parser, struct ekToken * token, const char * explanation);
// void ekParserExplainError(struct ekContext * E, ekParser * parser, const char * explanation);

#endif // ifndef EKPARSER_H
