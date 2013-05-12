// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekmRegex.h"

#include "ekValue.h"
#include "ekValueType.h"
#include "ekContext.h"

#include <string.h>
#include <stdio.h>
#include <pcre.h>

#define EUREKA_MAX_REGEX_VECTORS (30)

// ---------------------------------------------------------------------------
// Data Structures

typedef struct ekRegex
{
    pcre *regex;
} ekRegex;


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

static ekU32 regexMatch(struct ekContext *E, ekU32 argCount)
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
        return ekContextArgsFailure(E, argCount, "re.match([string] subject, [string] pattern, [optional string] options)");
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
        ekContextSetError(E, EVE_RUNTIME, "regexMatch() error: %s", regexError);
    }

    ekArrayPush(E, &E->stack, results);
    return 1;
}

// ---------------------------------------------------------------------------
// Regex Funcs

static void regexFuncClear(struct ekContext *E, struct ekValue *p)
{
    ekRegex *regex = (ekRegex *)p->ptrVal;
    // TODO: cleanup regex
}

static void regexFuncClone(struct ekContext *E, struct ekValue *dst, struct ekValue *src)
{
    ekAssert(0 && "regexFuncClone not implemented");
}

static ekBool regexFuncToBool(struct ekContext *E, struct ekValue *p)
{
    return ekTrue;
}

static ekS32 regexFuncToInt(struct ekContext *E, struct ekValue *p)
{
    return 1; // ?
}

static ekF32 regexFuncToFloat(struct ekContext *E, struct ekValue *p)
{
    return 1.0f; // ?
}

static struct ekValue *regexFuncToString(struct ekContext *E, struct ekValue *p)
{
    ekValueRemoveRefNote(E, p, "regexFuncToString doesnt need regex anymore");
    return ekValueCreateKString(E, "[regex]");
}

static void regexFuncDump(struct ekContext *E, ekDumpParams *params, struct ekValue *p)
{
    // TODO: implement something interesting
    ekStringConcat(E, &params->output, "regex");
}

//static ekCFunction *regexFuncIter(struct ekContext *E, struct ekValue *p)
//{
//    return regexCreateIterator;
//}

static void ekValueTypeRegisterRegex(struct ekContext *E)
{
    ekValueType *type = ekValueTypeCreate(E, "regex", 'R');
    type->funcClear      = regexFuncClear;
    type->funcClone      = regexFuncClone;
    type->funcToBool     = regexFuncToBool;
    type->funcToInt      = regexFuncToInt;
    type->funcToFloat    = regexFuncToFloat;
    type->funcToString   = regexFuncToString;
    //type->funcIter       = regexFuncIter;
    type->funcDump       = regexFuncDump;

    ekValueTypeRegister(E, type);
}

static ekModuleFunc regexFuncs[] =
{
    { "match", regexMatch },

    { NULL, NULL }
};

void ekModuleRegisterRegex(struct ekContext *E)
{
    ekValueTypeRegisterRegex(E);
    ekContextAddModule(E, "re", regexFuncs);
}
