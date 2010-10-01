#include "yapParser.h"

#include "yapLexer.h"

#include <stdlib.h>

void *yapTokenizeAlloc(void *(*mallocProc)(size_t));
void yapTokenize(void *yyp, yToken token, const char *text);

yBool yapParse(const char *text)
{
    void *parser = yapTokenizeAlloc( malloc );

    yapLex(parser, text, yapTokenize);
    yapTokenize(parser, 0, "end of file?");

    yapTokenizeFree(parser, free);
    return yTrue;
}
