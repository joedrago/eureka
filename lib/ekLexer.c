// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekLexer.h"
#include "ekContext.h"

#include "ekCompiler.h"
#include "ekParser.h"
#include "ekSyntax.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ekS32 getNextToken(ekLexer * l);

ekLexer * ekLexerCreate(struct ekContext * E, const char * source)
{
    ekLexer * lexer = (ekLexer *)ekAlloc(sizeof(ekLexer));
    lexer->text   = source;
    lexer->end    = lexer->text + strlen(lexer->text);
    lexer->cur    = lexer->text;
    lexer->token  = lexer->cur;
    lexer->line   = 1;
    return lexer;
}

void ekLexerDestroy(struct ekContext * E, ekLexer * lexer)
{
    ekFree(lexer);
}

ekTokenType ekLexerNext(struct ekContext * E, ekLexer * lexer, struct ekToken * token)
{
    ekS32 id;
    ekS32 token_len;
    ekS32 tokenMax;

    while ((id = getNextToken(lexer)) != ETT_EOF) {
        token_len = (ekS32)(lexer->cur - lexer->token);

        // Normal C style lexers treat semi-colons as end statements
        // and braces as block delimiters. Newlines are just whitespace.

        switch (id) {
            case ETT_HEREDOC:
                // Heredocs are just trimmed literal strings
                id = ETT_LITERALSTRING;
                ekAssert(token_len >= 6);

                // remove triple quotes
                token_len -= 6;
                lexer->token += 3;
                break;
            case ETT_NEWLINE:
                id = ETT_SPACE;
                break;
            case ETT_SEMI:
                id = ETT_ENDSTATEMENT;
                break;
            case ETT_OPENBRACE:
                id = ETT_STARTBLOCK;
                break;
            case ETT_CLOSEBRACE:
                id = ETT_ENDBLOCK;
                break;
        }

        if ((id != ETT_SPACE) && (id != ETT_COMMENT)) {
            if (token_len > 0) {
                token->type = id;
                token->text = lexer->token;
                token->len  = token_len;
                token->line = lexer->line;

                lexer->token = lexer->cur;
                return id;
            }
        }
        lexer->token = lexer->cur;
    }

    memset(token, 0, sizeof(ekToken));
    token->type = ETT_EOF;
    return ETT_EOF;
}

#if 0
ekBool ekLex(void * parser, const char * text, tokenCB cb, struct ekCompiler * compiler)
{
    struct ekContext * E = compiler->E;
    ekS32 id;
    ekS32 token_len;
    ekS32 tokenMax;
    ekToken token;
    ekLexer l = { 0 };

    l.text   = text;
    l.end    = l.text + strlen(l.text);
    l.cur    = l.text;
    l.token  = l.cur;
    l.line   = 1;

    while ((id = getNextToken(&l)) != ETT_EOF) {
        if (ekArraySize(E, &compiler->errors)) {
            break;
        }

        token_len = (ekS32)(l.cur - l.token);

        // Normal C style lexers treat semi-colons as end statements
        // and braces as block delimiters. Newlines are just whitespace.

        switch (id) {
            case ETT_HEREDOC:
                // Heredocs are just trimmed literal strings
                id = ETT_LITERALSTRING;
                ekAssert(token_len >= 6);

                // remove triple quotes
                token_len -= 6;
                l.token += 3;
                break;
            case ETT_NEWLINE:
                id = ETT_SPACE;
                break;
            case ETT_SEMI:
                id = ETT_ENDSTATEMENT;
                break;
            case ETT_OPENBRACE:
                id = ETT_STARTBLOCK;
                break;
            case ETT_CLOSEBRACE:
                id = ETT_ENDBLOCK;
                break;
        }

        if ((id != ETT_SPACE) && (id != ETT_COMMENT)) {
            if (token_len > 0) {
                token.text = l.token;
                token.len  = token_len;
                token.line = l.line;

                CALL_CB(parser, id, token, compiler);
            }
        }

        l.token = l.cur;
    }

    if (!ekArraySize(E, &compiler->errors)) {
        token.line = l.line;
        CALL_CB(parser, ETT_ENDSTATEMENT, token, compiler);
    }
    return ekTrue;
}
#endif /* if 0 */

ekToken * ekTokenClone(struct ekContext * E, ekToken * token)
{
    ekToken * ret = ekTokenCreate();
    *ret = *token;
    return ret;
}

char * ekTokenToString(struct ekContext * E, ekToken * t, int isRegex)
{
    const char * src = t->text;
    const char * end = src + t->len;
    char * str = ekAlloc(t->len + 1);
    char * dst = str;
    ekBool escaped = ekFalse;

    if (isRegex) { // Completely unescaped. Backslashes are passed directly as-is to PCRE
        int len;

        if (t->len > 1) {
            if (*src == 'r') {
                ++src;
                if (*src == '"') {
                    src++;
                }
                if (*(end - 1) == '"') {
                    end--;
                }
            } else if (*src == '@') {
                src++;
            }
            if (*(end - 1) == '@') {
                end--;
            }
        }

        len = (int)(end - src);
        memcpy(dst, src, len);
        dst[len] = 0;
    } else {
        // Remove outer quotes
        if (t->len > 1) {
            if (*src == '\"') {
                src++;
            }
            if (*(end - 1) == '\"') {
                end--;
            }
        }

        // The unescaped string should always be smaller or the same size,
        // so len+1 should be fine for now. Replace with actual len calc.

        while (src != end) {
            if (escaped) {
                switch (*src) {
                    case '\\':
                        *dst = '\\';
                        break;
                    case 't':
                        *dst = '\t';
                        break;
                    case 'n':
                        *dst = '\n';
                        break;

                    default:
                        *dst = *src; // anything else should just be pass-through, such as \"
                }
                escaped = ekFalse;
            } else {
                if (*src == '\\') {
                    escaped = ekTrue;
                    src++;
                    continue;
                } else {
                    *dst = *src;
                }
            }

            dst++;
            src++;
        }
        *dst = 0;
    }
    return str;
}

ekS32 ekTokenToInt(struct ekContext * E, ekToken * t)
{
    char temp[32];
    ekS32 base = 10;

    ekS32 len = t->len;
    if (len > 31) { len = 31; }
    strncpy(temp, t->text, len);
    temp[len] = 0;

    if (len && (temp[0] == '0')) {
        if ((len > 1) && ((temp[1] == 'x') || (temp[1] == 'X'))) {
            base = 16;
        } else {
            base = 8;
        }
    }

    return strtol(temp, NULL, base);
}

float ekTokenToFloat(struct ekContext * E, ekToken * t)
{
    char temp[32];
    char * endptr;
    double d;

    ekS32 len = t->len;
    if (len > 31) { len = 31; }
    strncpy(temp, t->text, len);
    temp[len] = 0;

    d = strtod(temp, &endptr);
    return (ekF32)d;
}

const char * ekTokenTypeName(ekTokenType type)
{
#define TYPE_NAME(T) case ETT_ ## T: return #T

    switch (type) {
        TYPE_NAME(AND);
        TYPE_NAME(ARRAYOPENBRACKET);
        TYPE_NAME(ASSIGN);
        TYPE_NAME(BITWISE_AND);
        TYPE_NAME(BITWISE_ANDEQUALS);
        TYPE_NAME(BITWISE_NOT);
        TYPE_NAME(BITWISE_OR);
        TYPE_NAME(BITWISE_OREQUALS);
        TYPE_NAME(BITWISE_XOR);
        TYPE_NAME(BITWISE_XOREQUALS);
        TYPE_NAME(BREAK);
        TYPE_NAME(CLOSEBRACE);
        TYPE_NAME(CLOSEBRACKET);
        TYPE_NAME(CMP);
        TYPE_NAME(COLON);
        TYPE_NAME(COLONCOLON);
        TYPE_NAME(COMMA);
        TYPE_NAME(COMMENT);
        TYPE_NAME(DASH);
        TYPE_NAME(DASHEQUALS);
        TYPE_NAME(ELLIPSIS);
        TYPE_NAME(ELSE);
        TYPE_NAME(ENDBLOCK);
        TYPE_NAME(ENDSTATEMENT);
        TYPE_NAME(EOF);
        TYPE_NAME(EQUALS);
        TYPE_NAME(FALSE);
        TYPE_NAME(FATCOMMA);
        TYPE_NAME(FLOATNUM);
        TYPE_NAME(FOR);
        TYPE_NAME(FUNCTION);
        TYPE_NAME(GREATERTHAN);
        TYPE_NAME(GREATERTHANOREQUAL);
        TYPE_NAME(GROUPLEFTPAREN);
        TYPE_NAME(HEREDOC);
        TYPE_NAME(IDENTIFIER);
        TYPE_NAME(IF);
        TYPE_NAME(IN);
        TYPE_NAME(INHERITS);
        TYPE_NAME(INTEGER);
        TYPE_NAME(LEFTPAREN);
        TYPE_NAME(LESSTHAN);
        TYPE_NAME(LESSTHANOREQUAL);
        TYPE_NAME(LITERALSTRING);
        TYPE_NAME(MAPSTARTBLOCK);
        TYPE_NAME(MOD);
        TYPE_NAME(NEGATIVE);
        TYPE_NAME(NEWLINE);
        TYPE_NAME(NOT);
        TYPE_NAME(NOTEQUALS);
        TYPE_NAME(NULL);
        TYPE_NAME(OPENBRACE);
        TYPE_NAME(OPENBRACKET);
        TYPE_NAME(OR);
        TYPE_NAME(PERIOD);
        TYPE_NAME(PLUS);
        TYPE_NAME(PLUSEQUALS);
        TYPE_NAME(QUESTIONMARK);
        TYPE_NAME(REGEXSTRING);
        TYPE_NAME(RETURN);
        TYPE_NAME(RIGHTPAREN);
        TYPE_NAME(SCOPESTARTBLOCK);
        TYPE_NAME(SEMI);
        TYPE_NAME(SHIFTLEFT);
        TYPE_NAME(SHIFTLEFTEQUALS);
        TYPE_NAME(SHIFTRIGHT);
        TYPE_NAME(SHIFTRIGHTEQUALS);
        TYPE_NAME(SLASH);
        TYPE_NAME(SLASHEQUALS);
        TYPE_NAME(SPACE);
        TYPE_NAME(STAR);
        TYPE_NAME(STAREQUALS);
        TYPE_NAME(STARTBLOCK);
        TYPE_NAME(THIS);
        TYPE_NAME(TRUE);
        TYPE_NAME(VAR);
        TYPE_NAME(WHILE);
        default:
            break;
    }
    return "UNKNOWN";
}

#define YYCTYPE char
#define YYCURSOR l->cur
#define YYMARKER l->marker
#define YYLIMIT l->end

//#include "ekLexerNames.h"
//#define CALL_CB(A,ID,C,D) { \
//    char temp[256] = {0}; \
//    strncpy(temp, l.token, token_len); \
//    printf("%s - %s\n", ekTokenIDToString(ID), temp); \
//    cb(A,ID,C,D); \
// //}
#define CALL_CB cb

static ekS32 getNextToken(ekLexer * l)
{
#include "ekLexer.re.inc"
    return ETT_EOF;
}
