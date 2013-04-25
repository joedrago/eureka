// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "ekIntrinsics.h"

#include "ekTypes.h"
#include "ekFrame.h"
#include "ekMap.h"
#include "ekObject.h"
#include "ekValue.h"
#include "ekContext.h"

#include <stdio.h>

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
                ekContextEval(E, ekStringSafePtr(&v->stringVal), 0);
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

static ekU32 ekiImport(struct ekContext *E, ekU32 argCount)
{
    ekU32 ret;
    char *code;
    ekValue *filenameValue = NULL;
    ekValue *codeValue = NULL;

    if(argCount)
    {
        filenameValue = ekContextGetArg(E, 0, argCount);
        if(filenameValue->type != EVT_STRING)
        {
            filenameValue = NULL;
        }
    }

    if(filenameValue)
    {
        code = loadFile(E, ekStringSafePtr(&filenameValue->stringVal));
    }

    ekContextPopValues(E, argCount);

    if(!filenameValue)
    {
        ekValue *reason = ekValueCreateString(E, "import() takes a single string argument");
        ekArrayPush(E, &E->stack, reason);
        return 1;
    }

    if(!code)
    {
        ekValue *reason = ekValueCreateString(E, "can't read file");
        ekArrayPush(E, &E->stack, reason);
        return 1;
    }

    codeValue = ekValueCreateKString(E, code);
    ekArrayPush(E, &E->stack, codeValue);
    ret = ekiEval(E, 1);
    ekFree(code);
    return ret;
}


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

    // TODO: Move these out of here
    ekContextAddIntrinsic(E, "print", ekiPrint);
    ekContextAddIntrinsic(E, "import", ekiImport);
}
