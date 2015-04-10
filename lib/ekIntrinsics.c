// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekIntrinsics.h"

#include "ekTypes.h"
#include "ekChunk.h"
#include "ekFrame.h"
#include "ekMap.h"
#include "ekObject.h"
#include "ekValue.h"
#include "ekContext.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>

#ifdef PLATFORM_WIN32
#include <windows.h>
#define MAX_PATH_LENGTH MAX_PATH
#define realpath(N,R) _fullpath((R),(N),MAX_PATH)
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#define PATH_SEPARATOR "\\"
#else
#define MAX_PATH_LENGTH 2048
#include <sys/types.h>
#include <dirent.h>
#define USE_DIRENT
#ifdef PLATFORM_MINGW
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#endif
#define PATH_SEPARATOR "/"
#endif

// ---------------------------------------------------------------------------

static ekU32 ekiEval(struct ekContext *E, ekU32 argCount)
{
    ekValue *ret = NULL;
    if(argCount)
    {
        ekS32 i;
        ekValue *v = ekContextGetArg(E, 0, argCount);
        for(i = 0; i < argCount; i++)
        {
            ekValue *v = ekContextGetArg(E, i, argCount);
            if(v->type == EVT_STRING)
            {
                ekContextEval(E, NULL, ekStringSafePtr(&v->stringVal), 0, NULL);
                if(E->error)
                {
                    // steal the error from the VM so we can recover and THEN give it back as a ekValue
                    char *error = E->error;
                    E->error = NULL;

                    ekContextRecover(E);
                    ret = ekValueCreateString(E, error);
                    ekFree(error);
                }
            }
        }
        ekContextPopValues(E, argCount);
    }
    if(!ret)
    {
        ret = ekValueCreateInt(E, 0);
    }
    ekArrayPush(E, &E->stack, ret);
    return 1;
}

// ---------------------------------------------------------------------------

ekU32 ekiAssert(struct ekContext *E, ekU32 argCount)
{
    ekBool doAssert = ekTrue;
    ekValue *v = NULL;
    ekValue *s = NULL;
    if(!ekContextGetArgs(E, argCount, "|?s", &v, &s))
    {
        return ekContextArgsFailure(E, argCount, "assert(expr, [string] explanation)");
    }

    if(v)
    {
        v = ekValueToInt(E, v);
        doAssert = (!v->intVal) ? ekTrue : ekFalse;
        ekValueRemoveRefNote(E, v, "doAssert temp");
    }

    if(doAssert)
    {
        const char *reason = "(unknown)";
        if(s && s->type == EVT_STRING)
        {
            reason = ekStringSafePtr(&s->stringVal);
        }
        ekContextSetError(E, EVE_RUNTIME, "Eureka Runtime Assert: %s", reason);
    }
    if(s)
    {
        ekValueRemoveRefNote(E, s, "temporary reason string");
    }
    return 0;
}

// ---------------------------------------------------------------------------

ekU32 ekiType(struct ekContext *E, ekU32 argCount)
{
    if(argCount)
    {
        ekValue *a = ekContextGetArg(E, 0, argCount);
        ekValue *ret = ekValueCreateKString(E, (char *)ekValueTypeName(E, a->type));
        ekContextPopValues(E, argCount);
        ekArrayPush(E, &E->stack, ret);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------

ekU32 ekiLength(struct ekContext *E, ekU32 argCount)
{
    if(argCount)
    {
        ekValue *a = ekContextGetArg(E, 0, argCount);
        ekValue *ret = ekValueCreateInt(E, ekValueLength(E, a));
        ekContextPopValues(E, argCount);
        ekArrayPush(E, &E->stack, ret);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------

ekU32 ekiReverse(struct ekContext *E, ekU32 argCount)
{
    ekValue *v = NULL;
    if(!ekContextGetArgs(E, argCount, "?", &v))
    {
        return ekContextArgsFailure(E, argCount, "reverse(v)");
    }
    ekArrayPush(E, &E->stack, ekValueReverse(E, v));
    return 1;
}

// ---------------------------------------------------------------------------

ekU32 ekiDump(struct ekContext *E, ekU32 argCount)
{
    if(argCount)
    {
        ekValue *a = ekContextGetArg(E, 0, argCount);
        ekDumpParams *params = ekDumpParamsCreate(E);
        ekValue *ret = ekValueCreateKString(E, "");

        ekValueDump(E, params, a);
        ekStringDonateStr(E, &ret->stringVal, &params->output);

        ekDumpParamsDestroy(E, params);
        ekContextPopValues(E, argCount);
        ekArrayPush(E, &E->stack, ret);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------

static ekU32 ekiIterator(struct ekContext *E, ekU32 argCount)
{
    return ekContextIterOp(E, argCount);
}

// ---------------------------------------------------------------------------

static ekU32 rangeIterator(struct ekContext *E, ekU32 argCount)
{
    ekFrame *frame = ekArrayTop(E, &E->frames);
    ekValue *i;
    ekValue *end;
    ekAssert(frame->closure && frame->closure->closureVars);
    i = ekMapGetS2P(E, frame->closure->closureVars, "i");
    end = ekMapGetS2P(E, frame->closure->closureVars, "end");
    ekAssert((i->type == EVT_INT) && (end->type == EVT_INT));
    ekAssert(argCount == 0);
    ekContextPopValues(E, argCount);

    if(i->intVal < end->intVal)
    {
        ekArrayPush(E, &E->stack, ekValueCreateInt(E, i->intVal));
        ++i->intVal;
    }
    else
    {
        ekArrayPush(E, &E->stack, ekValueNullPtr);
    }
    return 1;
}

static ekU32 ekiRange(struct ekContext *E, ekU32 argCount)
{
    ekValue *i1 = NULL;
    ekValue *i2 = NULL;
    ekValue *closure;
    int start = 0;
    int end = 0;
    if(!ekContextGetArgs(E, argCount, "i|i", &i1, &i2))
    {
        return ekContextArgsFailure(E, argCount, "range(end) or range(start, end)");
    }
    closure = ekValueCreateCFunction(E, rangeIterator);
    closure->closureVars = ekMapCreate(E, EMKT_STRING);
    if(i2)
    {
        start = i1->intVal;
        end   = i2->intVal;
    }
    else
    {
        end = i1->intVal;
    }
    ekValueRemoveRefNote(E, i1, "ekiRange i1 done");
    if(i2)
    {
        ekValueRemoveRefNote(E, i2, "ekiRange i2 done");
    }
    ekMapGetS2P(E, closure->closureVars, "i")   = ekValueCreateInt(E, start);
    ekMapGetS2P(E, closure->closureVars, "end") = ekValueCreateInt(E, end);
    ekArrayPush(E, &E->stack, closure);
    return 1;
}


// ---------------------------------------------------------------------------

static ekU32 ekiPrint(struct ekContext *E, ekU32 argCount)
{
    if(argCount)
    {
        ekS32 i;
        for(i = 0; i < argCount; i++)
        {
            ekValue *v = ekContextGetArg(E, i, argCount);
            switch(v->type)
            {
                case EVT_STRING:
                    printf("%s", ekStringSafePtr(&v->stringVal));
                    break;
                case EVT_INT:
                    printf("%d", v->intVal);
                    break;
                case EVT_FLOAT:
                    printf("%f", v->floatVal);
                    break;
                default:
                    ekValueAddRefNote(E, v, "keeping ref for print's default ToString (symmetry with PopValues)");
                    v = ekValueToString(E, v);
                    printf("%s", ekStringSafePtr(&v->stringVal));
                    ekValueRemoveRefNote(E, v, "done with temp string (default)");
                    break;
            };
        }
        ekContextPopValues(E, argCount);
    }
    else
    {
        printf("(printing nothing)\n");
    }
    return 0;
}

// ---------------------------------------------------------------------------

static ekBool isFile(const char *path)
{
    struct stat st;
    if(!stat(path, &st))
    {
        if(S_ISREG(st.st_mode))
        {
            return ekTrue;
        }
    }
    return ekFalse;
}

static char *loadFile(struct ekContext *E, const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if(f)
    {
        ekS32 size;
        char *buffer;

        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);

        buffer = (char *)ekAlloc(size + 1);
        fread(buffer, 1, size, f);
        buffer[size] = 0;

        fclose(f);

        return buffer;
    }
    return NULL;
}

static ekU32 ekiRequire(struct ekContext *E, ekU32 argCount)
{
    ekValue *path = NULL;
    ekValue *resultArray;
    ekValue *thisPtr = NULL;
    if(!ekContextGetArgs(E, argCount, "s", &path))
    {
        return ekContextArgsFailure(E, argCount, "require([string] path)");
    }

    resultArray = ekValueCreateArray(E);
    {
        char *code;
        ekString filename = {0};
        ekChunk *chunk = ekContextGetCurrentChunk(E);
        if(!chunk)
        {
            ekContextSetError(E, EVE_RUNTIME, "require() cannot find current chunk!");
            return ekFalse;
        }

        if(chunk->searchPath.len > 0)
        {
            ekStringSetStr(E, &filename, &chunk->searchPath);
            ekStringConcat(E, &filename, PATH_SEPARATOR);
            ekStringConcat(E, &filename, ekValueSafeStr(path));
            ekStringConcat(E, &filename, ".ek");
            if(!isFile(ekStringSafePtr(&filename)))
            {
                ekStringSet(E, &filename, "");
            }
        }
        if(!filename.len)
        {
            // TODO: check registered search paths
        }
        if(filename.len)
        {
            code = loadFile(E, ekStringSafePtr(&filename));
            if(code)
            {
                ekContextEval(E, ekStringSafePtr(&filename), code, EEO_REQUIRE, resultArray); // TODO: use proper EEO_ values (such as optimize)
                ekFree(code);
                if(resultArray->type == EVT_ARRAY)
                {
                    if(ekArraySize(E, &resultArray->arrayVal) > 0)
                    {
                        thisPtr = resultArray->arrayVal[0];
                        ekValueAddRefNote(E, thisPtr, "require thisPtr");
                    }
                }
            }
        }
        ekStringClear(E, &filename);
        if(thisPtr == NULL)
        {
            ekContextSetError(E, EVE_RUNTIME, "require() failed to load '%s'", ekValueSafeStr(path));
            thisPtr = ekValueNullPtr;
        }
    }
    ekValueRemoveRefNote(E, path, "require path done");
    ekValueRemoveRefNote(E, resultArray, "require resultArray done");
    ekContextReturn(E, thisPtr);
}

// ---------------------------------------------------------------------------

void ekIntrinsicsRegister(struct ekContext *E)
{
    // Generic calls
    ekContextAddIntrinsic(E, "eval", ekiEval);
    ekContextAddIntrinsic(E, "assert", ekiAssert);

    // Value related
    ekContextAddIntrinsic(E, "type", ekiType);
    ekContextAddIntrinsic(E, "length", ekiLength);
    ekContextAddIntrinsic(E, "reverse", ekiReverse);
    ekContextAddIntrinsic(E, "dump", ekiDump);

    // Iterators
    ekContextAddIntrinsic(E, "iterator", ekiIterator);
    ekContextAddIntrinsic(E, "range", ekiRange);

    // TODO: Move out of here
    ekContextAddIntrinsic(E, "print", ekiPrint);
    ekContextAddIntrinsic(E, "require", ekiRequire);
}
