// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#include "yapiCore.h"

#include "yapTypes.h"
#include "yapHash.h"
#include "yapObject.h"
#include "yapValue.h"
#include "yapContext.h"

#include <stdio.h>

static yU32 make_array(struct yapContext *Y, yU32 argCount)
{
    yapValue *a;
    a = yapValueCreateArray(Y);
    if(argCount)
    {
        int i;
        for(i = 0; i < argCount; i++)
        {
            yapValue *v = yapContextGetArg(Y, i, argCount);
            yapValueArrayPush(Y, a, v);
            yapValueAddRefNote(Y, v, "make_array push");
        }
        yapContextPopValues(Y, argCount);
    }
    yap2ArrayPush(Y, &Y->stack, a);
    return 1;
}

yU32 array_push(struct yapContext *Y, yU32 argCount)
{
    int i;
    yapValue *a;
    yapArray values = {0};
    if(!yapContextGetArgs(Y, argCount, "a.", &a, &values))
    {
        return yapContextArgsFailure(Y, argCount, "push([array] a, ... values)");
    }

    for(i=0; i<values.count; i++)
    {
        yapValue *v = (yapValue *)values.data[i];
        yapValueArrayPush(Y, a, v);
    }

    yapValueRemoveRefNote(Y, a, "array_push a done");
    yapArrayClear(Y, &values, NULL);
    return 0;
}

yU32 length(struct yapContext *Y, yU32 argCount)
{
    yapValue *a;
    yapValue *c = yapValueNullPtr;
    if(!yapContextGetArgs(Y, argCount, "a", &a))
    {
        return yapContextArgsFailure(Y, argCount, "length([array] a)");
    }

    c = yapValueCreateInt(Y, yap2ArraySize(Y, &a->arrayVal));
    yapValueRemoveRefNote(Y, a, "length a done");
    yap2ArrayPush(Y, &Y->stack, c);
    return 1;
}

static void yapAppendKey(struct yapContext *Y, yapValue *arrayVal, yapHashEntry *entry)
{
    yapValue *keyVal = yapValueCreateString(Y, entry->key);
    yapValueArrayPush(Y, arrayVal, keyVal);
}

static yU32 keys(struct yapContext *Y, yU32 argCount)
{
    yapValue *object;
    yapValue *arrayVal = yapValueCreateArray(Y);

    if(!yapContextGetArgs(Y, argCount, "o", &object))
    {
        return yapContextArgsFailure(Y, argCount, "keys([object/dict] o)");
    }

    yapHashIterateP1(Y, object->objectVal->hash, (yapIterateCB1)yapAppendKey, arrayVal);

    yapValueRemoveRefNote(Y, object, "keys object done");
    yap2ArrayPush(Y, &Y->stack, arrayVal);
    return 1;
}

static yU32 eval(struct yapContext *Y, yU32 argCount)
{
    yapValue *ret = NULL;
    if(argCount)
    {
        int i;
        yapValue *v = yapContextGetArg(Y, 0, argCount);
        for(i = 0; i < argCount; i++)
        {
            yapValue *v = yapContextGetArg(Y, i, argCount);
            if(v->type == YVT_STRING)
            {
                yapContextEval(Y, yapStringSafePtr(&v->stringVal), 0);
                if(Y->error)
                {
                    // steal the error from the VM so we can recover and THEN give it back as a yapValue
                    char *error = Y->error;
                    Y->error = NULL;

                    yapContextRecover(Y);
                    ret = yapValueCreateString(Y, error);
                    yapFree(error);
                }
            }
        }
        yapContextPopValues(Y, argCount);
    }
    if(!ret)
    {
        ret = yapValueCreateInt(Y, 0);
    }
    yap2ArrayPush(Y, &Y->stack, ret);
    return 1;
}

// ---------------------------------------------------------------------------
// global print() funcs -- someday to be moved into an optional lib

static yU32 standard_print(struct yapContext *Y, yU32 argCount)
{
    if(argCount)
    {
        int i;
        for(i = 0; i < argCount; i++)
        {
            yapValue *v = yapContextGetArg(Y, i, argCount);
            switch(v->type)
            {
                case YVT_STRING:
                    printf("%s", yapStringSafePtr(&v->stringVal));
                    break;
                case YVT_INT:
                    printf("%d", v->intVal);
                    break;
                case YVT_FLOAT:
                    printf("%f", v->floatVal);
                    break;
                default:
                    v = yapValueToString(Y, v);
                    printf("%s", yapStringSafePtr(&v->stringVal));
                    yapValueRemoveRefNote(Y, v, "done with temp string (default)");
                    break;
            };
        }
        yapContextPopValues(Y, argCount);
    }
    else
    {
        printf("(printing nothing)\n");
    }
    return 0;
}

// ---------------------------------------------------------------------------
// import()

static char *loadFile(struct yapContext *Y, const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if(f)
    {
        int size;
        char *buffer;

        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);

        buffer = (char *)yapAlloc(size + 1);
        fread(buffer, 1, size, f);
        buffer[size] = 0;

        fclose(f);

        return buffer;
    }
    return NULL;
}

static yU32 import(struct yapContext *Y, yU32 argCount)
{
    yU32 ret;
    char *code;
    yapValue *filenameValue = NULL;
    yapValue *codeValue = NULL;

    if(argCount)
    {
        filenameValue = yapContextGetArg(Y, 0, argCount);
        if(filenameValue->type != YVT_STRING)
        {
            filenameValue = NULL;
        }
    }

    if(filenameValue)
    {
        code = loadFile(Y, yapStringSafePtr(&filenameValue->stringVal));
    }

    yapContextPopValues(Y, argCount);

    if(!filenameValue)
    {
        yapValue *reason = yapValueCreateString(Y, "import() takes a single string argument");
        yap2ArrayPush(Y, &Y->stack, reason);
        return 1;
    }

    if(!code)
    {
        yapValue *reason = yapValueCreateString(Y, "can't read file");
        yap2ArrayPush(Y, &Y->stack, reason);
        return 1;
    }

    codeValue = yapValueCreateKString(Y, code);
    yap2ArrayPush(Y, &Y->stack, codeValue);
    ret = eval(Y, 1);
    yapFree(code);
    return ret;
}

// ---------------------------------------------------------------------------

yU32 type(struct yapContext *Y, yU32 argCount)
{
    if(argCount)
    {
        yapValue *a = yapContextGetArg(Y, 0, argCount);
        yapValue *ret = yapValueCreateKString(Y, (char *)yapValueTypeName(Y, a->type));
        yapContextPopValues(Y, argCount);
        yap2ArrayPush(Y, &Y->stack, ret);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------

yU32 dump(struct yapContext *Y, yU32 argCount)
{
    if(argCount)
    {
        yapValue *a = yapContextGetArg(Y, 0, argCount);
        yapDumpParams *params = yapDumpParamsCreate(Y);
        yapValue *ret = yapValueCreateKString(Y, "");

        yapValueDump(Y, params, a);
        yapStringDonateStr(Y, &ret->stringVal, &params->output);

        yapDumpParamsDestroy(Y, params);
        yapContextPopValues(Y, argCount);
        yap2ArrayPush(Y, &Y->stack, ret);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------

yU32 yap_assert(struct yapContext *Y, yU32 argCount)
{
    yBool doAssert = yTrue;
    yapValue *v = NULL;
    yapValue *s = NULL;
    if(!yapContextGetArgs(Y, argCount, "|?s", &v, &s))
    {
        return yapContextArgsFailure(Y, argCount, "assert(expr, [string] explanation)");
    }

    if(v)
    {
        v = yapValueToInt(Y, v);
        doAssert = (!v->intVal) ? yTrue : yFalse;
        yapValueRemoveRefNote(Y, v, "doAssert temp");
    }

    if(doAssert)
    {
        const char *reason = "(unknown)";
        if(s && s->type == YVT_STRING)
        {
            reason = yapStringSafePtr(&s->stringVal);
        }
        yapContextSetError(Y, YVE_RUNTIME, "Yap Runtime Assert: %s", reason);
    }
    if(s)
    {
        yapValueRemoveRefNote(Y, s, "temporary reason string");
    }
    return 0;
}

// ---------------------------------------------------------------------------

void yapIntrinsicsRegisterCore(struct yapContext *Y)
{
    yapContextRegisterGlobalFunction(Y, "array", make_array);
    yapContextRegisterGlobalFunction(Y, "length", length);
    yapContextRegisterGlobalFunction(Y, "push", array_push);

    yapContextRegisterGlobalFunction(Y, "keys", keys);
    yapContextRegisterGlobalFunction(Y, "eval", eval);
    yapContextRegisterGlobalFunction(Y, "type", type);
    yapContextRegisterGlobalFunction(Y, "dump", dump);
    yapContextRegisterGlobalFunction(Y, "assert", yap_assert);

    // TODO: Move these out of here
    yapContextRegisterGlobalFunction(Y, "print", standard_print);
    yapContextRegisterGlobalFunction(Y, "import", import);
}
