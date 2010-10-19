#ifndef YAPCODE_H
#define YAPCODE_H

#include "yapTypes.h"

// for yapToken
#include "yapLexer.h"

// ---------------------------------------------------------------------------
// Forwards

struct yapCompiler;

// ---------------------------------------------------------------------------

typedef struct yapCode
{
    struct yapOp *ops;
    yS32 size;
    yS32 count;
    yS32 keepIndex;                    // late fixup op index for YOP_KEEP. Due to YOP_CALL, must be non-zero
} yapCode;

#define yapCodeCreate() ((yapCode*)yapAlloc(sizeof(yapCode)))
yapCode * yapCodeCreateLiteralString(struct yapCompiler *compiler, struct yapToken *token);
yapCode * yapCodeCreateInteger(struct yapCompiler *compiler, struct yapToken *token);
yapCode * yapCodeCreateIdentifier(struct yapCompiler *compiler, struct yapToken *token);
yapCode * yapCodeCreateNull(struct yapCompiler *compiler);
yapCode * yapCodeCreateCall(struct yapCompiler *compiler, struct yapToken *token, yapArray *args);
yapCode * yapCodeCreateStringFormat(struct yapCompiler *compiler, yapCode *format, yapArray *args);

void yapCodeDestroy(yapCode *code);

void yapCodeGrow(yapCode *code, int count);
yS32 yapCodeAppend(yapCode *code, yOpcode opcode, yOperand operand);
yS32 yapCodeLastIndex(yapCode *code);

void yapCodeAppendExpression(struct yapCompiler *compiler, yapCode *code, yapCode *expr, int keepCount);
void yapCodeAppendNamedArg(struct yapCompiler *compiler, yapCode *code, yapToken *name);
void yapCodeAppendVar(struct yapCompiler *compiler, yapCode *code, struct yapToken *token, yBool popRef);
void yapCodeAppendVarRef(struct yapCompiler *compiler, yapCode *code, struct yapToken *token);
void yapCodeAppendSetVar(yapCode *code);
void yapCodeAppendCode(yapCode *dst, yapCode *src);
void yapCodeAppendRet(yapCode *code, int argcount);

// ---------------------------------------------------------------------------

#endif
