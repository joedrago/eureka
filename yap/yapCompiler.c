#include "yapCompiler.h"

#include "yapBlock.h"
#include "yapCode.h"
#include "yapFrame.h"
#include "yapLexer.h"
#include "yapChunk.h"
#include "yapOp.h"
#include "yapParser.h"
#include "yapSyntax.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Not in yapParser.h because it is autogenerated
void *yapParseAlloc();
void yapParse(void *yyp, int id, yapToken token, yapCompiler *compiler);
void yapParseFree(void *p);
// ---------------------------------------------------------------------------

void yapCompileOptimize(yapCompiler *compiler);

yBool yapAssemble(yapCompiler *compiler);

void yapCompilerDestroy(yapCompiler *compiler)
{
    if(compiler->chunk)
        yapChunkDestroy(compiler->chunk);
    if(compiler->root)
        yapSyntaxDestroy(compiler->root);
    yapArrayClear(&compiler->errors, (yapDestroyCB)yapFree);
    yapFree(compiler);
}

yBool yapCompile(yapCompiler *compiler, const char *text, yU32 compileOpts)
{
    yBool success = yFalse;
    yapToken emptyToken = {0};
    void *parser;

    yapTraceMem(("\n                                     "
                 "--- start chunk compile ---\n"));

    parser = yapParseAlloc();
    compiler->root = NULL;

    yapLex(parser, text, yapParse, compiler);
    yapParse(parser, 0, emptyToken, compiler);

    if(compiler->root)
    {
        if(compileOpts & YCO_OPTIMIZE)
            yapCompileOptimize(compiler);

        yapAssemble(compiler);
        success = yTrue;

        if(!(compileOpts & YCO_KEEP_SYNTAX_TREE))
        {
            yapSyntaxDestroy(compiler->root);
            compiler->root = NULL;
        };
    }
    else
    {
        printf("unknown badness - grammar is probably incomplete\n");
    }

    yapParseFree(parser);

    yapTraceMem(("                                     "
                 "---  end  chunk compile ---\n\n"));

    return success;
}

void yapCompileError(yapCompiler *compiler, const char *error)
{
    yapArrayPush(&compiler->errors, yapStrdup(error));
}

static void appendInt(char *text, int i)
{
    char temp[32];
    sprintf(temp, "%d", i);
    strcat(text, temp);
}

void yapCompileSyntaxError(yapCompiler *compiler, struct yapToken *token)
{
    char error[64];
    strcpy(error, "Line [");
    appendInt(error, token->line);
    strcat(error, "]: ");
    strcat(error, "syntax error near '");
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

static int yapCompileOptimizeChild(yapCompiler *compiler, yapSyntax *syntax);

static void yapCompileOptimizeArray(yapCompiler *compiler, yapArray *a)
{
    int i;
    for(i = 0; i < a->count; i++)
    {
        yapCompileOptimizeChild(compiler, (yapSyntax *)a->data[i]);
    }
}

static int yapCompileOptimizeChild(yapCompiler *compiler, yapSyntax *syntax)
{
    if(syntax->v.a) yapCompileOptimizeArray(compiler, syntax->v.a);
    if(syntax->v.p) yapCompileOptimizeChild(compiler, syntax->v.p);
    if(syntax->l.a) yapCompileOptimizeArray(compiler, syntax->l.a);
    if(syntax->l.p) yapCompileOptimizeChild(compiler, syntax->l.p);
    if(syntax->r.a) yapCompileOptimizeArray(compiler, syntax->r.a);
    if(syntax->r.p) yapCompileOptimizeChild(compiler, syntax->r.p);

    switch(syntax->type)
    {
    case YST_ADD:
    case YST_SUB:
    case YST_MUL:
    case YST_DIV:
    {
        // Integer arithmetic optimization
        if((syntax->l.p->type == YST_KINT)
           && (syntax->r.p->type == YST_KINT))
        {
            int val;
            switch(syntax->type)
            {
            case YST_ADD:
                val = syntax->l.p->v.i + syntax->r.p->v.i;
                break;
            case YST_SUB:
                val = syntax->l.p->v.i - syntax->r.p->v.i;
                break;
            case YST_MUL:
                val = syntax->l.p->v.i * syntax->r.p->v.i;
                break;
            case YST_DIV:
                if(!syntax->r.p->v.i) return 0;
                val = syntax->l.p->v.i / syntax->r.p->v.i;
                break;
            };
            yapSyntaxDestroy(syntax->l.p);
            syntax->l.p = NULL;
            yapSyntaxDestroy(syntax->r.p);
            syntax->r.p = NULL;
            syntax->type = YST_KINT;
            syntax->v.i = val;
        }
    }
    break;
    };

    return 0;
}

void yapCompileOptimize(yapCompiler *compiler)
{
    yapCompileOptimizeChild(compiler, compiler->root);
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
typedef yS32(*yapAssembleFunc)(yapCompiler *compiler, yapCode *dst, yapSyntax *syntax, yS32 keep, yU32 flags);

typedef struct yapAssembleInfo
{
    yapAssembleFunc assemble;
} yapAssembleInfo;

asmFunc(Nop);
asmFunc(KString);
asmFunc(KInt);
asmFunc(KFloat);
asmFunc(Identifier);
asmFunc(Index);
asmFunc(StatementExpr);
asmFunc(StatementList);
asmFunc(ExpressionList);
asmFunc(IdentifierList);
asmFunc(Call);
asmFunc(Null);
asmFunc(StringFormat);
asmFunc(Unary);
asmFunc(Binary);
asmFunc(ShortCircuit);
asmFunc(Var);
asmFunc(Return);
asmFunc(Assignment);
asmFunc(Inherits);
asmFunc(IfElse);
asmFunc(While);
asmFunc(For);
asmFunc(Function);
asmFunc(With);

static yapAssembleInfo asmDispatch[YST_COUNT] =
{
    { yapAssembleNop },             // YST_NOP

    { yapAssembleKString },         // YST_KSTRING
    { yapAssembleKInt },            // YST_KINT
    { yapAssembleKFloat },          // YST_KFLOAT
    { yapAssembleIdentifier },      // YST_IDENTIFIER

    { yapAssembleIndex },           // YST_INDEX

    { yapAssembleStatementList },   // YST_STATEMENTLIST
    { yapAssembleExpressionList },  // YST_EXPRESSIONLIST
    { yapAssembleIdentifierList },  // YST_IDENTIFIERLIST

    { yapAssembleCall },            // YST_CALL
    { yapAssembleStringFormat },    // YST_STRINGFORMAT

    { yapAssembleNull },            // YST_NULL

    { yapAssembleUnary },           // YST_TOSTRING
    { yapAssembleUnary },           // YST_TOINT
    { yapAssembleUnary },           // YST_TOFLOAT
    { yapAssembleUnary },           // YST_NOT

    { yapAssembleUnary },           // YST_BITWISE_NOT
    { yapAssembleBinary },          // YST_BITWISE_XOR
    { yapAssembleBinary },          // YST_BITWISE_AND
    { yapAssembleBinary },          // YST_BITWISE_OR
    { yapAssembleBinary },          // YST_SHIFTLEFT
    { yapAssembleBinary },          // YST_SHIFTRIGHT

    { yapAssembleBinary },          // YST_ADD
    { yapAssembleBinary },          // YST_SUB
    { yapAssembleBinary },          // YST_MUL
    { yapAssembleBinary },          // YST_DIV

    { yapAssembleShortCircuit },    // YST_AND
    { yapAssembleShortCircuit },    // YST_OR

    { yapAssembleBinary },          // YST_CMP
    { yapAssembleBinary },          // YST_EQUALS
    { yapAssembleBinary },          // YST_NOTEQUALS
    { yapAssembleBinary },          // YST_GREATERTHAN
    { yapAssembleBinary },          // YST_GREATERTHANOREQUAL
    { yapAssembleBinary },          // YST_LESSTHAN
    { yapAssembleBinary },          // YST_LESSTHANOREQUAL

    { yapAssembleStatementExpr },   // YST_STATEMENT_EXPR
    { yapAssembleAssignment },      // YST_ASSIGNMENT
    { yapAssembleInherits },        // YST_INHERITS
    { yapAssembleVar },             // YST_VAR
    { yapAssembleReturn },          // YST_RETURN

    { yapAssembleIfElse },          // YST_IFELSE
    { yapAssembleWhile },           // YST_WHILE
    { yapAssembleFor },             // YST_FOR
    { yapAssembleFunction },        // YST_FUNCTION
    { yapAssembleWith },            // YST_WITH
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
            for(i = 0; i < nulls; i++)
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
    yapCodeAppend(dst, YOP_PUSH_KS, yapArrayPushUniqueString(&compiler->chunk->kStrings, yapStrdup(syntax->v.s)));
    return PAD(1);
}

asmFunc(KInt)
{
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_PUSH_KI, yap32ArrayPushUnique(&compiler->chunk->kInts, &syntax->v.i));
    return PAD(1);
}

asmFunc(KFloat)
{
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_PUSH_KF, yap32ArrayPushUnique(&compiler->chunk->kFloats, (yU32*)&syntax->v.f));
    return PAD(1);
}

asmFunc(Identifier)
{
    yOpcode opcode = YOP_VARREF_KS;
    if(flags & ASM_VAR)
        opcode = YOP_VARREG_KS;
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, opcode, yapArrayPushUniqueString(&compiler->chunk->kStrings, yapStrdup(syntax->v.s)));
    if(!(flags & ASM_LVALUE))
    {
        yapCodeGrow(dst, 1);
        yapCodeAppend(dst, YOP_REFVAL, 0);
    }
    return PAD(1);
}

asmFunc(Index)
{
    yapSyntax *a = syntax->l.p;
    yapSyntax *b = syntax->r.p;
    asmDispatch[a->type].assemble(compiler, dst, a, 1, ASM_NORMAL);
    asmDispatch[b->type].assemble(compiler, dst, b, 1, ASM_NORMAL);
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_INDEX, (flags & ASM_LVALUE) ? 1 : 0);
    return PAD(1);
}

asmFunc(IdentifierList)
{
    yapArray *args = syntax->v.a;
    int keepCount = 0;
    int i;

    for(i = args->count - 1; i >= 0; i--)
    {
        yapSyntax *arg = (yapSyntax *)args->data[i];
        keepCount = asmDispatch[arg->type].assemble(compiler, dst, arg, 1, flags);
        yapCodeGrow(dst, 1);
        yapCodeAppend(dst, YOP_SETVAR, 0);
    }
    return PAD(args->count);
}

// /
//     yapSyntax *a = syntax->l.p;
//     yapSyntax *b = syntax->r.p;
//     asmDispatch[a->type].assemble(compiler, dst, a, 1, ASM_NORMAL);
//     asmDispatch[b->type].assemble(compiler, dst, b, 1, ASM_NORMAL);
//     yapCodeGrow(dst, 1);
//     yapCodeAppend(dst, YOP_INDEX, (flags & ASM_LVALUE) ? 1 : 0);
//     return PAD(1);
// /

asmFunc(Call)
{
    yapSyntax *obj  = syntax->l.p;
    yapSyntax *func = syntax->v.p;
    yapSyntax *args = syntax->r.p;
    int argCount;

    if(obj)
    {
        asmDispatch[obj->type].assemble(compiler, dst, obj, 1, ASM_NORMAL);
        yapCodeGrow(dst, 1);
        yapCodeAppend(dst, YOP_DUPE, 0); // dupe object
        asmDispatch[func->type].assemble(compiler, dst, func, 1, ASM_NORMAL);
        yapCodeGrow(dst, 1);
        yapCodeAppend(dst, YOP_INDEX, 0);
    }
    else
    {
        asmDispatch[func->type].assemble(compiler, dst, func, 1, ASM_NORMAL);
    }
    // at this point, stack is either T[funcref, obj] or T[funcref]

    argCount = asmDispatch[args->type].assemble(compiler, dst, args, ASM_ALL_ARGS, ASM_NORMAL);
    yapCodeGrow(dst, 3);
    if(argCount)
        yapCodeAppend(dst, YOP_MOVE, argCount); // should move the funcref above the args
    yapCodeAppend(dst, YOP_CALL, argCount + ((obj) ? 1 : 0));
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
    case YST_TOSTRING:
        yapCodeAppend(dst, YOP_TOSTRING, 0);
        break;
    case YST_TOINT:
        yapCodeAppend(dst, YOP_TOINT, 0);
        break;
    case YST_TOFLOAT:
        yapCodeAppend(dst, YOP_TOFLOAT, 0);
        break;
    case YST_NOT:
        yapCodeAppend(dst, YOP_NOT, 0);
        break;
    case YST_BITWISE_NOT:
        yapCodeAppend(dst, YOP_BITWISE_NOT, 0);
        break;
    };
    return PAD(1);
}

asmFunc(Binary)
{
    yapSyntax *a = syntax->l.p;
    yapSyntax *b = syntax->r.p;
    yBool compound = syntax->v.i;
    int op = YOP_NOP;
    int aflags = ASM_NORMAL;

    if(compound)
        aflags = ASM_LVALUE;

    asmDispatch[a->type].assemble(compiler, dst, a, 1, aflags);
    if(compound)
    {
        yapCodeGrow(dst, 2);
        yapCodeAppend(dst, YOP_DUPE, 0);
        yapCodeAppend(dst, YOP_REFVAL, 0);
    }
    asmDispatch[b->type].assemble(compiler, dst, b, 1, ASM_NORMAL);

    switch(syntax->type)
    {
    // Legal in either compound statements or expressions
    case YST_ADD:
        op = YOP_ADD;
        break;
    case YST_SUB:
        op = YOP_SUB;
        break;
    case YST_MUL:
        op = YOP_MUL;
        break;
    case YST_DIV:
        op = YOP_DIV;
        break;
    case YST_BITWISE_XOR:
        op = YOP_BITWISE_XOR;
        break;
    case YST_BITWISE_AND:
        op = YOP_BITWISE_AND;
        break;
    case YST_BITWISE_OR:
        op = YOP_BITWISE_OR;
        break;
    case YST_SHIFTLEFT:
        op = YOP_SHIFTLEFT;
        break;
    case YST_SHIFTRIGHT:
        op = YOP_SHIFTRIGHT;
        break;

    // Legal in expressions only
    case YST_CMP:
        op = YOP_CMP;
        break;
    case YST_EQUALS:
        op = YOP_EQUALS;
        break;
    case YST_NOTEQUALS:
        op = YOP_NOTEQUALS;
        break;
    case YST_GREATERTHAN:
        op = YOP_GREATERTHAN;
        break;
    case YST_GREATERTHANOREQUAL:
        op = YOP_GREATERTHANOREQUAL;
        break;
    case YST_LESSTHAN:
        op = YOP_LESSTHAN;
        break;
    case YST_LESSTHANOREQUAL:
        op = YOP_LESSTHANOREQUAL;
        break;
    };

    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, op, 0);

    if(compound)
    {
        yapCodeGrow(dst, 2);
        yapCodeAppend(dst, YOP_MOVE, 1);
        yapCodeAppend(dst, YOP_SETVAR, 0);
        return PAD(0);
    }
    return PAD(1);
}

asmFunc(ShortCircuit)
{
    yS32 skipIndex;
    yapSyntax *a = syntax->l.p;
    yapSyntax *b = syntax->r.p;

    // code to add first expression on the stack
    asmDispatch[a->type].assemble(compiler, dst, a, 1, ASM_NORMAL);

    // test stack top's truth and uses the current value instead of the second expression's value
    yapCodeGrow(dst, 1);
    skipIndex = yapCodeAppend(dst, (syntax->type == YST_AND) ? YOP_AND : YOP_OR, 0);

    // code to add second expression on the stack
    asmDispatch[b->type].assemble(compiler, dst, b, 1, ASM_NORMAL);

    // clue in the and/or opcode how far to optionally jump
    dst->ops[skipIndex].operand = dst->count - skipIndex - 1;

    return PAD(1);
}

asmFunc(Var)
{
    yapSyntax *expr = syntax->v.p;
    asmDispatch[expr->type].assemble(compiler, dst, expr, 1, ASM_VAR | ASM_LVALUE);
    return PAD(1);
}

asmFunc(Return)
{
    yapSyntax *expr = syntax->v.p;
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

asmFunc(Inherits)
{
    yapSyntax *l = syntax->l.p;
    yapSyntax *r = syntax->r.p;
    int leave = (keep > 0) ? 1 : 0;

    asmDispatch[r->type].assemble(compiler, dst, r, 1, ASM_NORMAL);
    asmDispatch[l->type].assemble(compiler, dst, l, 1, ASM_LVALUE);
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_INHERITS, leave);
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
        index = yapBlockConvertCode(elseCode, compiler->chunk, 0);
        yapCodeGrow(dst, 1);
        yapCodeAppend(dst, YOP_PUSHLBLOCK, index);
    }

    asmDispatch[ifBody->type].assemble(compiler, ifCode, ifBody, 0, ASM_NORMAL);
    yapCodeGrow(ifCode, 1);
    yapCodeAppend(ifCode, YOP_LEAVE, 0);
    index = yapBlockConvertCode(ifCode, compiler->chunk, 0);
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_PUSHLBLOCK, index);

    // Only keeps the value of the first expression on the stack for bool testing
    asmDispatch[cond->type].assemble(compiler, dst, cond, 1, ASM_NORMAL);

    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_IF, (elseBody) ? 1 : 0);

    return PAD(0);
}

asmFunc(While)
{
    yapSyntax *cond = syntax->v.p;
    yapSyntax *body = syntax->r.p;
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

    index = yapBlockConvertCode(loop, compiler->chunk, 0);

    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_PUSHLBLOCK, index);
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_ENTER, YFT_LOOP);

    return PAD(0);
}

asmFunc(For)
{
    yapSyntax *vars = syntax->v.p;
    yapSyntax *iter = syntax->l.p;
    yapSyntax *body = syntax->r.p;
    yapCode   *loop = yapCodeCreate();
    int index;

    // Get the iterable onto the stack
    asmDispatch[iter->type].assemble(compiler, dst, iter, 1, ASM_NORMAL);

    yapCodeGrow(loop, 4);
    yapCodeAppend(loop, YOP_DUPE, 0);    // stash the object itself
    yapCodeAppend(loop, YOP_COUNT, 0);   // using the dupe, call .count() to push the count on top
    yapCodeAppend(loop, YOP_KEEP, 1);    // keep only one value from the call to .count()
    yapCodeAppend(loop, YOP_PUSHI, 0);   // init our counter to zero

    // At this point, the stack should look like:
    // -- top --
    // counter = 0
    // count = N
    // object

    yapCodeGrow(loop, 3);
    yapCodeAppend(loop, YOP_START, 0);   // begin loop
    yapCodeAppend(loop, YOP_SUB, 1);     // push (count-counter) onto stack
    yapCodeAppend(loop, YOP_LEAVE, 1);   // leave if ((count-counter) == 0)

    // Populate the loop vars from the call to get()
    yapCodeGrow(loop, 7);
    yapCodeAppend(loop, YOP_DUPE, 0);    // dupe counter
    yapCodeAppend(loop, YOP_DUPE, 3);    // dupe object
    yapCodeAppend(loop, YOP_NTH, 0);     // call nth
    yapCodeAppend(loop, YOP_KEEP, vars->v.a->count);
    asmDispatch[vars->type].assemble(compiler, loop, vars, vars->v.a->count, ASM_LVALUE | ASM_VAR);

    // Assemble the loop body itself
    asmDispatch[body->type].assemble(compiler, loop, body, 0, ASM_NORMAL);

    // Increment the counter and loop
    yapCodeGrow(loop, 3);
    yapCodeAppend(loop, YOP_PUSHI, 1);
    yapCodeAppend(loop, YOP_ADD, 0);
    yapCodeAppend(loop, YOP_BREAK, 1);

    index = yapBlockConvertCode(loop, compiler->chunk, 0);

    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_PUSHLBLOCK, index);
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_ENTER, YFT_LOOP);

    return PAD(0);
}

asmFunc(Function)
{
    yapSyntax *args = syntax->l.p;
    yapSyntax *body = syntax->r.p;
    char *name = syntax->v.s;
    yapCode *code = yapCodeCreate();
    yOperand index;

    compiler->chunk->hasFuncs = yTrue;

    // If a name is used ("func name()" vs "func()"), the
    // generated code will consume the block left on the stack
    // with a setvar, effectively compiling "func name()" into
    // "var name = func()".
    int additionalOpsForNaming = 0;
    int valuesLeftOnStack = 1;

    int argCount = asmDispatch[args->type].assemble(compiler, code, args, ASM_ALL_ARGS, ASM_VAR | ASM_LVALUE);
    asmDispatch[body->type].assemble(compiler, code, body, 0, ASM_NORMAL);
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_RET, 0);
    index = yapBlockConvertCode(code, compiler->chunk, argCount);

    if(name)
    {
        additionalOpsForNaming = 2;
        valuesLeftOnStack = 0;
    }
    yapCodeGrow(dst, 1 + additionalOpsForNaming);
    yapCodeAppend(dst, YOP_PUSHLBLOCK, index);    // Push the new block on the stack
    if(name)
    {
        yapCodeAppend(dst, YOP_VARREG_KS, yapArrayPushUniqueString(&compiler->chunk->kStrings, yapStrdup(name)));
        yapCodeAppend(dst, YOP_SETVAR, 0);
    };
    return PAD(valuesLeftOnStack);
}

asmFunc(With)
{
    yapSyntax *obj  = syntax->v.p;
    yapSyntax *body = syntax->r.p;
    yapCode   *code = yapCodeCreate();
    int index;

    // "obj" expression should ultimately leave a reference to an object on the stack
    asmDispatch[obj->type].assemble(compiler, dst, obj, 1, ASM_VAR | ASM_LVALUE);

    // create new block from "body"
    asmDispatch[body->type].assemble(compiler, code, body, 0, ASM_NORMAL);
    yapCodeGrow(code, 1);
    yapCodeAppend(code, YOP_LEAVE, 0);
    index = yapBlockConvertCode(code, compiler->chunk, 0);

    // enter the newly created block from the parent block, flagging the frame as type WITH
    // this will cause the object reference to be popped and associated with the frame
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_PUSHLBLOCK, index);
    yapCodeGrow(dst, 1);
    yapCodeAppend(dst, YOP_ENTER, YFT_WITH);

    return PAD(0);
}

asmFunc(ExpressionList)
{
    int i;
    int keepCount = 0;
    for(i = 0; i < syntax->v.a->count; i++)
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
    for(i = 0; i < syntax->v.a->count; i++)
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
        compiler->chunk = yapChunkCreate();
        compiler->code   = yapCodeCreate();
        yapAssembleStatementList(compiler, compiler->code, compiler->root, 0, ASM_NORMAL);
        yapCodeGrow(compiler->code, 1);
        yapCodeAppend(compiler->code, YOP_RET, 0);
        blockIndex = yapBlockConvertCode(compiler->code, compiler->chunk, 0);
        compiler->chunk->block = (yapBlock *)compiler->chunk->blocks.data[blockIndex];
    }
    return yTrue;
}
