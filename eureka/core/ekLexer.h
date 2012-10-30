// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EUREKALEXER_H
#define EUREKALEXER_H

#include "ekTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekCompiler;
struct ekSyntaxTree;

// ---------------------------------------------------------------------------

typedef struct ekToken
{
    const char *text;
    int len;
    int line;
} ekToken;

#define ekTokenCreate() ((ekToken*)ekAlloc(sizeof(ekToken)))
#define ekTokenDestroy ekFree

ekToken *ekTokenClone(struct ekContext *Y, ekToken *token);

char *ekTokenToString(struct ekContext *Y, ekToken *t); // ekAlloc's a string copy
int ekTokenToInt(struct ekContext *Y, ekToken *t);
float ekTokenToFloat(struct ekContext *Y, ekToken *t);

typedef void (*tokenCB)(void *parser, int id, ekToken token, struct ekCompiler *compiler);
yBool ekLex(void *parser, const char *text, tokenCB cb, struct ekCompiler *compiler);

#endif
