#include "yapCode.h"

#include "yapCompiler.h"
#include "yapLexer.h"
#include "yapModule.h"
#include "yapOp.h"

#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// Expression

yapExpression * yapExpressionCreateLiteralString(struct yapToken *token)
{
    yapExpression *e = yapExpressionCreate();
    e->token = *token;
    e->type = YEP_LITERALSTRING;
    return e;
}

yapExpression * yapExpressionCreateIdentifier(struct yapToken *token)
{
    yapExpression *e = yapExpressionCreate();
    e->token = *token;
    e->type = YEP_IDENTIFIER;
    return e;
}

yapExpression * yapExpressionCreateNull()
{
    yapExpression *e = yapExpressionCreate();
    e->type = YEP_NULL;
    return e;
}

yapExpression * yapExpressionCreateCall(struct yapToken *token)
{
    yapExpression *e = yapExpressionCreate();
    e->token = *token;
    e->type = YEP_CALL;
    return e;
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

void yapCodeAppendExpression(yapCompiler *compiler, yapCode *code, yapExpression *expr, int keepCount)
{
    int offerCount = -1;

    switch(expr->type)
    {
        case YEP_LITERALSTRING:
        {
            yapCodeGrow(code, 1);
            yapCodeAppend(code, YOP_PUSH_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, expr->token.text, expr->token.len));
            offerCount = 1;
            break;
        }
        case YEP_IDENTIFIER:
        {
            yapCodeGrow(code, 2);
            yapCodeAppend(code, YOP_VARREF_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, expr->token.text, expr->token.len));
            yapCodeAppend(code, YOP_REFVAL, 0);
            offerCount = 1;
            break;
        }
        case YEP_NULL:
        {
            yapCodeGrow(code, 1);
            yapCodeAppend(code, YOP_PUSHNULL, 0);
            offerCount = 1;
            break;
        }
        case YEP_CALL:
        {
            yapCodeGrow(code, 3);
            yapCodeAppend(code, YOP_VARREF_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, expr->token.text, expr->token.len));
            yapCodeAppend(code, YOP_CALL, 0);
            yapCodeAppend(code, YOP_KEEP, keepCount);
            break;
        }
        default:
            printf("Unknown expression");
    }

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
    yapCodeGrow(code, 2);
    yapCodeAppend(code, YOP_VARREF_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, token->text, token->len));
    yapCodeAppend(code, YOP_REFVAL, 0);
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
