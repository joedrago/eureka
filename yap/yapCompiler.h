#ifndef YAPPARSER_H
#define YAPPARSER_H

#include "yapTypes.h"

typedef struct yapCompiler
{
    yapArray *list;
    yBool error;
} yapCompiler;

#define yapCompilerCreate() ((yapCompiler*)yapAlloc(sizeof(yapCompiler)))
void yapCompilerDestroy(yapCompiler *compiler);

yBool yapCompile(yapCompiler *compiler, const char *text);

#endif
