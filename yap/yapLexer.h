#ifndef YAPLEXER_H
#define YAPLEXER_H

#include "yapTokenizer.h"
#include "yapTypes.h"

typedef int yToken;

//typedef enum yapTokenType
//{
//    YTT_UNKNOWN = 0,
//
//    YTT_COMMENT,
//    YTT_IDENTIFIER,
//    YTT_SPACE,
//    YTT_NEWLINE,
//    YTT_LITERALSTRING, // 5
//
//    YTT_LEFTPAREN,
//    YTT_RIGHTPAREN,
//
//    YTT_WHILE,
//
//    YTT_EOF,
//    YTT_COUNT
//
//} yapTokenType;

typedef void (*tokenCB)(void *parser, yToken token, const char *text);
yBool yapLex(void *parser, const char *text, tokenCB cb);

#endif
