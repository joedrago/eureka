#ifndef YAPLEXER_H
#define YAPLEXER_H

#include "yapTypes.h"

#include "yapParser.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapVM;
struct yapCompiler;

// ---------------------------------------------------------------------------

typedef struct yapToken
{
    const char *text;
    int len;
} yapToken;

char *yapTokenToString(yapToken *t); // yapAlloc's a string copy

typedef void (*tokenCB)(void *parser, int id, yapToken token, struct yapCompiler *compiler);
yBool yapLex(void *parser, const char *text, tokenCB cb, struct yapCompiler *compiler);

#endif
