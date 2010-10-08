#ifndef YAPPARSER_H
#define YAPPARSER_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCode;
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

yapArray * yapCompileExpressionListCreate(yapCompiler *compiler, struct yapCode *firstExpression);
yapArray * yapCompileExpressionListAppend(yapCompiler *compiler, yapArray *list, struct yapCode *expression);

struct yapCode * yapCompileCombine(yapCompiler *compiler, yOpcode op, struct yapCode *code, struct yapCode *expr);

struct yapCode * yapCompileStatementFunctionDecl(yapCompiler *compiler, struct yapToken *name, yapArray *args, struct yapCode *body);
struct yapCode * yapCompileStatementExpressionList(yapCompiler *compiler, yapArray *list);
struct yapCode * yapCompileStatementReturn(yapCompiler *compiler, yapArray *list);
struct yapCode * yapCompileStatementVar(yapCompiler *compiler, struct yapToken *name);
struct yapCode * yapCompileStatementVarInit(yapCompiler *compiler, struct yapToken *name, struct yapCode *initialValue);
struct yapCode * yapCompileStatementAssignment(yapCompiler *compiler, struct yapToken *name, struct yapCode *value);
struct yapCode * yapCompileStatementListAppend(yapCompiler *compiler, struct yapCode *list, struct yapCode *statement);
struct yapCode * yapCompileStatementIfElse(yapCompiler *compiler, struct yapArray *cond, struct yapCode *ifBody, struct yapCode *elseBody);
struct yapCode * yapCompileStatementLoop(yapCompiler *compiler, struct yapArray *init, struct yapArray *cond, struct yapArray *incr, struct yapCode *body);
void yapCompileModuleStatementList(yapCompiler *compiler, struct yapCode *list);

#endif
