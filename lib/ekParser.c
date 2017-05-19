// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekParser.h"

#include "ekCompiler.h"
#include "ekContext.h"
#include "ekLexer.h"
#include "ekString.h"
#include "ekSyntax.h"

#include <stdio.h>

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

ekParser * ekParserCreate(struct ekContext * E)
{
    ekParser * parser = (ekParser *)ekAlloc(sizeof(ekParser));
    return parser;
}

void ekParserDestroy(struct ekContext * E, ekParser * parser)
{
    ekArrayDestroy(E, &parser->errors, (ekDestroyCB)ekErrorDestroy);
    ekFree(parser);
}

void ekParserSyntaxError(struct ekContext * E, ekParser * parser, struct ekToken * token, const char * explanation)
{
    const char * sourcePath = parser->sourcePath;
    if (!sourcePath)
        sourcePath = "<source>";
    ekError * error = ekErrorCreate(E, sourcePath, token->line, parser->source, token->text, explanation);
    ekArrayPush(E, &parser->errors, error);
}

// ---------------------------------------------------------------------------

typedef struct ekBinOp
{
    ekSyntaxType syntaxType;
    int precedence;
} ekBinOp;

static ekBinOp ekTokenTypeToBinOp(ekTokenType tokenType)
{
    ekBinOp binOp;
    switch (tokenType) {
        case ETT_ASSIGN: binOp.syntaxType = EST_ASSIGNMENT; binOp.precedence = 1 << 16; break;
        case ETT_AND:    binOp.syntaxType = EST_AND;        binOp.precedence = 2 << 16; break;
        case ETT_OR:     binOp.syntaxType = EST_OR;         binOp.precedence = 2 << 16; break;
        case ETT_PLUS:   binOp.syntaxType = EST_ADD;        binOp.precedence = 3 << 16; break;
        case ETT_DASH:   binOp.syntaxType = EST_SUB;        binOp.precedence = 3 << 16; break;
        case ETT_STAR:   binOp.syntaxType = EST_MUL;        binOp.precedence = 4 << 16; break;
        case ETT_SLASH:  binOp.syntaxType = EST_DIV;        binOp.precedence = 4 << 16; break;
        default:         binOp.syntaxType = EST_NOP;        binOp.precedence = -1;      break;
    }
    return binOp;
}

static ekSyntaxType ekTokenTypeToUnaryOp(ekTokenType tokenType)
{
    switch (tokenType) {
        case ETT_NOT:         return EST_NOT;
        case ETT_BITWISE_NOT: return EST_BITWISE_NOT;
        default:
            break;
    }
    ekAssert(0); // Someone forgot to add a unary op to this table!
    return EST_ADD;
}

// ---------------------------------------------------------------------------

static ekSyntax * ekParserParseChunk(struct ekContext * E, struct ekParser * parser);
static ekSyntax * ekParserParseStatementList(struct ekContext * E, struct ekParser * parser);
static ekSyntax * ekParserParseStatement(struct ekContext * E, struct ekParser * parser);
static ekSyntax * ekParserParseVar(struct ekContext * E, struct ekParser * parser);
static ekSyntax * ekParserParseExpression(struct ekContext * E, struct ekParser * parser);
static ekSyntax * ekParserParseExpressionRHS(struct ekContext * E, struct ekParser * parser, int exprPrecedence, struct ekSyntax * lhs);
static ekSyntax * ekParserParseUnary(struct ekContext * E, struct ekParser * parser);
static ekSyntax * ekParserParseParenExpressionList(struct ekContext * E, struct ekParser * parser);
static ekSyntax * ekParserParseExpressionList(struct ekContext * E, struct ekParser * parser);
static ekSyntax * ekParserParseParenIdentifierList(struct ekContext * E, struct ekParser * parser);
static ekSyntax * ekParserParseIdentifierList(struct ekContext * E, struct ekParser * parser);
static ekSyntax * ekParserParsePrimary(struct ekContext * E, struct ekParser * parser);
static ekSyntax * ekParserParseLValue(struct ekContext * E, struct ekParser * parser);

#define CONSUME() ekLexerConsume(E, parser->lexer, &parser->token)

struct ekSyntax * ekParserParse(struct ekContext * E, struct ekParser * parser, const char * sourcePath, const char * source)
{
    struct ekSyntax * root = NULL;
    parser->sourcePath = sourcePath;
    parser->source = source;
    parser->lexer = ekLexerCreate(E, source);

    // while (CONSUME() != ETT_EOF) {
    //     printf("Token: %s\n", ekTokenTypeName(parser->token.type));
    // }

    if (CONSUME() != ETT_EOF) {
        root = ekParserParseChunk(E, parser);
    }

    ekLexerDestroy(E, parser->lexer);
    parser->lexer = NULL;
    parser->sourcePath = NULL;
    parser->source = NULL;
    return root;
}

// chunk ::= statement_list.
static ekSyntax * ekParserParseChunk(struct ekContext * E, struct ekParser * parser)
{
    return ekParserParseStatementList(E, parser);
}

// statement_list ::= statement.
// statement_list ::= statement_list statement.
static ekSyntax * ekParserParseStatementList(struct ekContext * E, struct ekParser * parser)
{
    ekSyntax * statementList = ekSyntaxCreateList(E, EST_STATEMENTLIST, NULL);
    while (parser->token.type != ETT_EOF) {
        if (parser->token.type == ETT_ENDSTATEMENT) {
            CONSUME();
            continue;
        }
        ekSyntax * statement = ekParserParseStatement(E, parser);
        if (!statement) {
            ekSyntaxDestroy(E, statementList);
            return NULL;
        }
        ekSyntaxListAppend(E, statementList, statement, 0);
    }
    return statementList;
}

static ekSyntax * ekParserParseStatement(struct ekContext * E, struct ekParser * parser)
{
    return ekParserParseExpressionList(E, parser);
}

static ekSyntax * ekParserParseVar(struct ekContext * E, struct ekParser * parser)
{
    ekSyntax *idents = NULL;
    if (parser->token.type != ETT_VAR) {
        ekParserSyntaxError(E, parser, &parser->token, "var expected");
        return NULL;
    }
    CONSUME(); // eat 'var'

    switch(parser->token.type) {
        case ETT_LEFTPAREN:
            idents = ekParserParseParenIdentifierList(E, parser);
            break;
        case ETT_IDENTIFIER:
            idents = ekParserParseIdentifierList(E, parser);
            break;
        default:
            ekParserSyntaxError(E, parser, &parser->token, "var statements require an identifier list");
            return NULL;
    }
    if(!idents) {
        return NULL;
    }
    return ekSyntaxMarkVar(E, idents);
}

static ekSyntax * ekParserParseExpression(struct ekContext * E, struct ekParser * parser)
{
    ekSyntax * lhs = ekParserParseUnary(E, parser);
    if (lhs) {
        return ekParserParseExpressionRHS(E, parser, 0, lhs);
    }
    return NULL;
}

static ekSyntax * ekParserParseExpressionRHS(struct ekContext * E, struct ekParser * parser, int exprPrecedence, struct ekSyntax * lhs)
{
    ekBinOp op, nextOp;
    ekSyntax * rhs;
    while (1) {
        ekBinOp op = ekTokenTypeToBinOp(parser->token.type);
        if (op.precedence < exprPrecedence) {
            return lhs;
        }

        CONSUME();
        rhs = ekParserParseUnary(E, parser);
        if (!rhs) {
            ekSyntaxDestroy(E, lhs);
            return NULL;
        }

        nextOp = ekTokenTypeToBinOp(parser->token.type);
        if (op.precedence < nextOp.precedence) {
            rhs = ekParserParseExpressionRHS(E, parser, op.precedence + 1 /* ehhh */, rhs);
            if (!rhs) {
                ekSyntaxDestroy(E, lhs);
                return NULL;
            }
        }

        if (op.syntaxType == EST_ASSIGNMENT) {
            lhs = ekSyntaxCreateAssignment(E, lhs, rhs);
        } else {
            lhs = ekSyntaxCreateBinary(E, op.syntaxType, lhs, rhs, ekFalse);
        }
    }
    return lhs;
}

static ekSyntax * ekParserParseUnary(struct ekContext * E, struct ekParser * parser)
{
    switch (parser->token.type) {
        case ETT_NOT:
        case ETT_BITWISE_NOT:
        {
            ekSyntaxType type = (parser->token.type == ETT_NOT) ? EST_NOT : EST_BITWISE_NOT;
            CONSUME();
            ekSyntax * expr = ekParserParseUnary(E, parser);
            if (expr) {
                return ekSyntaxCreateUnary(E, type, expr, expr->line);
            }
            break;
        }
        default:
            return ekParserParsePrimary(E, parser);
    }
    return NULL;
}

static ekSyntax * ekParserParseParenExpressionList(struct ekContext * E, struct ekParser * parser)
{
    ekSyntax * expr = NULL;
    if (parser->token.type != ETT_LEFTPAREN) {
        ekParserSyntaxError(E, parser, &parser->token, "left paren expected");
        return NULL;
    }
    CONSUME(); // eat '('
    if (parser->token.type == ETT_RIGHTPAREN) {
        CONSUME(); // eat ')'
        return ekSyntaxCreateList(E, EST_EXPRESSIONLIST, NULL);
    }

    expr = ekParserParseExpressionList(E, parser);
    if (!expr) {
        return NULL;
    }
    if (parser->token.type != ETT_RIGHTPAREN) {
        ekSyntaxDestroy(E, expr);
        ekParserSyntaxError(E, parser, &parser->token, "missing ')'");
        return NULL;
    }
    CONSUME(); // eat ')'
    return expr;
}

static ekSyntax * ekParserParseExpressionList(struct ekContext * E, struct ekParser * parser)
{
    ekSyntax * expressionList = ekSyntaxCreateList(E, EST_EXPRESSIONLIST, NULL);
    while (1) {
        ekSyntax * expr = ekParserParseExpression(E, parser);
        if (!expr) {
            ekSyntaxDestroy(E, expressionList);
            return NULL;
        }
        ekSyntaxListAppend(E, expressionList, expr, 0);
        if (parser->token.type == ETT_COMMA) {
            CONSUME();
        } else {
            break;
        }
    }
    return expressionList;
}

static ekSyntax * ekParserParseParenIdentifierList(struct ekContext * E, struct ekParser * parser)
{
    ekSyntax * expr = NULL;
    if (parser->token.type != ETT_LEFTPAREN) {
        ekParserSyntaxError(E, parser, &parser->token, "'(' expected");
        return NULL;
    }
    CONSUME(); // eat '('
    expr = ekParserParseIdentifierList(E, parser);
    if (!expr) {
        return NULL;
    }
    if (parser->token.type != ETT_RIGHTPAREN) {
        ekSyntaxDestroy(E, expr);
        ekParserSyntaxError(E, parser, &parser->token, "missing ')'");
        return NULL;
    }
    CONSUME(); // eat ')'
    return expr;
}

static ekSyntax * ekParserParseIdentifierList(struct ekContext * E, struct ekParser * parser)
{
    ekSyntax * identifierList = ekSyntaxCreateList(E, EST_IDENTIFIERLIST, NULL);
    while (1) {
        if (parser->token.type != ETT_IDENTIFIER) {
            ekSyntaxDestroy(E, identifierList);
            ekParserSyntaxError(E, parser, &parser->token, "Expecting identifier");
            return NULL;
        }
        ekSyntaxListAppend(E, identifierList, ekSyntaxCreateIdentifier(E, &parser->token), 0);
        CONSUME(); // eat ident
        if (parser->token.type == ETT_COMMA) {
            CONSUME();
        } else {
            break;
        }
    }
    return identifierList;
}

static ekSyntax * ekParserParsePrimary(struct ekContext * E, struct ekParser * parser)
{
    ekSyntax * primary = NULL;
    switch (parser->token.type) {
        case ETT_IDENTIFIER:
        case ETT_THIS:
            return ekParserParseLValue(E, parser);
        case ETT_LITERALSTRING:
            primary = ekSyntaxCreateKString(E, &parser->token, ekFalse);
            break;
        case ETT_REGEXSTRING:
            primary = ekSyntaxCreateKString(E, &parser->token, ekTrue);
            break;
        case ETT_DASH:
            // Sneaking in a unary here! Naughty!
            CONSUME(); // eat '-'
            switch (parser->token.type) {
                case ETT_FLOATNUM:
                    primary = ekSyntaxCreateKFloat(E, &parser->token, CKO_NEGATIVE);
                    break;
                case ETT_INTEGER:
                    primary = ekSyntaxCreateKInt(E, &parser->token, CKO_NEGATIVE);
                    break;
                default:
                    // Not sure if I want to support things like this:   -(5 * 3)
                    ekParserSyntaxError(E, parser, &parser->token, "Expected number");
                    break;
            }
            break;
        case ETT_VAR:
            return ekParserParseVar(E, parser);
        case ETT_FLOATNUM:
            primary = ekSyntaxCreateKFloat(E, &parser->token, 0);
            break;
        case ETT_INTEGER:
            primary = ekSyntaxCreateKInt(E, &parser->token, 0);
            break;
        case ETT_TRUE:
            primary = ekSyntaxCreateBool(E, &parser->token, ekTrue);
            break;
        case ETT_FALSE:
            primary = ekSyntaxCreateBool(E, &parser->token, ekFalse);
            break;
        case ETT_NULL:
            primary = ekSyntaxCreateNull(E, parser->token.line);
            break;
        case ETT_LEFTPAREN:
            primary = ekParserParseParenExpressionList(E, parser);
            if (!primary) {
                return NULL;
            }
            break;
        default:
            ekParserSyntaxError(E, parser, &parser->token, "Expected expression");
            break;
    }
    CONSUME(); // eat primary token or ')'
    return primary;
}

static ekSyntax * ekParserParseLValue(struct ekContext * E, struct ekParser * parser)
{
    ekSyntax * lvalue = NULL;
    switch (parser->token.type) {
        case ETT_IDENTIFIER:
            lvalue = ekSyntaxCreateIdentifier(E, &parser->token);
            CONSUME();
            break;
        case ETT_THIS:
            lvalue = ekSyntaxCreateThis(E, parser->token.line);
            CONSUME();
            break;
        default:
            ekParserSyntaxError(E, parser, &parser->token, "Expected lvalue");
            return NULL;
    }
    ekAssert(lvalue);

    while (1) {
        ekBool pushThis;
        ekSyntax * expr;
        switch (parser->token.type) {
            case ETT_PERIOD:
            case ETT_COLONCOLON:
                pushThis = (parser->token.type == ETT_COLONCOLON) ? ekTrue : ekFalse;
                CONSUME();
                if (parser->token.type != ETT_IDENTIFIER) {
                    ekSyntaxDestroy(E, lvalue);
                    ekParserSyntaxError(E, parser, &parser->token, "property lookup requires an identifier");
                    return NULL;
                }
                lvalue = ekSyntaxCreateIndex(E, lvalue, ekSyntaxCreateKString(E, &parser->token, 0), pushThis);
                CONSUME();
                break;
            case ETT_OPENBRACKET:
                CONSUME();
                expr = ekParserParsePrimary(E, parser);
                if (!expr) {
                    ekSyntaxDestroy(E, lvalue);
                    ekParserSyntaxError(E, parser, &parser->token, "index requires an expression");
                    return NULL;
                }
                if (parser->token.type != ETT_CLOSEBRACKET) {
                    ekSyntaxDestroy(E, lvalue);
                    ekParserSyntaxError(E, parser, &parser->token, "index missing ']'");
                    return NULL;
                }
                lvalue = ekSyntaxCreateIndex(E, lvalue, expr, ekFalse);
                CONSUME();
                break;
            case ETT_LEFTPAREN:
                expr = ekParserParseParenExpressionList(E, parser);
                if (!expr) {
                    ekSyntaxDestroy(E, lvalue);
                    return NULL;
                }
                lvalue = ekSyntaxCreateCall(E, lvalue, expr);
                break;
            default:
                return lvalue;
        }
    }
    return lvalue;
}
