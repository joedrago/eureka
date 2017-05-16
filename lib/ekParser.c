// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekParser.h"

#include "ekCompiler.h"
#include "ekContext.h"
#include "ekSyntax.h"

ekParser * ekParserCreate(struct ekContext * E)
{
    ekParser * parser = (ekParser *)ekAlloc(sizeof(ekParser));
    return parser;
}

void ekParserDestroy(struct ekContext * E, ekParser * parser)
{
    ekFree(parser);
}

void ekParserParse(struct ekContext * E, struct ekParser * parser, struct ekCompiler * compiler, const char * source)
{
    compiler->root = ekSyntaxCreate(E, EST_STATEMENTLIST, 1); // Lies!
}
