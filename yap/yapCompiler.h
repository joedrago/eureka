#ifndef YAPPARSER_H
#define YAPPARSER_H

#include "yapTypes.h"

typedef struct yapCompiler
{
    struct yapModule *module;
    yBool error;
} yapCompiler;

typedef struct yapNugget
{
    struct yapOp *ops;
    int count;
} yapNugget;

#define yapNuggetCreate() ((yapNugget*)yapAlloc(sizeof(yapNugget)))
void yapNuggetGrowOps(yapNugget *nugget, int count);
void yapNuggetAppendOp(yapNugget *nugget, yOpcode opcode, yOperand operand);

typedef struct yapExpression
{
    const char *text;
} yapExpression;

#define yapExpressionCreate() ((yapExpression*)yapAlloc(sizeof(yapExpression)))

#define yapCompilerCreate() ((yapCompiler*)yapAlloc(sizeof(yapCompiler)))
void yapCompilerDestroy(yapCompiler *compiler);

yBool yapCompile(yapCompiler *compiler, const char *text);

#endif
