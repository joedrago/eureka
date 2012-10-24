// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapmPCRE.h"

#include "yapValue.h"
#include "yapContext.h"

#include <string.h>
#include <stdio.h>
#include <pcre.h>

#define YAP_MAX_REGEX_VECTORS (30)

static int yapRegexOptionsToPCREFlags(const char *options)
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

static yU32 regex_match(struct yapContext *Y, yU32 argCount)
{
    yapValue *pattern = NULL;
    yapValue *subject = NULL;
    yapValue *options = NULL;
    yapValue *results = yapValueNullPtr;
    yapValue *matches = NULL;

    pcre *regex;
    int regexFlags = 0;
    const char *regexError;
    int regexErrorOffset;
    int regexVectors[YAP_MAX_REGEX_VECTORS];

    if(!yapContextGetArgs(Y, argCount, "ss|s", &pattern, &subject, &options))
    {
        return yapContextArgsFailure(Y, argCount, "regex_match([string] pattern, [string] subject, [optional string] options)");
    }

    if(options)
    {
        regexFlags = yapRegexOptionsToPCREFlags(yapStringSafePtr(&options->stringVal));
    }

    regex = pcre_compile(yapStringSafePtr(&pattern->stringVal), regexFlags, &regexError, &regexErrorOffset, NULL);
    if(regex)
    {
        int len = strlen(yapStringSafePtr(&subject->stringVal));
        int err = pcre_exec(regex, 0, yapStringSafePtr(&subject->stringVal), len, 0, 0, regexVectors, YAP_MAX_REGEX_VECTORS);
        if(err > 0)
        {
            int i;
            matches = yapValueCreateArray(Y);
            //results = yapValueObjectCreate(Y, NULL, 0);
            //yapValueObjectSetMember(Y, results, "matches", matches);
            results = matches;
            for(i=0; i<err; i++)
            {
                int index = i*2;
                yapValue *match = yapValueDonateString(Y, yapSubstrdup(Y, yapStringSafePtr(&subject->stringVal), regexVectors[index], regexVectors[index+1]));
                yap2ArrayPush(Y, &matches->arrayVal, match);
            }
        }
        pcre_free(regex);
    }
    else
    {
        // Regex compilation errors are fatal for now. I think this is good.
        yapContextSetError(Y, YVE_RUNTIME, "regex_match() error: %s", regexError);
    }

    yap2ArrayPush(Y, &Y->stack, results);
    return 1;
}

void yapModuleRegisterPCRE(struct yapContext *Y)
{
    yapContextRegisterGlobalFunction(Y, "regex_match", regex_match);
}
