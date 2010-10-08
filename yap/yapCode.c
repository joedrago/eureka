#include "yapCode.h"

#include "yapCompiler.h"
#include "yapLexer.h"
#include "yapModule.h"
#include "yapOp.h"

#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// Expression

void yapCodeAppendLiteralString(struct yapCompiler *compiler, yapCode *code, struct yapToken *token)
{
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_PUSH_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, token->text, token->len));
    code->offer++;
}

void yapCodeAppendInteger(struct yapCompiler *compiler, yapCode *code, struct yapToken *token)
{
    int intVal = yapTokenToInt(token);
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_PUSH_KI, yap32ArrayPushUnique(&compiler->module->kInts, intVal));
    code->offer++;
}

void yapCodeAppendIdentifier(struct yapCompiler *compiler, yapCode *code, struct yapToken *token)
{
    yapCodeGrow(code, 2);
    yapCodeAppend(code, YOP_VARREF_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, token->text, token->len));
    yapCodeAppend(code, YOP_REFVAL, 0);
    code->offer++;
}

void yapCodeAppendNull(struct yapCompiler *compiler, yapCode *code)
{
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_PUSHNULL, 0);
    code->offer++;
}

void yapCodeAppendCall(struct yapCompiler *compiler, yapCode *code, struct yapToken *token, yapCode *args)
{
    yapCodeAppendCode(code, args);
    yapCodeGrow(code, 2);
    yapCodeAppend(code, YOP_VARREF_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, token->text, token->len));
    yapCodeAppend(code, YOP_CALL, args->offer);
    code->call = yTrue;
    code->offer = 0;
}

// ---------------------------------------------------------------------------
// Code

void yapCodeDestroy(yapCode *code)
{
    if(code->ops)
        yapFree(code->ops);
    yapFree(code);
}

void yapCodeGrow(yapCode *code, int count)
{
    if(code->count + count <= code->size)
        return;

    if(code->size)
        code->ops = yapRealloc(code->ops, sizeof(yapOp) * (code->size+count));
    else
        code->ops = yapAlloc(sizeof(yapOp) * count);
    code->size += count;
}

void yapCodeAppend(yapCode *code, yOpcode opcode, yOperand operand)
{
    yapOp *op = &code->ops[code->count];
    op->opcode  = opcode;
    op->operand = operand;
    code->count++;
}

yS32 yapCodeLastIndex(yapCode *code)
{
    return (code->count - 1);
}

void yapCodeAppendKeep(struct yapCompiler *compiler, yapCode *code, int keepCount)
{
    int offerCount = code->offer;

    if(code->call)
    {
        yapCodeGrow(code, 1);
        yapCodeAppend(code, YOP_KEEP, keepCount);
    }
    else
    {
        if(offerCount > keepCount)
        {
            yapCodeGrow(code, 1);
            yapCodeAppend(code, YOP_POP, (yOperand)(offerCount - keepCount));
        }
        else if(offerCount < keepCount)
        {
            int i;
            int nulls = keepCount - offerCount;
            yapCodeGrow(code, nulls);
            for(i=0; i<nulls; i++)
                yapCodeAppend(code, YOP_PUSHNULL, 0);
        }
    }

    code->offer = keepCount;
    code->call  = yFalse;
}

void yapCodeAppendExpression(yapCompiler *compiler, yapCode *code, yapCode *expr, int keepCount)
{
    int offerCount = expr->offer;

    yapCodeAppendKeep(compiler, expr, keepCount);
    yapCodeAppendCode(code, expr);
    code->offer = keepCount;
    code->call = yFalse;
}

void yapCodeAppendNamedArg(struct yapCompiler *compiler, yapCode *code, struct yapToken *name)
{
    yapCodeAppendVar(compiler, code, name, yFalse);
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_SETARG, 0);
}

void yapCodeAppendVar(struct yapCompiler *compiler, yapCode *code, struct yapToken *token, yBool popRef)
{
    int growAmount = (popRef) ? 2 : 1;
    yapCodeGrow(code, growAmount);
    yapCodeAppend(code, YOP_VARREG_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, token->text, token->len));
    if(popRef)
        yapCodeAppend(code, YOP_POP, 1);
}

void yapCodeAppendVarRef(struct yapCompiler *compiler, yapCode *code, struct yapToken *token)
{
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_VARREF_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, token->text, token->len));
}

void yapCodeAppendSetVar(yapCode *code)
{
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_SETVAR, 0);
}

void yapCodeAppendCode(yapCode *dst, yapCode *src)
{
    if(!src->count)
        return;

    yapCodeGrow(dst, src->count);
    memcpy(&dst->ops[dst->count], src->ops, src->count*sizeof(yapOp));
    dst->count += src->count;
    dst->offer += src->offer;
    dst->call   = src->call;
}

void yapCodeAppendRet(yapCode *code, int argcount)
{
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_RET, argcount);
}

void yapCodeAppendAdd(struct yapCompiler *compiler, yapCode *code, yapCode *add)
{
}

