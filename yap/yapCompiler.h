#ifndef YAPPARSER_H
#define YAPPARSER_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCode;
struct yapModule;
struct yapSyntax;
struct yapToken;

// ---------------------------------------------------------------------------

typedef struct yapCompiler
{
    struct yapSyntax *root;
    struct yapModule *module;
    struct yapCode *code;
    yapArray errors;
} yapCompiler;

#define yapCompilerCreate() ((yapCompiler*)yapAlloc(sizeof(yapCompiler)))
void yapCompilerDestroy(yapCompiler *compiler);

// Main entry point for the compiler
yBool yapCompile(yapCompiler *compiler, const char *text);

void yapCompileError(yapCompiler *compiler, const char *error);
void yapCompileSyntaxError(yapCompiler *compiler, struct yapToken *token);

#endif
