#include "yapLexer.h"

#include "yapCompiler.h"
#include "yapParser.h"
#include "yapSyntax.h"

#include <stdio.h>
#include <string.h>

#define YYCTYPE char
#define YYCURSOR l->cur
#define YYMARKER l->marker
#define YYLIMIT l->end

//#include "yapLexerNames.h"
//#define CALL_CB(A,ID,C,D) { \
//    char temp[256] = {0}; \
//    strncpy(temp, l.token, token_len); \
//    printf("%s - %s\n", yapTokenIDToString(ID), temp); \
//    cb(A,ID,C,D); \
//}
#define CALL_CB cb

typedef struct yapLexer
{
    const char *text;
    const char *marker;
    const char *cur;
    const char *token;
    const char *end;
    int line;
    yBool error;
} yapLexer;

int getNextToken(yapLexer *l)
{
#include "yapLexer.re.inc"
    return YTT_EOF;
}

yBool yapLex(void *parser, const char *text, tokenCB cb, struct yapCompiler *compiler)
{
    int id;
    int token_len;
    int tokenMax;
    yapToken token;
    yapLexer l = {0};

    l.text   = text;
    l.end    = l.text + strlen(l.text);
    l.cur    = l.text;
    l.token  = l.cur;
    l.line   = 1;

    while(!compiler->errors.count && ((id = getNextToken(&l)) != YTT_EOF))
    {
        if(l.error)
            break;

        token_len = (int)(l.cur - l.token);

        // Normal C style lexers treat semi-colons as end statements
        // and braces as block delimiters. Newlines are just whitespace.

        switch(id)
        {
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

    if(!compiler->errors.count)
    {
        token.line = l.line;
        CALL_CB(parser, YTT_ENDSTATEMENT, token, compiler);
    }
    return yTrue;
}

yapToken *yapTokenClone(yapToken *token)
{
    yapToken *ret = yapTokenCreate();
    *ret = *token;
    return ret;
}

char *yapTokenToString(yapToken *t)
{
    const char *src = t->text;
    const char *end = src + t->len;
    char *str = yapAlloc(t->len + 1);
    char *dst = str;
    yBool escaped = yFalse;

    // Remove outer quotes
    if(t->len > 1)
    {
        if(*src == '\"')
            src++;
        if(*(end - 1) == '\"')
            end--;
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
                yapTrace(("Unknown escape character %c\n", *src));
            }
            escaped = yFalse;
        }
        else
        {
            if(*src == '\\')
            {
                escaped = yTrue;
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

int yapTokenToInt(yapToken *t)
{
    char temp[32];
    int base = 10;

    int len = t->len;
    if(len > 31) len = 31;
    strncpy(temp, t->text, len);
    temp[len] = 0;

    if(len && temp[0] == '0')
        if((len > 1) && (temp[1] == 'x' || temp[1] == 'X'))
            base = 16;
        else
            base = 8;

    return strtol(temp, NULL, base);
}

