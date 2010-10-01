#include "yapLexer.h"

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

yToken getNextToken(yapLexer *l)
{
#include "yapLexer.re.inc"
    return YTT_EOF;
}

yBool yapLex(void *parser, const char *text, tokenCB cb)
{
	int id;
    int token_len;
	yapLexer l = {0};

	l.text   = text;
	l.end    = l.text + strlen(l.text);
	l.cur    = l.text;
	l.token  = l.cur;

	while((id = getNextToken(&l)) != YTT_EOF)
	{
		if(l.error)
			break;

		token_len = (int)(l.cur - l.token);
		if(token_len > 0)
		{
			char token[1024];
			if(token_len > 1023)
				token_len = 1023;
			strncpy(token, l.token, token_len);
			token[token_len] = 0;

            printf("TOKEN %d: %s\n", id, token);
            cb(parser, id, token);
		}

        l.token = l.cur;
	}

	return yTrue;
}
