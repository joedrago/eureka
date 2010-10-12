#include "yapCode.h"

#include "yapCompiler.h"
#include "yapLexer.h"
#include "yapModule.h"
#include "yapOp.h"

#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// Code

yapCode * yapCodeCreateLiteralString(struct yapCompiler *compiler, struct yapToken *token)
{
    yapCode *code = yapCodeCreate();
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_PUSH_KS, yapArrayPushUniqueToken(&compiler->module->kStrings, token));
    return code;
}

yapCode * yapCodeCreateInteger(struct yapCompiler *compiler, struct yapToken *token)
{
    yapCode *code = yapCodeCreate();
    int intVal = yapTokenToInt(token);
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_PUSH_KI, yap32ArrayPushUnique(&compiler->module->kInts, intVal));
    return code;
}

yapCode * yapCodeCreateIdentifier(struct yapCompiler *compiler, struct yapToken *token)
{
    yapCode *code = yapCodeCreate();
    yapCodeGrow(code, 2);
    yapCodeAppend(code, YOP_VARREF_KS, yapArrayPushUniqueToken(&compiler->module->kStrings, token));
    yapCodeAppend(code, YOP_REFVAL, 0);
    return code;
}

yapCode * yapCodeCreateNull(struct yapCompiler *compiler)
{
    yapCode *code = yapCodeCreate();
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_PUSHNULL, 0);
    return code;
}

yapCode * yapCodeCreateCall(struct yapCompiler *compiler, struct yapToken *token, yapArray *args)
{
    yapCode *code = yapCodeCreate();
    int i;
    for(i=0; i<args->count; i++)
    {
        yapCodeAppendExpression(compiler, code, (yapCode*)args->data[i], 1);
    }
    yapCodeGrow(code, 3);
    yapCodeAppend(code, YOP_VARREF_KS, yapArrayPushUniqueToken(&compiler->module->kStrings, token));
    yapCodeAppend(code, YOP_CALL, args->count);
    code->keepIndex = yapCodeAppend(code, YOP_KEEP, 0);
    yapArrayDestroy(args, (yapDestroyCB)yapCodeDestroy);
    return code;
}

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

int yapCodeAppend(yapCode *code, yOpcode opcode, yOperand operand)
{
    yapOp *op = &code->ops[code->count];
    op->opcode  = opcode;
    op->operand = operand;
    code->count++;
    return code->count - 1;
}

yS32 yapCodeLastIndex(yapCode *code)
{
    return (code->count - 1);
}

void yapCodeAppendExpression(yapCompiler *compiler, yapCode *code, yapCode *expr, int keepCount)
{
    int offerCount = -1;

    if(expr->keepIndex)
        expr->ops[expr->keepIndex].operand = keepCount;

    yapCodeAppendCode(code, expr);

    if(offerCount != -1)
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
}

void yapCodeAppendNamedArg(struct yapCompiler *compiler, yapCode *code, yapToken *name)
{
    yapCodeGrow(code, 2);
    yapCodeAppend(code, YOP_VARREG_KS, yapArrayPushUniqueToken(&compiler->module->kStrings, name));
    yapCodeAppend(code, YOP_SETVAR, 0);
}

void yapCodeAppendVar(struct yapCompiler *compiler, yapCode *code, struct yapToken *token, yBool popRef)
{
    int growAmount = (popRef) ? 2 : 1;
    yapCodeGrow(code, growAmount);
    yapCodeAppend(code, YOP_VARREG_KS, yapArrayPushUniqueToken(&compiler->module->kStrings, token));
    if(popRef)
        yapCodeAppend(code, YOP_POP, 1);
}

void yapCodeAppendVarRef(struct yapCompiler *compiler, yapCode *code, struct yapToken *token)
{
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_VARREF_KS, yapArrayPushUniqueToken(&compiler->module->kStrings, token));
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
}

void yapCodeAppendRet(yapCode *code, int argcount)
{
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_RET, argcount);
}
