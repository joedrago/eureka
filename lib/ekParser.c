// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekParser.h"

#include "ekCompiler.h"
#include "ekContext.h"
#include "ekLexer.h"
#include "ekString.h"
#include "ekSyntax.h"

ekError * ekErrorCreate(struct ekContext * E, const char * filename, ekS32 lineNo, const char * source, const char * loc, const char * explanation)
{
    ekError * error = (ekError *)ekAlloc(sizeof(ekError));
    ekStringSet(E, &error->filename, filename);
    error->lineNo = lineNo;
    ekStringSet(E, &error->explanation, explanation);
    if (loc) {
        // Find the full line where things went wrong, and store the location in it
        const char * line = loc;
        const char * end;
        while ((line != source) && (*line != '\n') && (*line != '\r')) {
            --line;
        }
        if ((*line == '\n') || (*line == '\r')) {
            ++line;
        }
        end = line;
        while (*end && (*end != '\n') && (*end != '\r')) {
            ++end;
        }
        ekStringSetLen(E, &error->line, line, end - line);
        error->col = loc - line;
    }
    return error;
}

void ekErrorDestroy(ekContext * E, ekError * error)
{
    ekStringClear(E, &error->line);
    ekStringClear(E, &error->explanation);
    ekStringClear(E, &error->filename);
    ekFree(error);
}

void ekParserSyntaxError(struct ekContext * E, ekParser * parser, struct ekToken * token, const char * explanation)
{
    const char * sourcePath = parser->sourcePath;
    if (!sourcePath)
        sourcePath = "<source>";
    ekError * error = ekErrorCreate(E, sourcePath, token->line, parser->source, token->text, explanation);
    ekArrayPush(E, &parser->errors, error);
}

#if 0
void ekParserExplainError(struct ekContext * E, ekParser * parser, const char * explanation)
{
    ekS32 size = ekArraySize(E, &parser->errors);
    if (size > 0) {
        ekError * error = parser->errors[size - 1];
        ekStringSet(E, &error->explanation, explanation);
    }
}
#endif

ekParser * ekParserCreate(struct ekContext * E)
{
    ekParser * parser = (ekParser *)ekAlloc(sizeof(ekParser));
    return parser;
}

void ekParserDestroy(struct ekContext * E, ekParser * parser)
{
    ekArrayDestroy(E, &parser->errors, (ekDestroyCB)ekErrorDestroy);
    ekFree(parser);
}

struct ekSyntax * ekParserParse(struct ekContext * E, struct ekParser * parser, const char * sourcePath, const char * source)
{
    struct ekSyntax * root;
    parser->sourcePath = sourcePath;
    parser->source = source;
    parser->lexer = ekLexerCreate(E);

    root = ekSyntaxCreate(E, EST_STATEMENTLIST, 1); // Lies!

    ekLexerDestroy(E, parser->lexer);
    parser->lexer = NULL;
    parser->sourcePath = NULL;
    parser->source = NULL;
    return root;
}
