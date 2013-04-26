// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekmPCRE.h"

#include "ekValue.h"
#include "ekContext.h"

#include <string.h>
#include <stdio.h>
#include <pcre.h>

#define EUREKA_MAX_REGEX_VECTORS (30)

// ---------------------------------------------------------------------------
// Data Structures

typedef struct ekRegex
{
    ekValue *filename;
    FILE *handle;
    int state;
} ekFile;


static ekS32 ekRegexOptionsToPCREFlags(const char *options)
{
    ekS32 pcreFlags = 0;
    const char *c = options;
    for(; *c; c++)
    {
        switch(*c)
        {
            case 'x': pcreFlags |= PCRE_EXTENDED;  break;
            case 'i': pcreFlags |= PCRE_CASELESS;  break;
            case 's': pcreFlags |= PCRE_DOTALL;    break;
            case 'm': pcreFlags |= PCRE_MULTILINE; break;
        };
    }
    return pcreFlags;
}

static ekU32 regex_match(struct ekContext *E, ekU32 argCount)
{
    ekValue *pattern = NULL;
    ekValue *subject = NULL;
    ekValue *options = NULL;
    ekValue *results = ekValueNullPtr;
    ekValue *matches = NULL;

    pcre *regex;
    ekS32 regexFlags = 0;
    const char *regexError;
    ekS32 regexErrorOffset;
    ekS32 regexVectors[EUREKA_MAX_REGEX_VECTORS];

    if(!ekContextGetArgs(E, argCount, "ss|s", &subject, &pattern, &options))
    {
        return ekContextArgsFailure(E, argCount, "match([string] subject, [string] pattern, [optional string] options)");
    }

    if(options)
    {
        regexFlags = ekRegexOptionsToPCREFlags(ekStringSafePtr(&options->stringVal));
    }

    regex = pcre_compile(ekStringSafePtr(&pattern->stringVal), regexFlags, &regexError, &regexErrorOffset, NULL);
    if(regex)
    {
        ekS32 len = strlen(ekValueSafeStr(subject));
        ekS32 err = pcre_exec(regex, 0, ekValueSafeStr(subject), len, 0, 0, regexVectors, EUREKA_MAX_REGEX_VECTORS);
        if(err > 0)
        {
            ekS32 i;
            matches = ekValueCreateArray(E);
            results = matches;
            for(i=0; i<err; i++)
            {
                ekS32 index = i*2;
                ekValue *match = ekValueDonateString(E, ekSubstrdup(E, ekStringSafePtr(&subject->stringVal), regexVectors[index], regexVectors[index+1]));
                ekArrayPush(E, &matches->arrayVal, match);
            }
        }
        pcre_free(regex);
    }
    else
    {
        // Regex compilation errors are fatal for now. I think this is good.
        ekContextSetError(E, EVE_RUNTIME, "regex_match() error: %s", regexError);
    }

    ekArrayPush(E, &E->stack, results);
    return 1;
}

void ekModuleRegisterPCRE(struct ekContext *E)
{
    ekContextAddIntrinsic(E, "match", regex_match);
}
