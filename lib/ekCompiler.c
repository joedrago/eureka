// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekCompiler.h"

#include "ekBlock.h"
#include "ekChunk.h"
#include "ekCode.h"
#include "ekContext.h"
#include "ekFrame.h"
#include "ekLexer.h"
#include "ekOp.h"
#include "ekParser.h"
#include "ekSyntax.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ---------------------------------------------------------------------------

void ekCompileOptimize(struct ekContext * E, ekCompiler * compiler);
ekBool ekAssemble(struct ekContext * E, ekCompiler * compiler, ekBool useSourcePathForImports);

ekError * ekErrorCreate(struct ekContext * E, const char * filename, ekS32 lineNo, const char * source, const char * loc, const char * explanation)
{
    ekError * error = (ekError *)ekAlloc(sizeof(ekError));
    ekStringSet(E, &error->filename, filename);
    error->lineNo = lineNo;
    ekStringSet(E, &error->explanation, explanation);
    if (loc) {
        // Find the full line where things went wrong, and store the location in it
        const char * line = loc;
        const char * end;
        while ((line != source) && (*line != '\n') && (*line != '\r')) {
            --line;
        }
        if ((*line == '\n') || (*line == '\r')) {
            ++line;
        }
        end = line;
        while (*end && (*end != '\n') && (*end != '\r')) {
            ++end;
        }
        ekStringSetLen(E, &error->line, line, end - line);
        error->col = loc - line;
    }
    return error;
}

void ekErrorDestroy(ekContext * E, ekError * error)
{
    ekStringClear(E, &error->line);
    ekStringClear(E, &error->explanation);
    ekStringClear(E, &error->filename);
    ekFree(error);
}

ekCompiler * ekCompilerCreate(struct ekContext * E)
{
    ekCompiler * compiler = (ekCompiler *)ekAlloc(sizeof(ekCompiler));
    compiler->E = E;
    return compiler;
}

void ekCompilerDestroy(ekCompiler * compiler)
{
    struct ekContext * E = compiler->E;
    if (compiler->chunk) {
        ekChunkDestroy(E, compiler->chunk);
    }
    if (compiler->root) {
        ekSyntaxDestroy(E, compiler->root);
    }
    ekArrayDestroy(E, &compiler->errors, (ekDestroyCB)ekErrorDestroy);
    ekFree(compiler);
}

ekBool ekCompile(ekCompiler * compiler, const char * sourcePath, const char * source, ekU32 compileOpts)
{
    struct ekContext * E = compiler->E;
    ekBool success = ekFalse;
    ekParser * parser;

    compiler->sourcePath = sourcePath;
    compiler->source = source;

    ekTraceMem(("\n                                     "
                "--- start chunk compile ---\n"));

    compiler->root = NULL;

    parser = ekParserCreate(E);
    ekParserParse(E, parser, compiler, source);
    // ekLex(parser, source, ekParse, compiler);
    // ekParse(parser, 0, emptyToken, compiler);

    if (compiler->root) {
        if (ekArraySize(E, &compiler->errors)) {
            ekSyntaxDestroy(E, compiler->root);
            compiler->root = NULL;
        } else {
            if (compileOpts & ECO_OPTIMIZE) {
                ekCompileOptimize(E, compiler);
            }

            ekAssemble(E, compiler, ekTrue);
            success = ekTrue;

            if (!(compileOpts & ECO_KEEP_SYNTAX_TREE)) {
                ekSyntaxDestroy(E, compiler->root);
                compiler->root = NULL;
            }
        }
    }

    ekParserDestroy(E, parser);

    ekTraceMem(("                                     "
                "---  end  chunk compile ---\n\n"));

    compiler->sourcePath = NULL;
    compiler->source = NULL;
    return success;
}

static void appendInt(ekContext * E, ekString * str, ekS32 i)
{
    char temp[32];
    sprintf(temp, "%d", i);
    ekStringConcat(E, str, temp);
}

ekBool ekCompilerFormatErrors(ekCompiler * compiler, ekString * output)
{
    struct ekContext * E = compiler->E;
    ekS32 i;
    for (i = 0; i < ekArraySize(E, &compiler->errors); i++) {
        ekError * error = compiler->errors[i];
        ekStringConcat(E, output, ekStringSafePtr(&error->filename));
        ekStringConcat(E, output, ":");
        appendInt(E, output, error->lineNo);
        ekStringConcat(E, output, ":");
        appendInt(E, output, error->col + 1); // "column" in an error is 1-indexed
        ekStringConcat(E, output, ": error: ");
        ekStringConcat(E, output, ekStringSafePtr(&error->explanation));
        ekStringConcat(E, output, "\n");

        if (error->line.len > 0) {
            ekS32 caratPos = error->col;

            ekStringConcat(E, output, ekStringSafePtr(&error->line));
            ekStringConcat(E, output, "\n");

            // draw the carat!
            for (; caratPos > 0; --caratPos) {
                ekStringConcat(E, output, " "); // how about that efficiency! /s
            }
            ekStringConcat(E, output, "^\n");
        }
    }
    return (ekArraySize(E, &compiler->errors) > 0) ? ekTrue : ekFalse;
}

void ekCompileSyntaxError(ekCompiler * compiler, struct ekToken * token, const char * explanation)
{
    ekContext * E = compiler->E;
    const char * sourcePath = compiler->sourcePath;
    if (!sourcePath)
        sourcePath = "<source>";
    ekError * error = ekErrorCreate(E, sourcePath, token->line, compiler->source, token->text, explanation);
    ekArrayPush(E, &compiler->errors, error);
}

void ekCompileExplainError(ekCompiler * compiler, const char * explanation)
{
    ekS32 size = ekArraySize(compiler->E, &compiler->errors);
    if (size > 0) {
        ekError * error = compiler->errors[size - 1];
        ekStringSet(compiler->E, &error->explanation, explanation);
    }
}

// ---------------------------------------------------------------------------

static ekS32 ekCompileOptimizeChild(struct ekContext * E, ekCompiler * compiler, ekSyntax * syntax);

static void ekCompileOptimizeArray(struct ekContext * E, ekCompiler * compiler, ekSyntax ** a)
{
    ekS32 i;
    for (i = 0; i < ekArraySize(E, &a); i++) {
        ekCompileOptimizeChild(E, compiler, (ekSyntax *)a[i]);
    }
}

static ekS32 ekCompileOptimizeChild(struct ekContext * E, ekCompiler * compiler, ekSyntax * syntax)
{
    if (syntax->v.a) { ekCompileOptimizeArray(E, compiler, syntax->v.a); }
    if (syntax->v.p) { ekCompileOptimizeChild(E, compiler, syntax->v.p); }
    if (syntax->l.a) { ekCompileOptimizeArray(E, compiler, syntax->l.a); }
    if (syntax->l.p) { ekCompileOptimizeChild(E, compiler, syntax->l.p); }
    if (syntax->r.a) { ekCompileOptimizeArray(E, compiler, syntax->r.a); }
    if (syntax->r.p) { ekCompileOptimizeChild(E, compiler, syntax->r.p); }

    switch (syntax->type) {
        case EST_ADD:
        case EST_SUB:
        case EST_MUL:
        case EST_DIV:
            // Integer arithmetic optimization
            if ((syntax->l.p->type == EST_KINT)
                && (syntax->r.p->type == EST_KINT))
            {
                ekS32 val;
                switch (syntax->type) {
                    case EST_ADD:
                        val = syntax->l.p->v.i + syntax->r.p->v.i;
                        break;
                    case EST_SUB:
                        val = syntax->l.p->v.i - syntax->r.p->v.i;
                        break;
                    case EST_MUL:
                        val = syntax->l.p->v.i * syntax->r.p->v.i;
                        break;
                    case EST_DIV:
                        if (!syntax->r.p->v.i) { return 0; }
                        val = syntax->l.p->v.i / syntax->r.p->v.i;
                        break;
                }
                ekSyntaxDestroy(E, syntax->l.p);
                syntax->l.p = NULL;
                ekSyntaxDestroy(E, syntax->r.p);
                syntax->r.p = NULL;
                syntax->type = EST_KINT;
                syntax->v.i = val;
            }
            break;
    }

    return 0;
}

void ekCompileOptimize(struct ekContext * E, ekCompiler * compiler)
{
    ekCompileOptimizeChild(E, compiler, compiler->root);
}

// ---------------------------------------------------------------------------

enum
{
    ASM_NORMAL     = 0,
    ASM_LVALUE     = (1 << 0),
    ASM_VAR        = (1 << 1),
    ASM_VSET     = (1 << 2),
    ASM_LEAVE_LAST = (1 << 3)
};

#define asmFunc(NAME) \
    ekS32 ekAssemble ## NAME(struct ekContext * E, ekCompiler * compiler, ekCode * dst, ekSyntax * syntax, ekS32 keep, ekU32 flags)
typedef ekS32 (*ekAssembleFunc)(struct ekContext * E, ekCompiler * compiler, ekCode * dst, ekSyntax * syntax, ekS32 keep, ekU32 flags);

typedef struct ekAssembleInfo
{
    ekAssembleFunc assemble;
} ekAssembleInfo;

asmFunc(Nop);
asmFunc(KString);
asmFunc(KInt);
asmFunc(KFloat);
asmFunc(Bool);
asmFunc(Identifier);
asmFunc(Index);
asmFunc(StatementExpr);
asmFunc(StatementList);
asmFunc(ExpressionList);
asmFunc(IdentifierList);
asmFunc(Call);
asmFunc(Null);
asmFunc(This);
asmFunc(StringFormat);
asmFunc(Unary);
asmFunc(Binary);
asmFunc(ShortCircuit);
asmFunc(Var);
asmFunc(Break);
asmFunc(Return);
asmFunc(Assignment);
asmFunc(Inherits);
asmFunc(IfElse);
asmFunc(While);
asmFunc(For);
asmFunc(Function);
asmFunc(FunctionArgs);
asmFunc(With);
asmFunc(Scope);
asmFunc(Import);

static ekAssembleInfo asmDispatch[EST_COUNT] =
{
    { ekAssembleNop }, // EST_NOP

    { ekAssembleKString },    // EST_KSTRING
    { ekAssembleKInt },       // EST_KINT
    { ekAssembleKFloat },     // EST_KFLOAT
    { ekAssembleBool },       // EST_BOOL
    { ekAssembleIdentifier }, // EST_IDENTIFIER

    { ekAssembleIndex }, // EST_INDEX

    { ekAssembleStatementList },  // EST_STATEMENTLIST
    { ekAssembleExpressionList }, // EST_EXPRESSIONLIST
    { ekAssembleIdentifierList }, // EST_IDENTIFIERLIST

    { ekAssembleUnary }, // EST_ARRAY
    { ekAssembleUnary }, // EST_MAP

    { ekAssembleCall },         // EST_CALL
    { ekAssembleStringFormat }, // EST_STRINGFORMAT

    { ekAssembleNull }, // EST_NULL
    { ekAssembleThis }, // EST_THIS

    { ekAssembleUnary }, // EST_NOT

    { ekAssembleUnary },  // EST_BITWISE_NOT
    { ekAssembleBinary }, // EST_BITWISE_XOR
    { ekAssembleBinary }, // EST_BITWISE_AND
    { ekAssembleBinary }, // EST_BITWISE_OR
    { ekAssembleBinary }, // EST_SHIFTLEFT
    { ekAssembleBinary }, // EST_SHIFTRIGHT

    { ekAssembleBinary }, // EST_ADD
    { ekAssembleBinary }, // EST_SUB
    { ekAssembleBinary }, // EST_MUL
    { ekAssembleBinary }, // EST_DIV

    { ekAssembleShortCircuit }, // EST_AND
    { ekAssembleShortCircuit }, // EST_OR

    { ekAssembleBinary }, // EST_CMP
    { ekAssembleBinary }, // EST_EQUALS
    { ekAssembleBinary }, // EST_NOTEQUALS
    { ekAssembleBinary }, // EST_GREATERTHAN
    { ekAssembleBinary }, // EST_GREATERTHANOREQUAL
    { ekAssembleBinary }, // EST_LESSTHAN
    { ekAssembleBinary }, // EST_LESSTHANOREQUAL

    { ekAssembleStatementExpr }, // EST_STATEMENT_EXPR
    { ekAssembleAssignment },    // EST_ASSIGNMENT
    { ekAssembleInherits },      // EST_INHERITS
    { ekAssembleBreak },         // EST_BREAK
    { ekAssembleReturn },        // EST_RETURN

    { ekAssembleIfElse },       // EST_IFELSE
    { ekAssembleWhile },        // EST_WHILE
    { ekAssembleFor },          // EST_FOR
    { ekAssembleFunction },     // EST_FUNCTION
    { ekAssembleFunctionArgs }, // EST_FUNCTION_ARGS
    { ekAssembleScope } // EST_SCOPE
};

// This function ensures that what we're being asked to keep is what we offered
ekS32 asmPad(struct ekContext * E, ekCode * code, ekS32 keep, ekS32 offer, ekS32 line)
{
    if (keep == EAV_ALL_ARGS) {
        keep = offer;
    } else {
        if (offer > keep) {
            ekCodeGrow(E, code, 1);
            ekCodeAppend(E, code, EOP_POP, (ekOperand)(offer - keep), line);
        } else if (offer < keep) {
            ekS32 i;
            ekS32 nulls = keep - offer;
            ekCodeGrow(E, code, nulls);
            for (i = 0; i < nulls; i++) {
                ekCodeAppend(E, code, EOP_PUSHNULL, 0, line);
            }
        }
    }
    return keep;
}

#define PAD(OFFER) asmPad(E, dst, keep, OFFER, syntax->line)

asmFunc(Nop)
{
    printf("NOP detected: %d\n", syntax->type);
    return PAD(0);
}

asmFunc(KString)
{
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_PUSH_KS, ekArrayPushUniqueString(E, &compiler->chunk->kStrings, ekStrdup(E, syntax->v.s)), syntax->line);
    return PAD(1);
}

asmFunc(KInt)
{
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_PUSH_KI, ek32ArrayPushUnique(E, &compiler->chunk->kInts, (ekU32 *)&syntax->v.i), syntax->line);
    return PAD(1);
}

asmFunc(KFloat)
{
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_PUSH_KF, ek32ArrayPushUnique(E, &compiler->chunk->kFloats, (ekU32 *)&syntax->v.f), syntax->line);
    return PAD(1);
}

asmFunc(Bool)
{
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_PUSH_BOOL, syntax->v.i, syntax->line);
    return PAD(1);
}

asmFunc(Identifier)
{
    ekOpcode opcode = EOP_VVAL;
    if (flags & ASM_VAR) {
        opcode = EOP_VREG;
    } else if (flags & ASM_LVALUE) {
        opcode = EOP_VREF;
    }
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, opcode, ekArrayPushUniqueString(E, &compiler->chunk->kStrings, ekStrdup(E, syntax->v.s)), syntax->line);
    return PAD(1);
}

asmFunc(Index)
{
    ekS32 offerCount = 1;
    ekOperand opFlags = 0;
    ekBool pushThis = (syntax->v.i) ? ekTrue : ekFalse;
    ekSyntax * a = syntax->l.p;
    ekSyntax * b = syntax->r.p;
    asmDispatch[a->type].assemble(E, compiler, dst, a, 1, ASM_NORMAL);
    asmDispatch[b->type].assemble(E, compiler, dst, b, 1, ASM_NORMAL);
    if (flags & ASM_LVALUE) {
        opFlags |= EOF_LVALUE;
    }
    if (keep > 1) {
        opFlags |= (pushThis) ? EOF_PUSHTHIS : EOF_PUSHOBJ;
        offerCount++;
    }
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_INDEX, opFlags, syntax->line);
    return PAD(offerCount);
}

asmFunc(IdentifierList)
{
    ekSyntax ** args = syntax->v.a;
    ekS32 additionalFlags = (syntax->v.i) ? ASM_VAR : 0;
    ekS32 keepCount = 0;
    ekS32 i;

    for (i = ekArraySize(E, &args) - 1; i >= 0; i--) {
        ekSyntax * arg = (ekSyntax *)args[i];
        keepCount = asmDispatch[arg->type].assemble(E, compiler, dst, arg, 1, flags | additionalFlags);
        if (flags & ASM_VSET) {
            ekCodeGrow(E, dst, 1);
            ekCodeAppend(E, dst, EOP_VSET, ((flags & ASM_LEAVE_LAST) && !i) ? 1 : 0, syntax->line);
        }
    }
    return PAD(ekArraySize(E, &args));
}

asmFunc(Call)
{
    ekSyntax * func = syntax->v.p;
    ekSyntax * args = syntax->r.p;
    ekS32 argCount;
    ekS32 retCount;

    argCount = asmDispatch[args->type].assemble(E, compiler, dst, args, EAV_ALL_ARGS, ASM_NORMAL);
    retCount = asmDispatch[func->type].assemble(E, compiler, dst, func, 2, ASM_NORMAL); // requesting 2 to receive 'this' (even if padded with null)

    if (keep == EAV_ALL_ARGS) {
        keep = retCount;
    }

    ekCodeGrow(E, dst, 2);
    ekCodeAppend(E, dst, EOP_CALL, argCount, syntax->line);
    ekCodeAppend(E, dst, EOP_KEEP, keep, syntax->line);
    return PAD(keep);
}

asmFunc(Null)
{
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_PUSHNULL, 0, syntax->line);
    return PAD(1);
}

asmFunc(This)
{
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_PUSHTHIS, 0, syntax->line);
    return PAD(1);
}

asmFunc(StringFormat)
{
    ekSyntax * format = syntax->l.p;
    ekSyntax * args   = syntax->r.p;
    ekS32 argCount = asmDispatch[args->type].assemble(E, compiler, dst, args, EAV_ALL_ARGS, ASM_NORMAL);
    asmDispatch[format->type].assemble(E, compiler, dst, format, 1, ASM_NORMAL);
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_FORMAT, argCount, syntax->line);
    return PAD(1);
}

asmFunc(Unary)
{
    ekSyntax * expr = syntax->v.p;
    ekS32 keepCount = ((syntax->type == EST_ARRAY) || (syntax->type == EST_MAP)) ? EAV_ALL_ARGS : 1;
    ekS32 rvalueCount = 0;
    if (expr) {
        rvalueCount = asmDispatch[expr->type].assemble(E, compiler, dst, expr, keepCount, ASM_NORMAL);
    }
    ekCodeGrow(E, dst, 1);
    switch (syntax->type) {
        case EST_NOT:
            ekCodeAppend(E, dst, EOP_NOT, 0, syntax->line);
            break;
        case EST_BITWISE_NOT:
            ekCodeAppend(E, dst, EOP_BITWISE_NOT, 0, syntax->line);
            break;
        case EST_ARRAY:
            ekCodeAppend(E, dst, EOP_ARRAY, rvalueCount, syntax->line);
            break;
        case EST_MAP:
            ekCodeAppend(E, dst, EOP_MAP, rvalueCount, syntax->line);
            break;
    }
    return PAD(1);
}

asmFunc(Binary)
{
    ekSyntax * a = syntax->l.p;
    ekSyntax * b = syntax->r.p;
    ekBool compound = syntax->v.i;
    ekS32 op = EOP_NOP;
    ekS32 aflags = ASM_NORMAL;

    if (compound) {
        aflags = ASM_LVALUE;
    }

    asmDispatch[a->type].assemble(E, compiler, dst, a, 1, aflags);
    if (compound) {
        ekCodeGrow(E, dst, 2);
        ekCodeAppend(E, dst, EOP_DUPE, 0, syntax->line);
        ekCodeAppend(E, dst, EOP_REFVAL, 0, syntax->line);
    }
    asmDispatch[b->type].assemble(E, compiler, dst, b, 1, ASM_NORMAL);

    switch (syntax->type) {
        // Legal in either compound statements or expressions
        case EST_ADD:
            op = EOP_ADD;
            break;
        case EST_SUB:
            op = EOP_SUB;
            break;
        case EST_MUL:
            op = EOP_MUL;
            break;
        case EST_DIV:
            op = EOP_DIV;
            break;
        case EST_BITWISE_XOR:
            op = EOP_BITWISE_XOR;
            break;
        case EST_BITWISE_AND:
            op = EOP_BITWISE_AND;
            break;
        case EST_BITWISE_OR:
            op = EOP_BITWISE_OR;
            break;
        case EST_SHIFTLEFT:
            op = EOP_SHIFTLEFT;
            break;
        case EST_SHIFTRIGHT:
            op = EOP_SHIFTRIGHT;
            break;

        // Legal in expressions only
        case EST_CMP:
            op = EOP_CMP;
            break;
        case EST_EQUALS:
            op = EOP_EQUALS;
            break;
        case EST_NOTEQUALS:
            op = EOP_NOTEQUALS;
            break;
        case EST_GREATERTHAN:
            op = EOP_GREATERTHAN;
            break;
        case EST_GREATERTHANOREQUAL:
            op = EOP_GREATERTHANOREQUAL;
            break;
        case EST_LESSTHAN:
            op = EOP_LESSTHAN;
            break;
        case EST_LESSTHANOREQUAL:
            op = EOP_LESSTHANOREQUAL;
            break;
    }

    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, op, 0, syntax->line);

    if (compound) {
        ekCodeGrow(E, dst, 2);
        ekCodeAppend(E, dst, EOP_MOVE, 1, syntax->line);
        ekCodeAppend(E, dst, EOP_VSET, 0, syntax->line);
        return PAD(0);
    }
    return PAD(1);
}

asmFunc(ShortCircuit)
{
    ekS32 skipIndex;
    ekSyntax * a = syntax->l.p;
    ekSyntax * b = syntax->r.p;

    // code to add first expression on the stack
    asmDispatch[a->type].assemble(E, compiler, dst, a, 1, ASM_NORMAL);

    // test stack top's truth and uses the current value instead of the second expression's value
    ekCodeGrow(E, dst, 1);
    skipIndex = ekCodeAppend(E, dst, (syntax->type == EST_AND) ? EOP_AND : EOP_OR, 0, syntax->line);

    // code to add second expression on the stack
    asmDispatch[b->type].assemble(E, compiler, dst, b, 1, ASM_NORMAL);

    // clue in the and/or opcode how far to optionally jump
    dst->ops[skipIndex].operand = dst->count - skipIndex - 1;

    return PAD(1);
}

asmFunc(Break)
{
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_BREAK, 0, syntax->line);
    return PAD(0);
}

asmFunc(Return)
{
    ekSyntax * expr = syntax->v.p;
    ekS32 retCount = 0;
    if (expr)
        retCount = asmDispatch[expr->type].assemble(E, compiler, dst, expr, EAV_ALL_ARGS, ASM_NORMAL);
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_RET, retCount, syntax->line);
    return PAD(0);
}

asmFunc(Assignment)
{
    ekS32 i;
    ekSyntax * l = syntax->l.p;
    ekSyntax * r = syntax->r.p;
    ekS32 leave = (keep > 0) ? 1 : 0;
    ekS32 lflags = ASM_LVALUE | ASM_VSET | ((leave) ? ASM_LEAVE_LAST : 0);
    ekCode * lvalueCode = ekCodeCreate();
    ekS32 lvalueCount = asmDispatch[l->type].assemble(E, compiler, lvalueCode, l, EAV_ALL_ARGS, lflags);
    asmDispatch[r->type].assemble(E, compiler, dst, r, lvalueCount, ASM_NORMAL);
    ekCodeConcat(E, dst, lvalueCode);
    ekCodeDestroy(E, lvalueCode);
    if ((l->type != EST_IDENTIFIERLIST) && (l->type != EST_EXPRESSIONLIST)) {
        ekCodeGrow(E, dst, 1);
        ekCodeAppend(E, dst, EOP_VSET, leave, syntax->line);
    }
    return PAD(leave);
}

asmFunc(Inherits)
{
    ekSyntax * l = syntax->l.p;
    ekSyntax * r = syntax->r.p;

    asmDispatch[r->type].assemble(E, compiler, dst, r, 1, ASM_NORMAL);
    asmDispatch[l->type].assemble(E, compiler, dst, l, 1, ASM_NORMAL);
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_INHERITS, 0, syntax->line);
    return PAD(1);
}

asmFunc(IfElse)
{
    ekBool ternary  = syntax->v.i;
    ekSyntax * cond     = syntax->v.p;
    ekSyntax * ifBody   = syntax->l.p;
    ekSyntax * elseBody = syntax->r.p;
    ekCode * ifCode = ekCodeCreate();
    ekS32 index;
    ekS32 i;
    ekS32 valuesLeftOnStack = (ternary) ? 1 : 0;

    if (elseBody) {
        ekCode * elseCode = ekCodeCreate();
        asmDispatch[elseBody->type].assemble(E, compiler, elseCode, elseBody, valuesLeftOnStack, ASM_NORMAL);
        ekCodeGrow(E, elseCode, 1);
        ekCodeAppend(E, elseCode, EOP_LEAVE, 0, syntax->line);
        index = ekBlockConvertCode(E, elseCode, compiler->chunk, 0);
        ekCodeGrow(E, dst, 1);
        ekCodeAppend(E, dst, EOP_PUSH_KB, index, syntax->line);
    }

    asmDispatch[ifBody->type].assemble(E, compiler, ifCode, ifBody, valuesLeftOnStack, ASM_NORMAL);
    ekCodeGrow(E, ifCode, 1);
    ekCodeAppend(E, ifCode, EOP_LEAVE, 0, syntax->line);
    index = ekBlockConvertCode(E, ifCode, compiler->chunk, 0);
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_PUSH_KB, index, syntax->line);

    // Only keeps the value of the first expression on the stack for bool testing
    asmDispatch[cond->type].assemble(E, compiler, dst, cond, 1, ASM_NORMAL);

    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_IF, (elseBody) ? 1 : 0, syntax->line);

    return PAD(valuesLeftOnStack);
}

asmFunc(While)
{
    ekSyntax * cond = syntax->v.p;
    ekSyntax * body = syntax->r.p;
    ekCode * loop = ekCodeCreate();
    ekS32 index;

    ekCodeGrow(E, loop, 1);
    ekCodeAppend(E, loop, EOP_START, 0, syntax->line);

    asmDispatch[cond->type].assemble(E, compiler, loop, cond, 1, ASM_NORMAL);
    ekCodeGrow(E, loop, 1);
    ekCodeAppend(E, loop, EOP_LEAVE, 1, syntax->line);

    asmDispatch[body->type].assemble(E, compiler, loop, body, 0, ASM_NORMAL);

    ekCodeGrow(E, loop, 1);
    ekCodeAppend(E, loop, EOP_CONTINUE, 0, syntax->line);

    index = ekBlockConvertCode(E, loop, compiler->chunk, 0);

    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_PUSH_KB, index, syntax->line);
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_ENTER, EFT_LOOP, syntax->line);

    return PAD(0);
}

asmFunc(For)
{
    ekSyntax * vars = syntax->v.p;
    ekSyntax * iter = syntax->l.p;
    ekSyntax * body = syntax->r.p;
    ekCode * loop = ekCodeCreate();
    ekS32 loopVarCount = ekArraySize(E, &vars->v.a);
    ekS32 index = 0;

    // Get the iterator onto the stack
    asmDispatch[iter->type].assemble(E, compiler, dst, iter, 1, ASM_NORMAL);
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_ITER, index, syntax->line); // Give the type a chance to turn itself into an iterator

    ekCodeGrow(E, loop, 8);
    ekCodeAppend(E, loop, EOP_CLEANUP, 1 + loopVarCount, syntax->line); // Mark the iterator and args as something to clean up
    ekCodeAppend(E, loop, EOP_START, 0, syntax->line);                  // begin loop
    ekCodeAppend(E, loop, EOP_DUPE, 0, syntax->line);                   // dupe the iterator
    ekCodeAppend(E, loop, EOP_PUSHNULL, 0, syntax->line);               // 'this' (null in an interator)
    ekCodeAppend(E, loop, EOP_CALL, 0, syntax->line);                   // Call iterator
    ekCodeAppend(E, loop, EOP_KEEP, loopVarCount, syntax->line);        // keep enough values to populate loop vars
    ekCodeAppend(E, loop, EOP_DUPE, loopVarCount - 1, syntax->line);    // dupe first parameter
    ekCodeAppend(E, loop, EOP_LEAVE, 2, syntax->line);                  // leave if top of stack is null

    // Populate loop vars
    asmDispatch[vars->type].assemble(E, compiler, loop, vars, ekArraySize(E, &vars->v.a), ASM_LVALUE | ASM_VAR | ASM_VSET);

    // Assemble the loop body itself
    asmDispatch[body->type].assemble(E, compiler, loop, body, 0, ASM_NORMAL);

    ekCodeGrow(E, loop, 1);
    ekCodeAppend(E, loop, EOP_CONTINUE, 0, syntax->line);

    index = ekBlockConvertCode(E, loop, compiler->chunk, 0);

    ekCodeGrow(E, dst, 2);
    ekCodeAppend(E, dst, EOP_PUSH_KB, index, syntax->line);
    ekCodeAppend(E, dst, EOP_ENTER, EFT_LOOP, syntax->line);

    return PAD(0);
}

asmFunc(Function)
{
    ekSyntax * args = syntax->l.p;
    ekSyntax * body = syntax->r.p;
    char * name = syntax->v.s;
    ekCode * code = ekCodeCreate();
    ekOperand index;

    // If a name is used ("func name()" vs "func()"), the
    // generated code will consume the block left on the stack
    // with a vset, effectively compiling "func name()" into
    // "var name = func()".
    ekS32 additionalOpsForNaming = 0;
    ekS32 valuesLeftOnStack = 1;

    ekS32 argCount = asmDispatch[args->type].assemble(E, compiler, code, args, EAV_ALL_ARGS, ASM_VAR | ASM_LVALUE | ASM_VSET);
    asmDispatch[body->type].assemble(E, compiler, code, body, 0, ASM_NORMAL);
    ekCodeGrow(E, code, 1);
    ekCodeAppend(E, code, EOP_RET, 0, syntax->line);
    index = ekBlockConvertCode(E, code, compiler->chunk, argCount);

    if (name) {
        additionalOpsForNaming = 2;
        valuesLeftOnStack = 0;
    }
    ekCodeGrow(E, dst, 2 + additionalOpsForNaming);
    ekCodeAppend(E, dst, EOP_PUSH_KB, index, syntax->line); // Push the new block on the stack
    ekCodeAppend(E, dst, EOP_CLOSE, 0, syntax->line);       // Give the VM an opportunity to pepper it with closure data
    if (name) {
        ekCodeAppend(E, dst, EOP_VREG, ekArrayPushUniqueString(E, &compiler->chunk->kStrings, ekStrdup(E, name)), syntax->line);
        ekCodeAppend(E, dst, EOP_VSET, 0, syntax->line);
    }
    compiler->chunk->hasFuncs = ekTrue;
    return PAD(valuesLeftOnStack);
}

asmFunc(FunctionArgs)
{
    ekS32 argCount;
    ekS32 varargsOpIndex = 0;
    ekSyntax * args = syntax->l.p;
    const char * varargsName = syntax->v.s;
    if (varargsName) {
        ekCodeGrow(E, dst, 3);
        varargsOpIndex = ekCodeAppend(E, dst, EOP_VARARGS, 0, args->line);
        ekCodeAppend(E, dst, EOP_VREG, ekArrayPushUniqueString(E, &compiler->chunk->kStrings, ekStrdup(E, varargsName)), syntax->line);
        ekCodeAppend(E, dst, EOP_VSET, 0, syntax->line);
    }

    argCount = asmDispatch[args->type].assemble(E, compiler, dst, args, keep, flags);

    if (varargsName) {
        dst->ops[varargsOpIndex].operand = argCount;
        return EAV_ALL_ARGS;
    }
    return argCount;
}

asmFunc(Scope)
{
    ekSyntax * body = syntax->v.p;
    ekCode * code = ekCodeCreate();
    ekS32 index;

    // create new block from "body"
    asmDispatch[body->type].assemble(E, compiler, code, body, 0, ASM_NORMAL);
    ekCodeGrow(E, code, 1);
    ekCodeAppend(E, code, EOP_LEAVE, 0, syntax->line);
    index = ekBlockConvertCode(E, code, compiler->chunk, 0);

    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_PUSH_KB, index, syntax->line);
    ekCodeGrow(E, dst, 1);
    ekCodeAppend(E, dst, EOP_ENTER, EFT_SCOPE, syntax->line);

    return PAD(0);
}

asmFunc(ExpressionList)
{
    ekS32 i = 0;
    ekS32 last = ekArraySize(E, &syntax->v.a);
    ekS32 increment = 1;
    ekS32 keepCount = 0;
    ekS32 reverseOrder = (flags & ASM_VSET); // values must be VSET'd by popping off the stack in reverse
    for (i = 0; i < ekArraySize(E, &syntax->v.a); i++) {
        ekS32 index = (reverseOrder) ? (ekArraySize(E, &syntax->v.a) - 1) - i : i;
        ekS32 keepIt = ((keep == EAV_ALL_ARGS) || (i < keep)) ? 1 : 0; // keep one from each expr, dump the rest
        ekSyntax * child = syntax->v.a[index];
        keepCount += asmDispatch[child->type].assemble(E, compiler, dst, child, keepIt, flags & ~ASM_VSET);
        if (flags & ASM_VSET) {
            ekCodeGrow(E, dst, 1);
            ekCodeAppend(E, dst, EOP_VSET, ((flags & ASM_LEAVE_LAST) && (i != ekArraySize(E, &syntax->v.a) - 1)) ? 1 : 0, syntax->line);
        }
    }
    return PAD(keepCount);
}

asmFunc(StatementExpr)
{
    ekSyntax * child = syntax->v.p;
    ekS32 keepCount = asmDispatch[child->type].assemble(E, compiler, dst, child, keep, ASM_NORMAL);
    return PAD(keepCount);
}

asmFunc(StatementList)
{
    ekS32 keepCount = 0;
    ekS32 i;
    for (i = 0; i < ekArraySize(E, &syntax->v.a); i++) {
        ekSyntax * child = syntax->v.a[i];
        keepCount = asmDispatch[child->type].assemble(E, compiler, dst, child, 0, ASM_NORMAL);
    }
    return PAD(keepCount);
}

ekBool ekAssemble(struct ekContext * E, ekCompiler * compiler, ekBool useSourcePathForImports)
{
    if (compiler->root && (compiler->root->type == EST_STATEMENTLIST)) {
        ekS32 blockIndex;
        compiler->chunk = ekChunkCreate(E, compiler->sourcePath, useSourcePathForImports);
        compiler->code   = ekCodeCreate();
        ekAssembleStatementList(E, compiler, compiler->code, compiler->root, 0, ASM_NORMAL);
        ekCodeGrow(E, compiler->code, 1);
        ekCodeAppend(E, compiler->code, EOP_RET, 0, 0);
        blockIndex = ekBlockConvertCode(E, compiler->code, compiler->chunk, 0);
        compiler->chunk->block = compiler->chunk->blocks[blockIndex];
    }
    return ekTrue;
}
