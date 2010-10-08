#ifndef YAPPARSER_H
#define YAPPARSER_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCode;
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

struct yapCode * yapCompileExpressionListCreate(yapCompiler *compiler, struct yapCode *firstExpression);
struct yapCode * yapCompileExpressionListAppend(yapCompiler *compiler, struct yapCode *list, struct yapCode *expression);

struct yapCode * yapCompileStatementFunctionDecl(yapCompiler *compiler, struct yapToken *name, yapArray *args, struct yapCode *body);
struct yapCode * yapCompileStatementExpressionList(yapCompiler *compiler, struct yapCode *list);
struct yapCode * yapCompileStatementReturn(yapCompiler *compiler, struct yapCode *list);
struct yapCode * yapCompileStatementVar(yapCompiler *compiler, struct yapToken *name);
struct yapCode * yapCompileStatementVarInit(yapCompiler *compiler, struct yapToken *name, struct yapCode *initialValue);
struct yapCode * yapCompileStatementAssignment(yapCompiler *compiler, struct yapToken *name, struct yapCode *value);
struct yapCode * yapCompileStatementListAppend(yapCompiler *compiler, struct yapCode *list, struct yapCode *statement);
struct yapCode * yapCompileStatementIfElse(yapCompiler *compiler, struct yapCode *cond, struct yapCode *ifBody, struct yapCode *elseBody);
struct yapCode * yapCompileStatementLoop(yapCompiler *compiler, struct yapCode *init, struct yapCode *cond, struct yapCode *incr, struct yapCode *body);
void yapCompileModuleStatementList(yapCompiler *compiler, struct yapCode *list);

#endif
