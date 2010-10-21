#ifndef YAPPARSER_H
#define YAPPARSER_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCode;
struct yapModule;
struct yapSyntaxTree;
struct yapToken;

// ---------------------------------------------------------------------------

typedef struct yapCompiler
{
    struct yapSyntaxTree *tree;
    struct yapModule *module;
    struct yapCode *code;
    yapArray errors;
} yapCompiler;

#define yapCompilerCreate() ((yapCompiler*)yapAlloc(sizeof(yapCompiler)))
void yapCompilerDestroy(yapCompiler *compiler);

// Main entry point for the compiler
yBool yapCompile(yapCompiler *compiler, const char *text);

void yapCompileError(yapCompiler *compiler, const char *error);

#endif
