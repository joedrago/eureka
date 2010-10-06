#ifndef YAPCODE_H
#define YAPCODE_H

#include "yapTypes.h"

// for yapToken
#include "yapLexer.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCompiler;

// ---------------------------------------------------------------------------

enum
{
    YEP_NULL = 0,

    YEP_IDENTIFIER,
    YEP_LITERALSTRING,
    YEP_CALL,

    YEP_COUNT
};

typedef struct yapExpression
{
    yapToken token;
    yU32 type;
    yapArray *args;                    // for YEP_CALL
} yapExpression;

#define yapExpressionCreate() ((yapExpression*)yapAlloc(sizeof(yapExpression)))
yapExpression * yapExpressionCreateLiteralString(struct yapToken *token);
yapExpression * yapExpressionCreateIdentifier(struct yapToken *token);
yapExpression * yapExpressionCreateNull();
yapExpression * yapExpressionCreateCall(struct yapToken *token, yapArray *args);

void yapExpressionDestroy(yapExpression *expr);

// ---------------------------------------------------------------------------

typedef struct yapCode
{
    struct yapOp *ops;
    int size;
    int count;
} yapCode;

#define yapCodeCreate() ((yapCode*)yapAlloc(sizeof(yapCode)))
void yapCodeDestroy(yapCode *code);
void yapCodeGrow(yapCode *code, int count);
void yapCodeAppend(yapCode *code, yOpcode opcode, yOperand operand);

void yapCodeAppendExpression(struct yapCompiler *compiler, yapCode *code, yapExpression *expr, int keepCount);
void yapCodeAppendNamedArg(struct yapCompiler *compiler, yapCode *code, yapExpression *name);
void yapCodeAppendVar(struct yapCompiler *compiler, yapCode *code, struct yapToken *token, yBool popRef);
void yapCodeAppendVarRef(struct yapCompiler *compiler, yapCode *code, struct yapToken *token);
void yapCodeAppendSetVar(yapCode *code);
void yapCodeAppendCode(yapCode *dst, yapCode *src);
void yapCodeAppendRet(yapCode *code, int argcount);
// ---------------------------------------------------------------------------

#endif

