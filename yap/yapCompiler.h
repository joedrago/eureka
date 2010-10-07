#ifndef YAPPARSER_H
#define YAPPARSER_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCode;
struct yapExpression;
struct yapToken;

// ---------------------------------------------------------------------------

typedef struct yapCompiler
{
    struct yapModule *module;
    yBool error;
} yapCompiler;

#define yapCompilerCreate() ((yapCompiler*)yapAlloc(sizeof(yapCompiler)))
void yapCompilerDestroy(yapCompiler *compiler);

yBool yapCompile(yapCompiler *compiler, const char *text);

void yapCompileSyntaxError(yapCompiler *compiler, const char *token);

yapArray * yapCompileIdentifierListCreate(yapCompiler *compiler, struct yapToken *firstIdentifier);
yapArray * yapCompileIdentifierListAppend(yapCompiler *compiler, yapArray *list, struct yapToken *identifier);

yapArray * yapCompileExpressionListCreate(yapCompiler *compiler, struct yapExpression *firstExpression);
yapArray * yapCompileExpressionListAppend(yapCompiler *compiler, yapArray *list, struct yapExpression *expression);

struct yapCode * yapCompileStatementFunctionDecl(yapCompiler *compiler, struct yapToken *name, yapArray *args, struct yapCode *body);
struct yapCode * yapCompileStatementExpressionList(yapCompiler *compiler, yapArray *list);
struct yapCode * yapCompileStatementReturn(yapCompiler *compiler, yapArray *list);
struct yapCode * yapCompileStatementVar(yapCompiler *compiler, struct yapToken *name);
struct yapCode * yapCompileStatementVarInit(yapCompiler *compiler, struct yapToken *name, struct yapExpression *initialValue);
struct yapCode * yapCompileStatementAssignment(yapCompiler *compiler, struct yapToken *name, struct yapExpression *value);
struct yapCode * yapCompileStatementListAppend(yapCompiler *compiler, struct yapCode *list, struct yapCode *statement);
void yapCompileModuleStatementList(yapCompiler *compiler, struct yapCode *list);

#endif
