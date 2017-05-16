// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKLEXER_H
#define EKLEXER_H

#include "ekTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekCompiler;
struct ekSyntaxTree;

// ---------------------------------------------------------------------------

typedef struct ekToken
{
    const char * text;
    ekS32 len;
    ekS32 line;
} ekToken;

#define ekTokenCreate() ((ekToken *)ekAlloc(sizeof(ekToken)))
#define ekTokenDestroy ekFree

ekToken * ekTokenClone(struct ekContext * E, ekToken * token);

char * ekTokenToString(struct ekContext * E, ekToken * t, int isRegex); // ekAlloc's a string copy
ekS32 ekTokenToInt(struct ekContext * E, ekToken * t);
float ekTokenToFloat(struct ekContext * E, ekToken * t);

typedef void (* tokenCB)(void * parser, ekS32 id, ekToken token, struct ekCompiler * compiler);
ekBool ekLex(void * parser, const char * text, tokenCB cb, struct ekCompiler * compiler);

#endif
