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

void yapCodeAppendExpression(yapCompiler *compiler, yapCode *code, yapExpression *expr)
{
    switch(expr->type)
    {
        case YEP_LITERALSTRING:
        {
            yapCodeGrow(code, 1);
            yapCodeAppend(code, YOP_PUSH_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, expr->token.text, expr->token.len));
            break;
        }
        case YEP_IDENTIFIER:
        {
            yapCodeGrow(code, 2);
            yapCodeAppend(code, YOP_VARREF_KS, yapArrayPushUniqueStringLen(&compiler->module->kStrings, expr->token.text, expr->token.len));
            yapCodeAppend(code, YOP_REFVAL, 0);
            break;
        }
        case YEP_NULL:
        {
            yapCodeGrow(code, 1);
            yapCodeAppend(code, YOP_PUSHNULL, 0);
            break;
        }
        default:
            printf("Unknown expression");
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
    yapCodeGrow(dst, src->count);
    memcpy(&dst->ops[dst->count], src->ops, src->count*sizeof(yapOp));
    dst->count += src->count;
}
