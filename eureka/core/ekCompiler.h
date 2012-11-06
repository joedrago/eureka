// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EUREKAPARSER_H
#define EUREKAPARSER_H

#include "ekTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekCode;
struct ekChunk;
struct ekSyntax;
struct ekToken;

// ---------------------------------------------------------------------------

// Eureka Compile Options
enum
{
    YCO_DEFAULT = 0,

    YCO_OPTIMIZE         = (1 << 0),
    YCO_KEEP_SYNTAX_TREE = (1 << 1)
};

typedef struct ekCompiler
{
    struct ekContext *E;
    struct ekSyntax *root;
    struct ekChunk *chunk;
    struct ekCode *code;
    char **errors;
} ekCompiler;

ekCompiler *ekCompilerCreate(struct ekContext *E);
void ekCompilerDestroy(ekCompiler *compiler);

// Main entry point for the compiler
ekBool ekCompile(ekCompiler *compiler, const char *text, ekU32 compileOpts);

void ekCompileError(ekCompiler *compiler, const char *error);
void ekCompileSyntaxError(ekCompiler *compiler, struct ekToken *token);

#endif
