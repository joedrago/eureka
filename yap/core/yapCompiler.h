// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef YAPPARSER_H
#define YAPPARSER_H

#include "yapArray.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCode;
struct yapChunk;
struct yapSyntax;
struct yapToken;

// ---------------------------------------------------------------------------

// Yap Compile Options
enum
{
    YCO_DEFAULT = 0,

    YCO_OPTIMIZE         = (1 << 0),
    YCO_KEEP_SYNTAX_TREE = (1 << 1)
};

typedef struct yapCompiler
{
    struct yapSyntax *root;
    struct yapChunk *chunk;
    struct yapCode *code;
    yapArray errors;
} yapCompiler;

#define yapCompilerCreate() ((yapCompiler*)yapAlloc(sizeof(yapCompiler)))
void yapCompilerDestroy(yapCompiler *compiler);

// Main entry point for the compiler
yBool yapCompile(yapCompiler *compiler, const char *text, yU32 compileOpts);

void yapCompileError(yapCompiler *compiler, const char *error);
void yapCompileSyntaxError(yapCompiler *compiler, struct yapToken *token);

#endif
