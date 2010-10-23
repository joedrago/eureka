#include "yapCompiler.h"

#include "yapBlock.h"
#include "yapCode.h"
#include "yapLexer.h"
#include "yapModule.h"
#include "yapParser.h"
#include "yapSyntax.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void *yapParseAlloc();
void yapParse(void *yyp, int id, yapToken token, yapCompiler *compiler);
void yapParseFree(void *p);

yBool yapAssemble(yapCompiler *compiler);

void yapCompilerDestroy(yapCompiler *compiler)
{
    yapFree(compiler);
}

yBool yapCompile(yapCompiler *compiler, const char *text)
{
    yBool success = yFalse;
    yapToken emptyToken = {0};
    void *parser;

    yapTraceMem(("\n                                     "
                 "--- start module compile ---\n"));

    parser = yapParseAlloc();
    compiler->root = NULL;

    yapLex(parser, text, yapParse, compiler);
    yapParse(parser, 0, emptyToken, compiler);

    if(compiler->errors.count)
    {
        int i;
        for(i=0; i<compiler->errors.count; i++)
        {
            char *error = (char *)compiler->errors.data[i];
            printf("Error: %s\n", error);
        }
    }
    else if(!compiler->root)
    {
        printf("unknown badness\n");
    }
    else
    {
        yapAssemble(compiler);
        success = yTrue;
    };

    if(compiler->root)
    {
        yapSyntaxDot(compiler->root);
        yapSyntaxDestroy(compiler->root);
    };
    yapParseFree(parser);

    yapTraceMem(("                                     "
                 "---  end  module compile ---\n\n"));

    if(success && compiler->module)
        yapModuleDump(compiler->module);

    return success;
}

void yapCompileError(yapCompiler *compiler, const char *error)
{
    yapArrayPush(&compiler->errors, yapStrdup(error));
}

void yapCompileSyntaxError(yapCompiler *compiler, struct yapToken *token)
{
    char error[64];
    strcpy(error, "syntax error near '");
    if(token && token->text)
    {
        char temp[32];
        int len = strlen(token->text);
        if(len > 31) len = 31;
        memcpy(temp, token->text, len);
        temp[len] = 0;
        strcat(error, temp);
    }
    else
    {
        strcat(error, "<unknown>");
    }
    strcat(error, "'");

    yapCompileError(compiler, error);
}

// ---------------------------------------------------------------------------

enum
{
    ASM_NORMAL = 0,
    ASM_LVALUE = (1 << 0),
    ASM_VAR    = (1 << 1)
};

#define ASM_ALL_ARGS (-1)

#define asmFunc(NAME) \
yS32 yapAssemble ## NAME (yapCompiler *compiler, yapCode *dst, yapSyntax *syntax, yS32 keep, yU32 flags)
typedef yS32 (*yapAssembleFunc)(yapCompiler *compiler, yapCode *dst, yapSyntax *syntax, yS32 keep, yU32 flags);

typedef struct yapAssembleInfo
{
    yapAssembleFunc assemble;
} yapAssembleInfo;


asmFunc(Nop);
asmFunc(KString);
asmFunc(KInt);
asmFunc(Identifier);
asmFunc(StatementExpr);
asmFunc(StatementList);
asmFunc(ExpressionList);
asmFunc(IdentifierList);
asmFunc(Call);
asmFunc(Null);
asmFunc(StringFormat);
asmFunc(Unary);
asmFunc(Binary);
asmFunc(Var);
asmFunc(Return);
asmFunc(Assignment);
asmFunc(IfElse);
asmFunc(Loop);
asmFunc(Function);

static yapAssembleInfo asmDispatch[YST_COUNT] =
{
    { yapAssembleNop },             // YST_NOP

    { yapAssembleKString },         // YST_KSTRING
    { yapAssembleKInt },            // YST_KINT
    { yapAssembleIdentifier },      // YST_IDENTIFIER

    { yapAssembleStatementList },   // YST_STATEMENTLIST
    { yapAssembleExpressionList },  // YST_EXPRESSIONLIST
    { yapAssembleIdentifierList },  // YST_IDENTIFIERLIST

    { yapAssembleCall },            // YST_CALL
    { yapAssembleStringFormat },    // YST_STRINGFORMAT

    { yapAssembleNull },            // YST_NULL

    { yapAssembleUnary },           // YST_TOSTRING
    { yapAssembleUnary },           // YST_TOINT
    { yapAssembleBinary },          // YST_ADD
    { yapAssembleBinary },          // YST_SUB
    { yapAssembleBinary },          // YST_MUL
    { yapAssembleBinary },          // YST_DIV

    { yapAssembleStatementExpr },   // YST_STATEMENT_EXPR
    { yapAssembleAssignment },      // YST_ASSIGNMENT
    { yapAssembleVar },             // YST_VAR
    { yapAssembleReturn },          // YST_RETURN

    { yapAssembleIfElse },          // YST_IFELSE
    { yapAssembleLoop },            // YST_LOOP
    { yapAssembleFunction },        // YST_FUNCTION
};

// This function ensures that what we're being asked to keep is what we offered
int asmPad(yapCode *code, int keep, int offer)
{
    if(keep == ASM_ALL_ARGS)
    {
        keep = offer;
    }
    else
    {
        if(offer > keep)
        {
            yapCodeGrow(code, 1);
            yapCodeAppend(code, YOP_POP, (yOperand)(offer - keep));
        }
        else if(offer < keep)
        {
            int i;
            int nulls = keep - offer;
            yapCodeGrow(code, nulls);
            for(i=0; i<nulls; i++)
                yapCodeAppend(code, YOP_PUSHNULL, 0);
        }
    }
    return keep;
}

#define PAD(OFFER) asmPad(dst, keep, OFFER)

asmFunc(Nop)
{
    printf("NOP detected: %d\n", syntax->type);
    return PAD(0);
}

asmFunc(KString)
{
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_PUSH_KS, yapArrayPushUniqueString(&compiler->module->kStrings, yapStrdup(syntax->v.s)));
    return PAD(1);
}

asmFunc(KInt)
{
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_PUSH_KI, yap32ArrayPushUnique(&compiler->module->kInts, syntax->v.i));
    return PAD(1);
}

asmFunc(Identifier)
{
    yOpcode opcode = YOP_VARREF_KS;
    if(flags & ASM_VAR)
        opcode = YOP_VARREG_KS;
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, opcode, yapArrayPushUniqueString(&compiler->module->kStrings, yapStrdup(syntax->v.s)));
    if(!(flags & ASM_LVALUE))
    {
        yapCodeGrow(dst, 1);
        yapCodeAppend(dst, YOP_REFVAL, 0);
    }
    return PAD(1);
}

asmFunc(IdentifierList)
{
    yapArray *args = syntax->v.a;
    int keepCount = 0;
    int i;

    for(i=args->count-1; i>=0; i--)
    {
        yapSyntax *arg = (yapSyntax *)args->data[i];
        keepCount = asmDispatch[arg->type].assemble(compiler, dst, arg, 1, flags);
        yapCodeGrow(dst, 1);
        yapCodeAppend(dst, YOP_SETVAR, 0);
    }
    return PAD(args->count);
}

asmFunc(Call)
{
    char *name = syntax->v.s;
    yapSyntax *args = syntax->r.p;
    int argCount = asmDispatch[args->type].assemble(compiler, dst, args, ASM_ALL_ARGS, ASM_NORMAL);

    yapCodeGrow(dst, 3);
    yapCodeAppend(dst, YOP_VARREF_KS, yapArrayPushUniqueString(&compiler->module->kStrings, yapStrdup(name)));
    yapCodeAppend(dst, YOP_CALL, argCount);
    yapCodeAppend(dst, YOP_KEEP, keep);
    return PAD(keep);
}

asmFunc(Null)
{
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_PUSHNULL, 0);
    return PAD(1);
}

asmFunc(StringFormat)
{
    yapSyntax *format = syntax->l.p;
    yapSyntax *args   = syntax->r.p;
    int argCount = asmDispatch[args->type].assemble(compiler, dst, args, ASM_ALL_ARGS, ASM_NORMAL);
    asmDispatch[format->type].assemble(compiler, dst, format, 1, ASM_NORMAL);
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_FORMAT, argCount);
    return PAD(1);
}

asmFunc(Unary)
{
    yapSyntax *expr = syntax->v.p;
    asmDispatch[expr->type].assemble(compiler, dst, expr, 1, ASM_NORMAL);
    yapCodeGrow(dst, 1);
    switch(syntax->type)
    {
        case YST_TOSTRING: yapCodeAppend(dst, YOP_TOSTRING, 0); break;
        case YST_TOINT:    yapCodeAppend(dst, YOP_TOINT,    0); break;
    };
    return PAD(1);
}

asmFunc(Binary)
{
    yapSyntax *a = syntax->l.p;
    yapSyntax *b = syntax->r.p;
    asmDispatch[a->type].assemble(compiler, dst, a, 1, ASM_NORMAL);
    asmDispatch[b->type].assemble(compiler, dst, b, 1, ASM_NORMAL);

    yapCodeGrow(dst, 1);
    switch(syntax->type)
    {
        case YST_ADD: yapCodeAppend(dst, YOP_ADD, 0); break;
        case YST_SUB: yapCodeAppend(dst, YOP_SUB, 0); break;
        case YST_MUL: yapCodeAppend(dst, YOP_MUL, 0); break;
        case YST_DIV: yapCodeAppend(dst, YOP_DIV, 0); break;
    };

    return PAD(1);
}

asmFunc(Var)
{
    yapSyntax *expr = syntax->v.p;
    asmDispatch[expr->type].assemble(compiler, dst, expr, 1, ASM_VAR|ASM_LVALUE);
    return PAD(1);
}

asmFunc(Return)
{
    yapSyntax *expr = syntax->r.p;
    int retCount = asmDispatch[expr->type].assemble(compiler, dst, expr, 1, ASM_NORMAL);
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_RET, retCount);
    return PAD(0);
}

asmFunc(Assignment)
{
    yapSyntax *l = syntax->l.p;
    yapSyntax *r = syntax->r.p;
    int leave = (keep > 0) ? 1 : 0;

    asmDispatch[r->type].assemble(compiler, dst, r, 1, ASM_NORMAL);
    asmDispatch[l->type].assemble(compiler, dst, l, 1, ASM_LVALUE);
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_SETVAR, leave);
    return PAD(leave);
}

asmFunc(IfElse)
{
    yapSyntax *cond     = syntax->v.p;
    yapSyntax *ifBody   = syntax->l.p;
    yapSyntax *elseBody = syntax->r.p;
    yapCode *ifCode = yapCodeCreate();
    int index;
    int i;

    if(elseBody)
    {
        yapCode *elseCode = yapCodeCreate();
        asmDispatch[elseBody->type].assemble(compiler, elseCode, elseBody, 0, ASM_NORMAL);
        yapCodeGrow(elseCode, 1);
        yapCodeAppend(elseCode, YOP_LEAVE, 0);
        index = yapBlockConvertCode(elseCode, compiler->module, 0);
        yapCodeGrow(dst, 1);
        yapCodeAppend(dst, YOP_PUSHLBLOCK, index);
    }

    asmDispatch[ifBody->type].assemble(compiler, ifCode, ifBody, 0, ASM_NORMAL);
    yapCodeGrow(ifCode, 1);
    yapCodeAppend(ifCode, YOP_LEAVE, 0);
    index = yapBlockConvertCode(ifCode, compiler->module, 0);
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_PUSHLBLOCK, index);

    // Only keeps the value of the first expression on the stack for bool testing
    asmDispatch[cond->type].assemble(compiler, dst, cond, 1, ASM_NORMAL);

    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_IF, (elseBody) ? 1 : 0);

    return PAD(0);
}

asmFunc(Loop)
{
    yapSyntax *cond = syntax->v.p;
    yapSyntax *body = syntax->l.p;
    yapCode   *loop = yapCodeCreate();
    int index;

    yapCodeGrow(loop, 1);
    yapCodeAppend(loop, YOP_START, 0);

    asmDispatch[cond->type].assemble(compiler, loop, cond, 1, ASM_NORMAL);
    yapCodeGrow(loop, 1);
    yapCodeAppend(loop, YOP_LEAVE, 1);

    asmDispatch[body->type].assemble(compiler, loop, body, 0, ASM_NORMAL);

    yapCodeGrow(loop, 1);
    yapCodeAppend(loop, YOP_BREAK, 1);

    index = yapBlockConvertCode(loop, compiler->module, 0);

    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_PUSHLBLOCK, index);
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_ENTER, 0);

    return PAD(0);
}

asmFunc(Function)
{
    yapSyntax *args = syntax->l.p;
    yapSyntax *body = syntax->r.p;
    char *name = syntax->v.s;
    yapCode *code = yapCodeCreate();
    yOperand index;

    int argCount = asmDispatch[args->type].assemble(compiler, code, args, ASM_ALL_ARGS, ASM_VAR|ASM_LVALUE);
    asmDispatch[body->type].assemble(compiler, code, body, 0, ASM_NORMAL);
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_RET, 0);
    index = yapBlockConvertCode(code, compiler->module, argCount);

    // Register the new block as a function in the current scope
    yapCodeGrow(dst, 3);
    yapCodeAppend(dst, YOP_PUSHLBLOCK, index);
    yapCodeAppend(dst, YOP_VARREG_KS, yapArrayPushUniqueString(&compiler->module->kStrings, yapStrdup(name)));
    yapCodeAppend(dst, YOP_SETVAR, 0);
    return PAD(0);
}

asmFunc(ExpressionList)
{
    int i;
    int keepCount = 0;
    for(i=0; i<syntax->v.a->count; i++)
    {
        int keepIt = ((keep == ASM_ALL_ARGS) || (i < keep)) ? 1 : 0; // keep one from each expr, dump the rest
        yapSyntax *child = (yapSyntax *)syntax->v.a->data[i];
        keepCount += asmDispatch[child->type].assemble(compiler, dst, child, keepIt, ASM_NORMAL);
    }
    return PAD(keepCount);
}

asmFunc(StatementExpr)
{
    yapSyntax *child = syntax->v.p;
    int keepCount = asmDispatch[child->type].assemble(compiler, dst, child, keep, ASM_NORMAL);
    return PAD(keepCount);
}

asmFunc(StatementList)
{
    int keepCount = 0;
    int i;
    for(i=0; i<syntax->v.a->count; i++)
    {
        yapSyntax *child = (yapSyntax *)syntax->v.a->data[i];
        keepCount = asmDispatch[child->type].assemble(compiler, dst, child, 0, ASM_NORMAL);
    }
    return PAD(keepCount);
}

yBool yapAssemble(yapCompiler *compiler)
{
    if(compiler->root && (compiler->root->type == YST_STATEMENTLIST))
    {
        int blockIndex;
        compiler->module = yapModuleCreate();
        compiler->code   = yapCodeCreate();
        yapAssembleStatementList(compiler, compiler->code, compiler->root, 0, ASM_NORMAL);
        yapCodeGrow(compiler->code, 1);
        yapCodeAppend(compiler->code, YOP_RET, 0);
        blockIndex = yapBlockConvertCode(compiler->code, compiler->module, 0);
        compiler->module->block = (yapBlock*)compiler->module->blocks.data[blockIndex];
    }
    return yTrue;
}
