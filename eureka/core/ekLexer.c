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
//}
#define CALL_CB cb

typedef struct ekLexer
{
    const char *text;
    const char *marker;
    const char *cur;
    const char *token;
    const char *end;
    int line;
    ekBool error;
} ekLexer;

int getNextToken(ekLexer *l)
{
#include "ekLexer.re.inc"
    return YTT_EOF;
}

ekBool ekLex(void *parser, const char *text, tokenCB cb, struct ekCompiler *compiler)
{
    struct ekContext *E = compiler->E;
    int id;
    int token_len;
    int tokenMax;
    ekToken token;
    ekLexer l = {0};

    l.text   = text;
    l.end    = l.text + strlen(l.text);
    l.cur    = l.text;
    l.token  = l.cur;
    l.line   = 1;

    while((id = getNextToken(&l)) != YTT_EOF)
    {
        if(l.error)
        {
            break;
        }

        token_len = (int)(l.cur - l.token);

        // Normal C style lexers treat semi-colons as end statements
        // and braces as block delimiters. Newlines are just whitespace.

        switch(id)
        {
            case YTT_HEREDOC:
                // Heredocs are just trimmed literal strings
                id = YTT_LITERALSTRING;
                ekAssert(token_len >= 6);

                // remove triple quotes
                token_len -= 6;
                l.token += 3;
                break;
            case YTT_NEWLINE:
                id = YTT_SPACE;
                break;
            case YTT_SEMI:
                id = YTT_ENDSTATEMENT;
                break;
            case YTT_OPENBRACE:
                id = YTT_STARTBLOCK;
                break;
            case YTT_CLOSEBRACE:
                id = YTT_ENDBLOCK;
                break;
        };

        if((id != YTT_SPACE) && (id != YTT_COMMENT))
        {
            if(token_len > 0)
            {
                token.text = l.token;
                token.len  = token_len;
                token.line = l.line;

                CALL_CB(parser, id, token, compiler);
            }
        }

        l.token = l.cur;
    }

    if(!ekArraySize(E, &compiler->errors))
    {
        token.line = l.line;
        CALL_CB(parser, YTT_ENDSTATEMENT, token, compiler);
    }
    return ekTrue;
}

ekToken *ekTokenClone(struct ekContext *E, ekToken *token)
{
    ekToken *ret = ekTokenCreate();
    *ret = *token;
    return ret;
}

char *ekTokenToString(struct ekContext *E, ekToken *t)
{
    const char *src = t->text;
    const char *end = src + t->len;
    char *str = ekAlloc(t->len + 1);
    char *dst = str;
    ekBool escaped = ekFalse;

    // Remove outer quotes
    if(t->len > 1)
    {
        if(*src == '\"')
        {
            src++;
        }
        if(*(end - 1) == '\"')
        {
            end--;
        }
    }

    // The unescaped string should always be smaller or the same size,
    // so len+1 should be fine for now. Replace with actual len calc.

    while(src != end)
    {
        if(escaped)
        {
            switch(*src)
            {
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
        }
        else
        {
            if(*src == '\\')
            {
                escaped = ekTrue;
                src++;
                continue;
            }
            else
            {
                *dst = *src;
            }
        }

        dst++;
        src++;
    }
    *dst = 0;
    return str;
}

int ekTokenToInt(struct ekContext *E, ekToken *t)
{
    char temp[32];
    int base = 10;

    int len = t->len;
    if(len > 31) { len = 31; }
    strncpy(temp, t->text, len);
    temp[len] = 0;

    if(len && temp[0] == '0')
        if((len > 1) && (temp[1] == 'x' || temp[1] == 'X'))
        {
            base = 16;
        }
        else
        {
            base = 8;
        }

    return strtol(temp, NULL, base);
}

float ekTokenToFloat(struct ekContext *E, ekToken *t)
{
    char temp[32];
    char *endptr;
    double d;

    int len = t->len;
    if(len > 31) { len = 31; }
    strncpy(temp, t->text, len);
    temp[len] = 0;

    d = strtod(temp, &endptr);
    return (ekF32)d;
}

