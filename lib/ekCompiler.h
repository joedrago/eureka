// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef EKCOMPILER_H
#define EKCOMPILER_H

#include "ekTypes.h"
#include "ekString.h"

// ---------------------------------------------------------------------------
// Forwards

struct ekCode;
struct ekChunk;
struct ekParser;
struct ekSyntax;
struct ekToken;

// ---------------------------------------------------------------------------

// Eureka Compile Options
enum
{
    ECO_DEFAULT = 0,

    ECO_OPTIMIZE         = (1 << 0),
    ECO_KEEP_SYNTAX_TREE = (1 << 1)
};

typedef struct ekCompiler
{
    struct ekParser * parser;
    struct ekSyntax * root;
    struct ekChunk * chunk;
    struct ekCode * code;
    const char * sourcePath; // only valid during ekCompile()
} ekCompiler;

ekCompiler * ekCompilerCreate(struct ekContext * E);
void ekCompilerDestroy(struct ekContext * E, ekCompiler * compiler);

// Main entry point for the compiler
ekBool ekCompile(struct ekContext * E, ekCompiler * compiler, const char * sourcePath, const char * source, ekU32 compileOpts);

ekBool ekCompilerFormatErrors(struct ekContext * E, ekCompiler * compiler, ekString * output);

#endif // ifndef EKCOMPILER_H
