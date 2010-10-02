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
    int col;
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

    while(!compiler->error && ((id = getNextToken(&l)) != YTT_EOF))
    {
        if(l.error)
            break;

        if((id != YTT_SPACE)
        && (id != YTT_NEWLINE))
        && (id != YTT_COMMENT))
        {
            token_len = (int)(l.cur - l.token);
            if(token_len > 0)
            {
                token.text = l.token;
                token.len = token_len;

                cb(parser, id, token, compiler);
            }
        }

        l.token = l.cur;
    }

    return yTrue;
}

char *yapTokenToString(yapToken *t)
{
    char *str = yapAlloc(t->len+1);
    memcpy(str, t->text, t->len);
    str[t->len] = 0;
    return str;
}

