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

typedef struct ekCompiler
{
    struct ekContext * E;
    struct ekSyntax * root;
    struct ekChunk * chunk;
    struct ekCode * code;
    ekError ** errors;
    const char * sourcePath; // only valid during ekCompile()
    const char * source;     // only valid during ekCompile()
} ekCompiler;

ekCompiler * ekCompilerCreate(struct ekContext * E);
void ekCompilerDestroy(ekCompiler * compiler);

// Main entry point for the compiler
ekBool ekCompile(ekCompiler * compiler, const char * sourcePath, const char * source, ekU32 compileOpts);

void ekCompileSyntaxError(ekCompiler * compiler, struct ekToken * token, const char * explanation);
void ekCompileExplainError(ekCompiler * compiler, const char * explanation);
ekBool ekCompilerFormatErrors(ekCompiler * compiler, ekString * output);

#endif // ifndef EKCOMPILER_H
