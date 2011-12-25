#ifndef YAPPARSER_H
#define YAPPARSER_H

#include "yapTypes.h"

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
