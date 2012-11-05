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

static int ekRegexOptionsToPCREFlags(const char *options)
{
    int pcreFlags = 0;
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

static ekU32 regex_match(struct ekContext *Y, ekU32 argCount)
{
    ekValue *pattern = NULL;
    ekValue *subject = NULL;
    ekValue *options = NULL;
    ekValue *results = ekValueNullPtr;
    ekValue *matches = NULL;

    pcre *regex;
    int regexFlags = 0;
    const char *regexError;
    int regexErrorOffset;
    int regexVectors[EUREKA_MAX_REGEX_VECTORS];

    if(!ekContextGetArgs(Y, argCount, "ss|s", &pattern, &subject, &options))
    {
        return ekContextArgsFailure(Y, argCount, "regex_match([string] pattern, [string] subject, [optional string] options)");
    }

    if(options)
    {
        regexFlags = ekRegexOptionsToPCREFlags(ekStringSafePtr(&options->stringVal));
    }

    regex = pcre_compile(ekStringSafePtr(&pattern->stringVal), regexFlags, &regexError, &regexErrorOffset, NULL);
    if(regex)
    {
        int len = strlen(ekStringSafePtr(&subject->stringVal));
        int err = pcre_exec(regex, 0, ekStringSafePtr(&subject->stringVal), len, 0, 0, regexVectors, EUREKA_MAX_REGEX_VECTORS);
        if(err > 0)
        {
            int i;
            matches = ekValueCreateArray(Y);
            //results = ekValueObjectCreate(Y, NULL, 0);
            //ekValueObjectSetMember(Y, results, "matches", matches);
            results = matches;
            for(i=0; i<err; i++)
            {
                int index = i*2;
                ekValue *match = ekValueDonateString(Y, ekSubstrdup(Y, ekStringSafePtr(&subject->stringVal), regexVectors[index], regexVectors[index+1]));
                ekArrayPush(Y, &matches->arrayVal, match);
            }
        }
        pcre_free(regex);
    }
    else
    {
        // Regex compilation errors are fatal for now. I think this is good.
        ekContextSetError(Y, YVE_RUNTIME, "regex_match() error: %s", regexError);
    }

    ekArrayPush(Y, &Y->stack, results);
    return 1;
}

void ekModuleRegisterPCRE(struct ekContext *Y)
{
    ekContextRegisterGlobalFunction(Y, "regex_match", regex_match);
}
