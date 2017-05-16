// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekmRegex.h"

#include "ekFrame.h"
#include "ekMap.h"
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
    ekValue * subject;
    pcre * regex;
    int offset;
    ekBool global;
} ekRegex;

static void ekRegexDestroy(ekContext * E, ekRegex * regex)
{
    if (regex->regex) {
        pcre_free(regex->regex);
    }
    ekFree(regex);
}

static ekRegex * ekRegexCreate(ekContext * E, ekValue * subject, ekValue * pattern, ekValue * options)
{
    ekS32 regexFlags = 0;
    const char * regexError;
    ekS32 regexErrorOffset;
    ekRegex * regex = calloc(1, sizeof(ekRegex));

    regex->subject = subject;
    regex->offset = 0;
    regex->global = ekFalse;

    if (options) {
        const char * c = ekValueSafeStr(options);
        for (; *c; c++) {
            switch (*c) {
                case 'x': regexFlags |= PCRE_EXTENDED;  break;
                case 'i': regexFlags |= PCRE_CASELESS;  break;
                case 's': regexFlags |= PCRE_DOTALL;    break;
                case 'm': regexFlags |= PCRE_MULTILINE; break;

                case 'g': regex->global = ekTrue;       break;
            }
        }
    }

    regex->regex = pcre_compile(ekValueSafeStr(pattern), regexFlags, &regexError, &regexErrorOffset, NULL);
    if (!regex->regex) {
        // Regex compilation errors are fatal for now. I think this is good.
        ekContextSetError(E, EVE_RUNTIME, "regex compilation error: %s", regexError);
        ekRegexDestroy(E, regex);
        regex = NULL;
    }
    return regex;
}

static ekBool ekRegexMatchNext(ekContext * E, ekRegex * regex, ekValue * matches)
{
    // Why PCRE didn't make this an array of small structs is baffling to me;
    // it would have led to much more obvious, self-documenting code.
    ekS32 regexVectors[EUREKA_MAX_REGEX_VECTORS];
    ekS32 len = strlen(ekValueSafeStr(regex->subject));
    ekS32 err = pcre_exec(regex->regex, 0, ekValueSafeStr(regex->subject), len, regex->offset, 0, regexVectors, EUREKA_MAX_REGEX_VECTORS);
    if (err > 0) {
        ekS32 i;
        ekAssert(matches->type == EVT_ARRAY);
        for (i = 0; i < err; ++i) {
            ekS32 index = i * 2;
            ekValue * match = ekValueDonateString(E, ekSubstrdup(E, ekValueSafeStr(regex->subject), regexVectors[index], regexVectors[index + 1]));
            ekArrayPush(E, &matches->arrayVal, match);
        }
        regex->offset = regexVectors[1];
        return ekTrue;
    }
    return ekFalse;
}

static ekS32 ekRegexSub(ekContext * E, ekRegex * regex, ekValue * replace, ekString * output)
{
    const char * subject = ekValueSafeStr(regex->subject);
    ekS32 len = strlen(subject);
    ekS32 count = 0;
    ekS32 regexVectors[EUREKA_MAX_REGEX_VECTORS];
    ekS32 err;
    while ((err = pcre_exec(regex->regex, 0, subject, len, regex->offset, 0, regexVectors, EUREKA_MAX_REGEX_VECTORS)) > 0) {
        ekStringConcatLen(E, output, &subject[regex->offset], regexVectors[0] - regex->offset);
        ekStringConcatStr(E, output, &replace->stringVal);
        regex->offset = regexVectors[1];
        ++count;

        if (!regex->global)
            break;
    }
    if (regex->offset < len) {
        ekStringConcatLen(E, output, &subject[regex->offset], len - regex->offset);
    }
    return count;
}

static ekRegex * regexCreateMatchFromArgs(struct ekContext * E, ekU32 argCount, const char * errorString)
{
    ekValue * pattern = NULL;
    ekValue * subject = NULL;
    ekValue * options = NULL;
    ekRegex * regex;

    if (!ekContextGetArgs(E, argCount, "ss|s", &subject, &pattern, &options)) {
        ekContextArgsFailure(E, argCount, errorString);
        return NULL;
    }

    return ekRegexCreate(E, subject, pattern, options);
}

static ekU32 regexMatch(struct ekContext * E, ekU32 argCount)
{
    ekValue * results = ekValueNullPtr;
    ekRegex * regex = regexCreateMatchFromArgs(E, argCount, "re.match([string] subject, [string] pattern, [optional string] options)");
    if (regex) {
        results = ekValueCreateArray(E);
        if (!ekRegexMatchNext(E, regex, results)) {
            results = ekValueNullPtr;
        }
        ekRegexDestroy(E, regex);
    }
    ekContextReturn(E, results);
}

static ekU32 regexGMatchIterator(struct ekContext * E, ekU32 argCount)
{
    ekFrame * frame = ekArrayTop(E, &E->frames);
    ekValue * regexValue;
    ekRegex * regex;
    ekValue * results;

    ekAssert(frame->closure && frame->closure->closureVars);
    regexValue = ekMapGetS2P(E, frame->closure->closureVars, "regex");
    regex = (ekRegex *)regexValue->ptrVal;

    ekAssert(argCount == 0);
    ekContextPopValues(E, argCount);

    results = ekValueCreateArray(E);
    if (!ekRegexMatchNext(E, regex, results)) {
        results = ekValueNullPtr;
    }

    ekContextReturn(E, results);
}

static ekU32 regexGMatch(struct ekContext * E, ekU32 argCount)
{
    ekRegex * regex = regexCreateMatchFromArgs(E, argCount, "re.gmatch([string] subject, [string] pattern, [optional string] options)");
    if (regex) {
        ekValue * closure = ekValueCreateCFunction(E, regexGMatchIterator);
        ekValue * regexValue = ekValueCreate(E, ekValueTypeId(E, 'R'));
        regexValue->ptrVal = regex;
        closure->closureVars = ekMapCreate(E, EMKT_STRING);
        ekMapGetS2P(E, closure->closureVars, "regex") = regexValue;
        ekContextReturn(E, closure);
    }
    ekContextReturn(E, ekValueNullPtr);
}

static ekU32 regexSubInternal(struct ekContext * E, ekU32 argCount, ekBool forceGlobal)
{
    ekValue * subject = NULL;
    ekValue * pattern = NULL;
    ekValue * replace = NULL;
    ekValue * options = NULL;
    ekValue * result = ekValueNullPtr;
    ekRegex * regex;
    int count = 0;

    if (!ekContextGetArgs(E, argCount, "sss|s", &subject, &pattern, &replace, &options)) {
        return ekContextArgsFailure(E, argCount, "re.sub([string] subject, [string] pattern, [string] replacement, [optional string] options)");
    }

    result = ekValueCreateString(E, "");
    regex = ekRegexCreate(E, subject, pattern, options);
    if (forceGlobal) {
        regex->global = ekTrue;
    }
    if (regex) {
        count = ekRegexSub(E, regex, replace, &result->stringVal);
        ekRegexDestroy(E, regex);
    }
    ekContextReturn(E, result);
}

static ekU32 regexSub(struct ekContext * E, ekU32 argCount)
{
    return regexSubInternal(E, argCount, ekFalse);
}

static ekU32 regexGSub(struct ekContext * E, ekU32 argCount)
{
    return regexSubInternal(E, argCount, ekTrue);
}

// ---------------------------------------------------------------------------
// Regex Funcs

static void regexFuncClear(struct ekContext * E, struct ekValue * p)
{
    ekRegex * regex = (ekRegex *)p->ptrVal;
    ekRegexDestroy(E, regex);
}

static void regexFuncDump(struct ekContext * E, ekDumpParams * params, struct ekValue * p)
{
    // TODO: implement something interesting
    ekStringConcat(E, &params->output, "regex");
}

static void ekValueTypeRegisterRegex(struct ekContext * E)
{
    // This is only used as a closure variable during a gmatch, no need to implement
    // all of the type routines.
    ekValueType * type = ekValueTypeCreate(E, "regex", 'R');
    type->funcClear      = regexFuncClear;
    type->funcDump       = regexFuncDump;

    ekValueTypeRegister(E, type);
}

static ekModuleFunc regexFuncs[] =
{
    { "match", regexMatch },
    { "gmatch", regexGMatch },
    { "sub", regexSub },
    { "gsub", regexGSub },

    { NULL, NULL }
};

void ekModuleRegisterRegex(struct ekContext * E)
{
    ekValueTypeRegisterRegex(E);
    ekContextAddModule(E, "re", regexFuncs);
}
