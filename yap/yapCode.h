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
    YEP_UNKNOWN = 0,

    YEP_IDENTIFIER,
    YEP_LITERALSTRING,

    YEP_COUNT
};

typedef struct yapExpression
{
    yapToken token;
    yU32 type;
} yapExpression;

#define yapExpressionCreate() ((yapExpression*)yapAlloc(sizeof(yapExpression)))
yapExpression * yapExpressionCreateLiteralString(struct yapToken *token);
yapExpression * yapExpressionCreateIdentifier(struct yapToken *token);

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

void yapCodeAppendExpression(struct yapCompiler *compiler, yapCode *code, yapExpression *expr);
void yapCodeAppendVar(struct yapCompiler *compiler, yapCode *code, struct yapToken *token, yBool popRef);
void yapCodeAppendVarRef(struct yapCompiler *compiler, yapCode *code, struct yapToken *token);
void yapCodeAppendSetVar(yapCode *code);
void yapCodeAppendCode(yapCode *dst, yapCode *src);
// ---------------------------------------------------------------------------

#endif

