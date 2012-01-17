// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPLEXER_H
#define YAPLEXER_H

#include "yapTypes.h"

// #include "yapParser.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCompiler;
struct yapSyntaxTree;
struct yapVM;

// ---------------------------------------------------------------------------

typedef struct yapToken
{
    const char *text;
    int len;
    int line;
} yapToken;

#define yapTokenCreate() ((yapToken*)yapAlloc(sizeof(yapToken)))
#define yapTokenDestroy yapFree

yapToken *yapTokenClone(yapToken *token);

char *yapTokenToString(yapToken *t); // yapAlloc's a string copy
int yapTokenToInt(yapToken *t);
float yapTokenToFloat(yapToken *t);

typedef void (*tokenCB)(void *parser, int id, yapToken token, struct yapCompiler *compiler);
yBool yapLex(void *parser, const char *text, tokenCB cb, struct yapCompiler *compiler);

#endif
