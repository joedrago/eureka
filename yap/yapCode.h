#ifndef YAPCODE_H
#define YAPCODE_H

#include "yapTypes.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCompiler;
struct yapToken;

// ---------------------------------------------------------------------------

typedef struct yapCode
{
    struct yapOp *ops;
    int size;
    int count;
    yBool call;                        // this represents a call expression
    int offer;                         // how many stack values this will leave
} yapCode;

void yapCodeAppendLiteralString(struct yapCompiler *compiler, yapCode *code, struct yapToken *token);
void yapCodeAppendInteger(struct yapCompiler *compiler, yapCode *code, struct yapToken *token);
void yapCodeAppendIdentifier(struct yapCompiler *compiler, yapCode *code, struct yapToken *token);
void yapCodeAppendNull(struct yapCompiler *compiler, yapCode *code);
void yapCodeAppendCall(struct yapCompiler *compiler, yapCode *code, struct yapToken *token, yapCode *args);

void yapCodeAppendAdd(struct yapCompiler *compiler, yapCode *code, yapCode *add);

#define yapCodeCreate() ((yapCode*)yapAlloc(sizeof(yapCode)))
void yapCodeDestroy(yapCode *code);
void yapCodeGrow(yapCode *code, int count);
void yapCodeAppend(yapCode *code, yOpcode opcode, yOperand operand);
yS32 yapCodeLastIndex(yapCode *code);

void yapCodeAppendKeep(struct yapCompiler *compiler, yapCode *code, int keepCount);
void yapCodeAppendExpression(struct yapCompiler *compiler, yapCode *code, yapCode *expr, int keepCount);
void yapCodeAppendNamedArg(struct yapCompiler *compiler, yapCode *code, struct yapToken *name);
void yapCodeAppendVar(struct yapCompiler *compiler, yapCode *code, struct yapToken *token, yBool popRef);
void yapCodeAppendVarRef(struct yapCompiler *compiler, yapCode *code, struct yapToken *token);
void yapCodeAppendSetVar(yapCode *code);
void yapCodeAppendCode(yapCode *dst, yapCode *src);
void yapCodeAppendRet(yapCode *code, int argcount);
// ---------------------------------------------------------------------------

#endif

