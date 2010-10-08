#include "yapLexer.h"

#include "yapCompiler.h"

#include <stdio.h>
#include <string.h>

#define YYCTYPE char 
#define YYCURSOR l->cur
#define YYMARKER l->marker
#define YYLIMIT l->end

typedef struct yapLexer
{
    const char *text;
    const char *marker;
    const char *cur;
    const char *token;
    const char *end;
    int line;
    yBool newline;
    yap32Array indents;
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
    int indentTop;
    yapToken token;
    yapLexer l = {0};

    l.text   = text;
    l.end    = l.text + strlen(l.text);
    l.cur    = l.text;
    l.token  = l.cur;

    // TODO: Comment how this all works

    yap32ArrayPush(&l.indents, 0);
    indentTop = l.indents.data[l.indents.count-1];

    while(!compiler->error && ((id = getNextToken(&l)) != YTT_EOF))
    {
        if(l.error)
            break;

        token_len = (int)(l.cur - l.token);

        if(l.newline)
        {
            int curIndent = 0;
            if(id == YTT_SPACE)
            {
                curIndent = token_len;
            }

            if(curIndent > indentTop)
            {
                yap32ArrayPush(&l.indents, token_len);
                indentTop = l.indents.data[l.indents.count-1];
                cb(parser, YTT_INDENT, token, compiler);
            }
            else if(curIndent < indentTop)
            {
                if(l.indents.count <= 1)
                {
                    printf("Indent stack empty!\n");
                    compiler->error = yTrue;
                    return yFalse;
                }

                yap32ArrayPop(&l.indents);
                indentTop = l.indents.data[l.indents.count-1];
                cb(parser, YTT_DEDENT, token, compiler);
            }
        }

        if((id != YTT_SPACE) && (id != YTT_COMMENT))
        {
            if(token_len > 0)
            {
                token.text = l.token;
                token.len = token_len;

                cb(parser, id, token, compiler);
            }
        }

        l.token = l.cur;
        l.newline = (id == YTT_NEWLINE);
    }

    while(l.indents.count > 1)
    {
        yap32ArrayPop(&l.indents);
        cb(parser, YTT_DEDENT, token, compiler);
    }

    yap32ArrayClear(&l.indents);
    return yTrue;
}

yapToken * yapTokenClone(yapToken *token)
{
    yapToken *ret = yapTokenCreate();
    *ret = *token;
    return ret;
}

char *yapTokenToString(yapToken *t)
{
    char *str = yapAlloc(t->len+1);
    memcpy(str, t->text, t->len);
    str[t->len] = 0;
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

