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

struct ekLexer;
struct ekSyntax;
struct ekToken;

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
    struct ekError ** errors;
    struct ekLexer * lexer;  // only valid during ekCompile()
    const char * sourcePath; // only valid during ekCompile()
    const char * source;     // only valid during ekCompile()
} ekParser;

ekParser * ekParserCreate(struct ekContext * E);
void ekParserDestroy(struct ekContext * E, struct ekParser * parser);

struct ekSyntax * ekParserParse(struct ekContext * E, struct ekParser * parser, const char * sourcePath, const char * source);

void ekParserSyntaxError(struct ekContext * E, ekParser * parser, struct ekToken * token, const char * explanation);
// void ekParserExplainError(struct ekContext * E, ekParser * parser, const char * explanation);

#endif // ifndef EKPARSER_H
